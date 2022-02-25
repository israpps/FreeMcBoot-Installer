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

#define INSTALL_MODE_FLAG_SKIP_CNF	0x01
#define INSTALL_MODE_FLAG_MULTI_INST	0x02
#define INSTALL_MODE_FLAG_CROSS_REG	0x04	//Note: This cannot be set with INSTALL_MODE_FLAG_MULTI_INST together. It cannot be set with INSTALL_MODE_FLAG_CROSS_MODEL either.
#define INSTALL_MODE_FLAG_CROSS_MODEL	0x08	//Note: This cannot be set with INSTALL_MODE_FLAG_MULTI_INST together. It cannot be set with INSTALL_MODE_FLAG_CROSS_REG either.
#define INSTALL_MODE_FLAG_CROSS_PSX	0x10
#define INSTALL_MODE_FLAG_SKIP_CLEANUP	0x20

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

