enum BootDeviceIDs{
	BOOT_DEVICE_UNKNOWN = -1,
	BOOT_DEVICE_MASS,

	BOOT_DEVICE_COUNT,
};

enum CONSOLE_REGION{
	CONSOLE_REGION_JAPAN	= 0,
	CONSOLE_REGION_USA,	//USA and Asia
	CONSOLE_REGION_EUROPE,
	CONSOLE_REGION_CHINA,

	CONSOLE_REGION_COUNT
};

#define FILE_IS_KELF		0x01

enum EXTENDED_ERROR_CODES{
	EEXTMGSIGNERR	 = 0x1000,
	EEXTCRSLNKFAIL,
	EEXTCACHEINITERR,	//MCTools cache initialization error.
	EEXTUNSUPPUNINST	//Unsupported uninstallation file.
};

//Bits for indicating the side the error is on.
#define ERROR_SIDE_DST	0x4000
#define ERROR_SIDE_SRC	0x8000

struct InstallationFile{
	const char *SrcRelPath;
	char *DestRelPath;
	unsigned int flags;
};
#define SHIFT_FLAG(x) (1 << x)
enum INSTALL_MODE_FLAGS
{
	INSTALL_MODE_FLAG_SKIP_CNF		= SHIFT_FLAG(0),
	INSTALL_MODE_FLAG_MULTI_INST	= SHIFT_FLAG(1),
	INSTALL_MODE_FLAG_CROSS_REG		= SHIFT_FLAG(2),	//Note: This cannot be set with INSTALL_MODE_FLAG_MULTI_INST, INSTALL_MODE_FLAG_CROSS_MODEL, INSTALL_MODE_FLAG_REGION_JAP, INSTALL_MODE_FLAG_REGION_CHI, INSTALL_MODE_FLAG_REGION_EUR or INSTALL_MODE_FLAG_REGION_USA.
	INSTALL_MODE_FLAG_CROSS_MODEL	= SHIFT_FLAG(3),	//Note: This cannot be set with INSTALL_MODE_FLAG_MULTI_INST or INSTALL_MODE_FLAG_CROSS_REG .
	INSTALL_MODE_FLAG_CROSS_PSX		= SHIFT_FLAG(4),
	INSTALL_MODE_FLAG_SKIP_CLEANUP	= SHIFT_FLAG(5),
	INSTALL_MODE_FLAG_REGION_JAP	= SHIFT_FLAG(6),	// same as INSTALL_MODE_FLAG_CROSS_MODEL (actually, that flag has to be set in conjunction) the difference is that this flag is used to modify the target region into another one chosen by user, instead of the console's one
	INSTALL_MODE_FLAG_REGION_CHI	= SHIFT_FLAG(7),	// same as INSTALL_MODE_FLAG_CROSS_MODEL (actually, that flag has to be set in conjunction) the difference is that this flag is used to modify the target region into another one chosen by user, instead of the console's one
	INSTALL_MODE_FLAG_REGION_EUR	= SHIFT_FLAG(8),	// same as INSTALL_MODE_FLAG_CROSS_MODEL (actually, that flag has to be set in conjunction) the difference is that this flag is used to modify the target region into another one chosen by user, instead of the console's one
	INSTALL_MODE_FLAG_REGION_USA	= SHIFT_FLAG(9),	// same as INSTALL_MODE_FLAG_CROSS_MODEL (actually, that flag has to be set in conjunction) the difference is that this flag is used to modify the target region into another one chosen by user, instead of the console's one
};



struct FileCopyTarget{
	char *source, *target;
	unsigned int size, mode, flags;
	void *buffer;
};

#define UNINST_FILE_VERSION	0x110
#define UNINST_FILE_VERSION_100	0x100	//Old uninstallation file format.

/*	The structure of the v1.10 multi-installation uninstallation data file is as follows:
 *
 *		0x00	- File Header
 *		0x04	- ROM version of the console used for installation.
 *		0x08	- Region code of the console used for installation.
 *		0x09	- Number of cross-linked files listed in the file.
 *		0x0A+	- The path of the cross-linked files, 66-bytes for each record (see the name field of the FileAlias structure within MCTOOLS/mctools.h).
 */

struct UninstallationDataFileHeader{
	char signature[2];		/* "FB" */
	unsigned short int version;
};

enum PlayStation2SystemType{
	PS2_SYSTEM_TYPE_PS2	= 0,
	PS2_SYSTEM_TYPE_PSX,	//DVR unit
	PS2_SYSTEM_TYPE_DEX	//DebugStation unit
};

enum WorkerThreadCommandCodes{
	WORKER_THREAD_CMD_NONE	= 0,
	WORKER_THREAD_CMD_DUMP_MC,
	WORKER_THREAD_CMD_RESTORE_MC,
	WORKER_THREAD_CMD_STOP	//Stop ongoing operations.
};

enum WorkerThreadResultCodes{
	WORKER_THREAD_RES_ERR	= -1,	//Error codes are all <0.
	WORKER_THREAD_RES_OK	= 0,
	WORKER_THREAD_RES_CMD_OK,	//Command acknowledged and understood.
	WORKER_THREAD_RES_BSY,
	WORKER_THREAD_RES_ABRT
};

struct WorkerThreadMcMaintParams{	//MC Maintanence parameters. For MC dumping and restoration.
	int port, slot;
	FILE *file;
	unsigned int ClusterSize;	//Number of pages cluster. Used only in a read operation.
	const struct MCTools_McSpecData *McSpecData;
};

/* Function prototypes */
int GetBootDeviceID(void);
int GetConsoleRegion(void);
int GetConsoleVMode(void);
int CleanupTarget(int port, int slot);
int CleanupHDDTarget(void);
int PerformHDDInstallation(unsigned int flags);
int PerformInstallation(unsigned char port, unsigned char slot, unsigned int flags);
void UpdateRegionalPaths(void);
int GetPs2Type(void);
int HasOldFMCBConfigFileOnHDD(void);
int HasOldFMCBConfigFile(int port, int slot);
int HasOldMultiInstall(int port, int slot);
int CleanupMultiInstallation(int port, int slot);
int PerformMemoryCardDump(int port, int slot);
int PerformMemoryCardRestore(int port, int slot);

int CheckPrerequisites(const struct McData *McData, unsigned char OperationMode);
int GetNumMemcardsInserted(struct McData *McData);

int SysCreateThread(void *function, void *stack, unsigned int StackSize, void *arg, int priority);

int StartWorkerThread(void);
void StopWorkerThread(void);
int SendWorkerThreadCommand(int command, const void *arg);
int GetWorkerThreadState(void);
float GetWorkerThreadProgress(void);

int IsHDDBootingEnabled(void);

int CreateAPPSPartition(void);
int WriteAPPSPartitionAttributes(void);
void DeleteAPPSPartition(void);

int IsUnsupportedModel(void);
int IsRareModel(void);
int HDDCheckSMARTStatus(void);
int HDDCheckSectorErrorStatus(void);
int HDDCheckPartErrorStatus(void);
int HDDCheckHasSpace(unsigned int PartSize);
int HDDCheckStatus(void);

void poweroffCallback(void *arg);

