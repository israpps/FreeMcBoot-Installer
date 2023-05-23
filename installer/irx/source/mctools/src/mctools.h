/* RPC function numbers */
#define MCTOOLS_CreateCrossLinkedEnts	0x00
#define MCTOOLS_DeleteCrossLinkedEnts	0x01
#define MCTOOLS_GetMCInfo		0x02
#define MCTOOLS_ReadCluster		0x03
#define MCTOOLS_WriteBlock		0x04
#define MCTOOLS_Init			0x80
#define MCTOOLS_InitPageCache		0x81
#define MCTOOLS_FlushPageCache		0x82
#define MCTOOLS_FlushMCMANClusterCache	0x83

#define MCTOOLS_RPC_NUM		0x00001906

#define MAX_RPC_FILE_ENTRIES	8

/* 340 bytes long */
struct SCE_MC_Superblock{
	u8 magic[28];		/* "Sony PS2 Memory Card Format" */
	u8 version[12];		/* E.g. "1.X.0.0". Version 1.2 indicates full support for the bad_block_list */
	u16 page_len;		/* Length of each page in bytes. */
	u16 pages_per_cluster;
	u16 pages_per_block;	/* Must be 16 (?) */
	u16 unknown;		/* Unknown. Value: 0xFF00 */
	u32 clusters;		/* Total size of the card in clusters. */
	u32 alloc_start;	/* The cluster number of the first allocatable cluster (The cluster immediately after the FAT) */
	u32 alloc_end;		/* The cluster number of the cluster after the highest-allocatable cluster. Not used. */
	u32 rootdir_cluster;	/* The first cluster representing the root directory relative to alloc_start. Must be zero. */
	u32 backup_block1;	/* Erase block used as a backup area during programming. */
	u32 backup_block2;	/* A block with it's content set to 1 during programming. */
	u8 unknown2[8];		/* Unused/unknown region. */
	u32 ifc_list[32];	/* List of indirect FAT clusters (On a standard 8M card there's only one indirect FAT cluster). */
	u32 bad_block_list[32];	/* List of erase blocks that have errors and shouldn't be used. */
	u8 card_type;		/* Memory card type (2 = Playstation 2 Memory Card). */
	u8 card_flags;		/* Physical characteristics of the memory card. */
};

/* 512 bytes long (Occupies one page). 96 bytes long here. */
struct SCE_MC_dirent{
	u16 mode;	/* Attributes of this directory entry. */
	u16 unknown;
	u32 length;	/* Length in bytes if a file, or entries if a directory. */
	u8 created[8];	/* Creation time. */
	u32 cluster;	/* First cluster of the file, or 0xFFFFFFFF for an empty file. In "." entries this the first cluster of this directory's parent directory instead. Relative to alloc_offset. */
	u32 dir_entry;	/* Only in "." entries. Entry of this directory in its parent's directory. */
	u8 modified[8];	/* Modification time. */
	u32 attr;	/* User defined attribute */
	u8 unknown2[28];
	u8 name[32];	/* Zero terminated name for this directory entry. */
};

struct FileAlias{
	u16 mode;
	s8 name[66];	//Note: Do not just update this! This affects the size of each record within the uninstallation file, and changing this may break older installers that expect the original size. Instead, update this and the uninstallation file's version number together to lock out the older installers.
};

/* RPC data structures. */
struct MCTools_MCMANFlushCommandData{
	s32 port, slot;
};

struct MCTools_CrossLinkedEntRPCData{
	u8 port;
	u8 slot;
	s8 path[256];
	struct FileAlias Aliases[MAX_RPC_FILE_ENTRIES];
};

struct MCTools_McSpecData{
	u16 PageSize, BlockSize;
	int CardSize;
};

struct MCTools_GetMCInfoRPCReqData{
	u8 port;
	u8 slot;
	struct MCTools_McSpecData *SpecData;	//On the EE
};

struct MCTools_GetMCInfoRPCResData{
	s32 result;
	struct MCTools_McSpecData SpecData;
};

struct MCTools_ReadWriteRPCData{
	s32 port, slot;
	u16 PageSize, BlockSize;	//Or cluster size, when reading.
	u32 offset;	/* In blocks, clusters or pages. Depending on the RPC function used. */
	void *EEbuffer;
	void *IOPbuffer;
};

struct MCTools_CacheRPCData{
	u16 port;
	u16 slot;
	struct MCTools_McSpecData SpecData;
};

struct MCTools_AlignmentData{
	void *buffer1Address;
	void *buffer2Address;

	u8 buffer1[64];
	u8 buffer2[64];

	u32 buffer1_len;
	u32 buffer2_len;
};
