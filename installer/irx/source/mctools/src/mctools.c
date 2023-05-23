#include <errno.h>
#include <irx.h>
#include <intrman.h>
#include <loadcore.h>
#include <mcman.h>
#include <sifcmd.h>
#include <sifman.h>
#include <sysclib.h>
#include <sysmem.h>
#include <stdio.h>
#include <thbase.h>
#include <types.h>

#include "mctools_internal.h"
#include "mctools.h"
#include "cache.h"
#include "MCIO.h"

#define MODNAME "mctools_ee_driver"
IRX_ID(MODNAME, 0x01, 0x21);

/* Data used for registering the RPC servers */
static SifRpcServerData_t rpc_sdata;
static unsigned char rpc_buffer[1024];
static unsigned char rpc_TX_buffer[16];
static SifRpcDataQueue_t rpc_qdata;

static struct MCTools_AlignmentData *EEAlignmentDataStructure;

/* Function prototypes */
static void mctools_RPC_srv(void *args);
static void *mctools_rpc_handler(int fno, void *buf, int size);

/* Common thread creation data */
static iop_thread_t thread_data={
	TH_C,			/* rpc_thp.attr */
	0,			/* rpc_thp.option */
	&mctools_RPC_srv,	/* rpc_thp.thread */
	0x1000,			/* rpc_thp.stacksize */
	0x66,			/* rpc_thp.piority */
};

/* Entry point */
int _start(int argc,char *argv[])
{
	printf("MCTOOLs start.\n");

	StartThread(CreateThread(&thread_data), NULL);

	return MODULE_RESIDENT_END;
}

void *malloc(unsigned int size){
	int oldstate;
	void *result;

	CpuSuspendIntr(&oldstate);
	result=AllocSysMemory(ALLOC_FIRST, size, NULL);
	CpuResumeIntr(oldstate);

	return result;
}

void free(void *ptr){
	int oldstate;

	CpuSuspendIntr(&oldstate);
	FreeSysMemory(ptr);
	CpuResumeIntr(oldstate);
}

static inline void DMATransferDataToEE(void *src, void *dest, int size, struct MCTools_AlignmentData *AlignmentData){
	SifDmaTransfer_t dmat[2];
	int DMATransferID, oldstate;

	dmat[0].src=src;
	dmat[0].dest=dest;
	dmat[0].size=size;
	dmat[1].attr=dmat[0].attr=0;

	dmat[1].src=AlignmentData;
	dmat[1].dest=EEAlignmentDataStructure;
	dmat[1].size=sizeof(struct MCTools_AlignmentData);

	CpuSuspendIntr(&oldstate);
	while((DMATransferID=sceSifSetDma(dmat, 2))==0);
	while(sceSifDmaStat(DMATransferID)>=0){};
	CpuResumeIntr(oldstate);
}

static void EE_memcpy(void *src, void *dest, int size){
	void *FinalDestPtr;
	int DMATransferLen;
	unsigned char AlignmentBufferLen;
	struct MCTools_AlignmentData AlignmentData;

	FinalDestPtr=dest;
	DMATransferLen=size;

	memset(&AlignmentData, 0, sizeof(struct MCTools_AlignmentData));

	if((u32)dest&0x3F){
		FinalDestPtr=(void *)(((u32)dest+0x3F)&~0x3F);
		AlignmentBufferLen=(u32)FinalDestPtr-(u32)dest;
		memcpy(AlignmentData.buffer1, src, AlignmentBufferLen);
		AlignmentData.buffer1_len=AlignmentBufferLen;
		AlignmentData.buffer1Address=dest;
		DMATransferLen-=AlignmentBufferLen;
	}

	if(DMATransferLen&0x3F){
		AlignmentBufferLen=DMATransferLen&0x3F;
		memcpy(AlignmentData.buffer2, &((unsigned char *)src)[size-AlignmentBufferLen], AlignmentBufferLen);
		AlignmentData.buffer2_len=AlignmentBufferLen;
		AlignmentData.buffer2Address=&((unsigned char *)dest)[size-AlignmentBufferLen];
		DMATransferLen-=AlignmentBufferLen;
	}

	DMATransferDataToEE((unsigned char *)src+AlignmentData.buffer1_len, FinalDestPtr, DMATransferLen, &AlignmentData);
}

static void mctools_RPC_srv(void *args){
	sceSifSetRpcQueue(&rpc_qdata, GetThreadId());

	sceSifRegisterRpc(&rpc_sdata, MCTOOLS_RPC_NUM, &mctools_rpc_handler, rpc_buffer, NULL, NULL, &rpc_qdata);
	sceSifRpcLoop(&rpc_qdata);
}

static int FlushMCMANClusterCache(int port, int slot){
	//While file should not be opened while MCTOOLs is running (MCMAN's cache cannot be invalidated, by design), these steps are safeguards.
	McCloseAll();
	McFlushCache(port, slot);

	return 0;
}

static int UpdateFatTableIndexData(unsigned char port, unsigned char slot, unsigned int index, unsigned int data, struct SCE_MC_Superblock *SuperBlock){
	unsigned int NumEntriesPerFatCluster, *IndirectFatIndexTable, *FatTable;
	unsigned int IndirectFATIndex, IndirectFATIndexOffset, IndirectFATDoubleIndex;
	int result;

	if(index>=SuperBlock->clusters){
		return -EINVAL;
	}

	/*	Unlike FAT12/16/32, the FAT table is not contiguous. ifc_list contains the indexes of the FAT clusters,
		hence double-indexing is required for accessing the clusters within the FAT. */

	result=-1;
	NumEntriesPerFatCluster=SuperBlock->pages_per_cluster*SuperBlock->page_len/4;	/* Calculate the number of FAT entries contained within each FAT cluster. */
	IndirectFATIndex = index / NumEntriesPerFatCluster;				/* Calculate the index within the indirect FAT table, that the FAT record resides in. */
	IndirectFATIndexOffset = IndirectFATIndex % NumEntriesPerFatCluster;		/* Calculate the offset of the indirect FAT index table to retrieve from (It contains the FAT cluster number). */
	IndirectFATDoubleIndex = IndirectFATIndex / NumEntriesPerFatCluster;		/* Calculate the index of the entry to retrieve from the table containing the cluster numbers of the clusters that make up the indirect FAT index table. */

	IndirectFatIndexTable=malloc(SuperBlock->pages_per_cluster*SuperBlock->page_len);
	FatTable=malloc(SuperBlock->pages_per_cluster*SuperBlock->page_len);

	if(ReadCluster(SuperBlock->page_len, SuperBlock->pages_per_cluster, SuperBlock->ifc_list[IndirectFATDoubleIndex], IndirectFatIndexTable)>=0){
		/* Update the FAT. */
		if(ReadCluster(SuperBlock->page_len, SuperBlock->pages_per_cluster, IndirectFatIndexTable[IndirectFATIndexOffset], FatTable)>=0){
			FatTable[index % NumEntriesPerFatCluster]=data;
			result=WriteCluster(SuperBlock->page_len, SuperBlock->pages_per_cluster, IndirectFatIndexTable[IndirectFATIndexOffset], FatTable);
		}
		else DEBUG_PRINTF("Read fault while reading FAT cluster 0x%08x\n", IndirectFatIndexTable[IndirectFATIndexOffset]);
	}
	else DEBUG_PRINTF("Read fault while reading the indirect FAT index table\n");

	free(IndirectFatIndexTable);
	free(FatTable);

	return result;
}

/* Returns the specifications of the card (And it's filesystem) present in the specified port and slot. */
static int GetCardFilesystemSuperblock(int port, int slot, struct SCE_MC_Superblock *SuperBlock){
	unsigned char *buffer;
	int result;

	DEBUG_PRINTF("GetCardFilesystemSuperblock() - Port %d Slot %d\n", port, slot);

	result=-ENOMEM;
	if((buffer=malloc(1024))!=NULL){	/* Note: The maximum page size should only be 1024 bytes. */
		if((result=McReadPage(port, slot, 0, buffer))==sceMcResSucceed){
			memcpy(SuperBlock, buffer, sizeof(struct SCE_MC_Superblock));
		}

		free(buffer);
	}

	return(result);
}

static int GetCardSpecs(int port, int slot, u16 *PageSize, u16 *BlockSize, int *CardSize){
	u8 flags;
	int result;

	result=McGetCardSpec(port, slot, PageSize, BlockSize, CardSize, &flags);
	DEBUG_PRINTF("GetCardSpecs: port: %d, slot: %d, PageSize: %u, BlockSize: %u, CardSize: %u\n", port, slot, *PageSize, *BlockSize, *CardSize);

	return result;
}

static int GetFatTableIndexData(unsigned char port, unsigned char slot, unsigned int index, struct SCE_MC_Superblock *SuperBlock, unsigned int *NextClusterNum){
	unsigned int NumEntriesPerFatCluster, *IndirectFatIndexTable, *FatTable;
	unsigned int IndirectFATIndex, IndirectFATIndexOffset, IndirectFATDoubleIndex;
	int result;

	if(index>=SuperBlock->clusters){
		return -EINVAL;
	}

	/*	Unlike FAT12/16/32, the FAT table is not contiguous. ifc_list contains the indexes of the FAT clusters,
		hence double-indexing is required for accessing the clusters within the FAT. */

	result=-1;
	NumEntriesPerFatCluster=SuperBlock->pages_per_cluster*SuperBlock->page_len/4;	/* Calculate the number of FAT entries contained within each FAT cluster. */
	IndirectFATIndex = index / NumEntriesPerFatCluster;				/* Calculate the index within the indirect FAT table, that the FAT record resides in. */
	IndirectFATIndexOffset = IndirectFATIndex % NumEntriesPerFatCluster;		/* Calculate the offset of the indirect FAT index table to retrieve from (It contains the FAT cluster number). */				
	IndirectFATDoubleIndex = IndirectFATIndex / NumEntriesPerFatCluster;		/* Calculate the index of the entry to retrieve from the table containing the cluster numbers of the clusters that make up the indirect FAT index table. */

	if((IndirectFatIndexTable=malloc(SuperBlock->pages_per_cluster*SuperBlock->page_len))!=NULL){
		if((FatTable=malloc(SuperBlock->pages_per_cluster*SuperBlock->page_len))!=NULL){

			if((result=ReadCluster(SuperBlock->page_len, SuperBlock->pages_per_cluster, SuperBlock->ifc_list[IndirectFATDoubleIndex], IndirectFatIndexTable))>=0){
				if((result=ReadCluster(SuperBlock->page_len, SuperBlock->pages_per_cluster, IndirectFatIndexTable[IndirectFATIndexOffset], FatTable))>=0){
					*NextClusterNum=FatTable[index % NumEntriesPerFatCluster];	/* Retrieve the next index contained within the FAT. */
					result=0;
				}
				else DEBUG_PRINTF("Read fault while reading FAT cluster 0x%08x\n", IndirectFatIndexTable[IndirectFATIndexOffset]);
			}
			else DEBUG_PRINTF("Read fault while reading the indirect FAT index table\n");

			free(FatTable);
		}
		else result=-ENOMEM;

		free(IndirectFatIndexTable);
	}
	else result=-ENOMEM;

	return result;
}

static int GetFatTableNextIndexData(unsigned char port, unsigned char slot, unsigned int index, struct SCE_MC_Superblock *SuperBlock, unsigned int *NextClusterNum){
	int result;

	result=GetFatTableIndexData(port, slot, index, SuperBlock, NextClusterNum);
	if((result>=0) && (*NextClusterNum!=0xFFFFFFFF)) *NextClusterNum&=0x7FFFFFFF;

	return(result);
}

/*
 *	Adds a new cluster to an existing FAT record chain within the FAT.
 *	Returns the cluster number of the new allocated cluster. Returns 0 on error.
 */
static unsigned int AddFATRecord(unsigned char port, unsigned char slot, struct SCE_MC_Superblock *SuperBlock, unsigned int FATChainStartCluster){
	unsigned int NewRecClusterNum, ClusterNum, NextClusterNum;
	int result;

	/* Attempt to allocate a new cluster */
	for(NewRecClusterNum=0, result=0; result>=0; NewRecClusterNum++){
		if((result=GetFatTableIndexData(port, slot, NewRecClusterNum, SuperBlock, &NextClusterNum))>=0){
			if(!(NextClusterNum&0x80000000)){
				DEBUG_PRINTF("Free cluster found\n");
				break;
			}
		}
		else DEBUG_PRINTF("Error %d\n", result);
	}

	DEBUG_PRINTF("Free cluster at 0x%08x result %d\n", NewRecClusterNum, result);

	/* If a free cluster was identified */
	if(result>=0){
		DEBUG_PRINTF("FATChainStartCluster=0x%08x\n", FATChainStartCluster);

		/* Update the FAT chain by going through it to the end. */
		NextClusterNum=FATChainStartCluster;
		do{
			ClusterNum=NextClusterNum;
			if((result=GetFatTableNextIndexData(port, slot, ClusterNum, SuperBlock, &NextClusterNum))<0) break;
		}while(NextClusterNum!=0xFFFFFFFF);

		DEBUG_PRINTF("Begining write operations... ClusterNum=0x%08x\n", ClusterNum);

		if(result>=0){
			//Update the last cluster's record to point to the newly-allocated cluster.
			if((result=UpdateFatTableIndexData(port, slot, ClusterNum, 0x80000000|NewRecClusterNum, SuperBlock))>=0){
				//The new cluster will be at the end of the chain.
				if((result=UpdateFatTableIndexData(port, slot, NewRecClusterNum, 0xFFFFFFFF, SuperBlock))>=0){
					result=NewRecClusterNum;
				}
			}
		}
	}

	return result;
}

#if 0
/*
 *	Removes a cluster from an existing FAT record chain within the FAT.
 *	Returns the cluster number of the cluster deleted. Returns 0 on error.
 */
static unsigned int DeleteFATRecord(unsigned char port, unsigned char slot, struct SCE_MC_Superblock *SuperBlock, unsigned int FATChainStartCluster, unsigned int ClusterToDelete){
	unsigned int PrevRecClusterNum, RecClusterNum, NextRecClusterNum;
	unsigned int result;

	PrevRecClusterNum=NextRecClusterNum=RecClusterNum=FATChainStartCluster;
	result=0;

	while((RecClusterNum!=0xFFFFFFFF) && ((RecClusterNum&0x7FFFFFFF)!=ClusterToDelete)){
		PrevRecClusterNum=RecClusterNum;
		RecClusterNum=NextRecClusterNum;

		if(GetFatTableIndexData(port, slot, RecClusterNum&0x7FFFFFFF, SuperBlock, &NextRecClusterNum)<0){
			DEBUG_PRINTF("Error in GetFatTableIndexData()\n");
			goto DeleteFATRecord_end;
		}
	}

	if((RecClusterNum&0x7FFFFFFF)==ClusterToDelete){
		DEBUG_PRINTF("PrevRecClusterNum=0x%08x\nRecClusterNum=0x%08x\nNextRecClusterNum=0x%08x\n\n", PrevRecClusterNum, RecClusterNum, NextRecClusterNum);
		//Point the previous cluster's record to the one after it.
		if(UpdateFatTableIndexData(port, slot, PrevRecClusterNum&0x7FFFFFFF, NextRecClusterNum, SuperBlock)>=0){
			//Update the deleted record's next cluster address to unallocated.
			if(UpdateFatTableIndexData(port, slot, RecClusterNum&0x7FFFFFFF, NextRecClusterNum&0x7FFFFFFF, SuperBlock)>=0){
				result=ClusterToDelete;
			}
		}
	}

DeleteFATRecord_end:
	DEBUG_PRINTF("Delete result=0x%08x\n", result);

	return result;
}

static int FreeUnusedFATRecords(unsigned char port, unsigned char slot, u32 DirEntStartCluster, u32 *NumValidRecords, struct SCE_MC_Superblock *SuperBlock){
	int result;
	unsigned int LogicalPageNum, ClusterNum, PageNum, PrevClusterNum, recordID, NumDirEntRecords;
	unsigned char NumberOfUnusedPages, ClustersFreed, NumValidRecordsInCluster;
	void *PageBuffer;

	DEBUG_PRINTF("Freeing unused clusters...\n");

	if((PageBuffer=malloc(SuperBlock->page_len))==NULL){
		return -ENOMEM;
	}

	result=0;
	recordID=0;
	ClustersFreed=0;
	NumberOfUnusedPages=0;
	ClusterNum=DirEntStartCluster;
	PageNum=(SuperBlock->alloc_start+ClusterNum)*SuperBlock->pages_per_cluster;
	LogicalPageNum=0;
	NumDirEntRecords=*NumValidRecords;

	*NumValidRecords=0;
	NumValidRecordsInCluster=0;

	//Go through the FAT chain of the directory
	while((result>=0) && (ClusterNum!=0xFFFFFFFF)){
		if((recordID>0) && (LogicalPageNum==0)){
			PrevClusterNum=ClusterNum;

			/* Get the cluster number of the next cluster. Break out of the loop if the last cluster has been reached (In other words, the file was not found). */
			if(GetFatTableNextIndexData(port, slot, ClusterNum, SuperBlock, &ClusterNum)<0){
				break;
			}

			PageNum=(SuperBlock->alloc_start+ClusterNum)*SuperBlock->pages_per_cluster;

			//Count the number of unused pages in the cluster. If the whole cluster is unused, it can be freed.
			if(NumberOfUnusedPages==SuperBlock->pages_per_cluster){
				DEBUG_PRINTF("Attempting to delete cluster 0x%08x\n", PrevClusterNum);
				if(DeleteFATRecord(port, slot, SuperBlock, DirEntStartCluster, PrevClusterNum)!=PrevClusterNum){
					DEBUG_PRINTF("Error detected in DeleteFATRecord()\n");
					result=-1;
					break;
				}
				else{
					ClustersFreed++;
					(*NumValidRecords)-=NumValidRecordsInCluster;
				}
			}

			NumberOfUnusedPages=0;
			LogicalPageNum=0;
			NumValidRecordsInCluster=0;
		}

		//Only if the end of the chain has not been reached.
		if(ClusterNum!=0xFFFFFFFF){
			/* The actions in here are only valid for valid records (For both erased and existent).
				Note: There can be unused pages in the last cluster, but that page might not have a valid directory record.
				Don't treat it like it has valid information because it might even contain random garbage.
			*/
			if(recordID<NumDirEntRecords){
				if((result=ReadPageCached(PageNum+LogicalPageNum, PageBuffer))<0){
					DEBUG_PRINTF("Read fault. Page: %d Code: %d\n", PageNum+LogicalPageNum, result);
					break;
				}

				DEBUG_PRINTF("ClusterNum: 0x%08x LogicalPageNum=%d\n", ClusterNum, LogicalPageNum);
				DEBUG_PRINTF("name: %s\nmode: 0x%04x\n\n", ((struct SCE_MC_dirent *)PageBuffer)->name, ((struct SCE_MC_dirent *)PageBuffer)->mode);

				/* Count the number of consecutive pages that are unused. */
				if(!(((struct SCE_MC_dirent *)PageBuffer)->mode & 0x8000)){
					NumberOfUnusedPages++;
				}
				else{
					NumberOfUnusedPages=0;
				}

				NumValidRecordsInCluster++;
				(*NumValidRecords)++;
			}
			else NumberOfUnusedPages++;	/* Otherwise, remember to count the unused pages that are not part of this directory record list that exist in the last cluster! */

			recordID++;
			LogicalPageNum++;
			if(LogicalPageNum>=SuperBlock->pages_per_cluster) LogicalPageNum=0;
		}
	}

	free(PageBuffer);

	return((result>=0)?ClustersFreed:result);
}
#endif

static int UpdateDirEntInformation(unsigned char port, unsigned char slot, struct SCE_MC_Superblock *SuperBlock, unsigned int TargetCluster, unsigned int TargetOffsetWithinCluster, struct SCE_MC_dirent *DirEntData){
	unsigned int ClusterSize;
	int result;
	unsigned char *buffer;

	DEBUG_PRINTF("TargetCluster=0x%08x offset 0x%08x\n", TargetCluster, TargetOffsetWithinCluster);

	ClusterSize=SuperBlock->page_len*SuperBlock->pages_per_cluster;

	/* Now, read in, modify and write the required clusters back onto the Memory Card. */
	if((buffer=malloc(ClusterSize))!=NULL){
		if((result=ReadCluster(SuperBlock->page_len, SuperBlock->pages_per_cluster, TargetCluster, buffer))>=0){
			memcpy(&buffer[TargetOffsetWithinCluster], DirEntData, sizeof(struct SCE_MC_dirent));
			DEBUG_PRINTF("Name: %s size 0x%08x\n", ((struct SCE_MC_dirent *)&buffer[TargetOffsetWithinCluster])->name, ((struct SCE_MC_dirent *)&buffer[TargetOffsetWithinCluster])->length);

			result=WriteCluster(SuperBlock->page_len, SuperBlock->pages_per_cluster, TargetCluster, buffer);
		}

		free(buffer);
	}
	else result=-ENOMEM;

	return result;
}

static int ScanThroughDirEnt(unsigned char port, unsigned char slot, unsigned short int mode, unsigned int nRecords, unsigned int *PageNum, unsigned int *LogicalPageNum, const char *dirent_name, unsigned int *ClusterNum, struct SCE_MC_Superblock *SuperBlock, void *PageBuffer){
	unsigned int recordID;
	int result;

	recordID=0;
	result=0;

	DEBUG_PRINTF("ScanThroughDirEnt called.\n");

	/* Search through the file records for the file, going through all leading directories first. */
	do{
		if(recordID>0){
			(*LogicalPageNum)++;
			if(*LogicalPageNum>=SuperBlock->pages_per_cluster) *LogicalPageNum=0;

			if(*LogicalPageNum==0){
				/* Get the cluster number of the next cluster. Break out of the loop if the last cluster has been reached (In other words, the file was not found). */
				if(((result=GetFatTableNextIndexData(port, slot, *ClusterNum, SuperBlock, ClusterNum))<0) || (*ClusterNum==0xFFFFFFFF)){
					DEBUG_PRINTF("End of DirEnt reached.\n");
					result=-ENOENT;
					break;
				}

				*PageNum=(SuperBlock->alloc_start+*ClusterNum)*SuperBlock->pages_per_cluster;
			}
		}

		if((result=ReadPageCached(*PageNum+*LogicalPageNum, PageBuffer))<0){
			DEBUG_PRINTF("Read fault. Page: %d Code: %d\n", *PageNum+*LogicalPageNum, result);
			break;
		}

		if((nRecords<1) && (recordID==0)){
			nRecords=((struct SCE_MC_dirent *)PageBuffer)->length;
		}

		if(recordID>=nRecords){
			if((mode!=1)||(recordID%SuperBlock->pages_per_cluster)==0){
				if(mode!=1){
					DEBUG_PRINTF("Error: File not found.\n");
				}
				else{
					DEBUG_PRINTF("End of DirEnt reached. No free pages found.\n");
				}

				result=-ENOENT;
				break;
			}
			else{
				result=1;
				break;
			}
		}

		if(((struct SCE_MC_dirent *)PageBuffer)->mode & 0x8000){
			DEBUG_PRINTF("Name: %s\nCluster: 0x%08x\nsize 0x%08x\n\n", ((struct SCE_MC_dirent *)PageBuffer)->name, ((struct SCE_MC_dirent *)PageBuffer)->cluster, ((struct SCE_MC_dirent *)PageBuffer)->length);
		}
		else{
			if(mode==1){
				result=2;
				break;
			}
		}

		recordID++;
	}while((mode==1) || (!(((struct SCE_MC_dirent *)PageBuffer)->mode & 0x8000)) || strncmp(dirent_name, ((struct SCE_MC_dirent *)PageBuffer)->name, sizeof(((struct SCE_MC_dirent *)PageBuffer)->name))!=0);

	DEBUG_PRINTF("ScanThroughDirEnt end.\n");

	return result;
}

/*
 *	Returns (and/or sets) the information of a file. To search for the root folder, pass "." as the path.
 *	mode = 0 -> Normal operation
 *	mode = 1 -> Write file data into the first unused page detected.
 *
 *	On success >=0 is returned.
 *	Returns 1 when mode = 1 and the allocated page does not belong to any directory entry.
 *	Returns 2 when mode = 1 and the allocated page belonged to a deleted file.
 */
static int GetSetFileData(int port, int slot, int mode, const char *path, struct SCE_MC_dirent *entry, struct SCE_MC_dirent *NewDirEntData, struct SCE_MC_Superblock *SuperBlock){
	unsigned int ClusterNum, PageNum, LogicalPageNum, DirEntLength;
	int result, AllocatedPageType;
	char *dirent_name;
	char *PathToFile;
	void *PageBuffer;

	DEBUG_PRINTF("GetFileData(): File: %s mode: %d\n", path, mode);
	PathToFile=malloc(strlen(path)+1);
	strcpy(PathToFile, path);

	result=0;
	AllocatedPageType=0;
	if((PageBuffer=malloc(SuperBlock->page_len))!=NULL){
		dirent_name=strtok(PathToFile, "/\\");

		ClusterNum=SuperBlock->rootdir_cluster;
		DirEntLength=0;	/* Unknown size. The size will then be retrieved later on within ScanThroughDirEnt() */

		while((dirent_name!=NULL)&&(result>=0)){
			LogicalPageNum=0;

			if(ClusterNum!=0) ClusterNum=((struct SCE_MC_dirent *)PageBuffer)->cluster;
			PageNum=(SuperBlock->alloc_start+ClusterNum)*SuperBlock->pages_per_cluster;

			DEBUG_PRINTF("Scanning for dirent %s\n", dirent_name);

			if(ClusterNum==0xFFFFFFFF) break;	/* Do not enter the directory if it's empty (Or rather non-existent). */

			result=ScanThroughDirEnt(port, slot, 0, DirEntLength, &PageNum, &LogicalPageNum, dirent_name, &ClusterNum, SuperBlock, PageBuffer);

			if(result>=0){
				DEBUG_PRINTF("DirEnt found. Cluster: 0x%08x size 0x%08x\n", ((struct SCE_MC_dirent *)PageBuffer)->cluster, ((struct SCE_MC_dirent *)PageBuffer)->length);
				dirent_name=strtok(NULL, "/\\");
				DirEntLength=((struct SCE_MC_dirent *)PageBuffer)->length;
			}
		}

		if(result>=0){
			if(mode==1){
				if(ClusterNum!=0){
					ClusterNum=((struct SCE_MC_dirent *)PageBuffer)->cluster;
					PageNum=(SuperBlock->alloc_start+ClusterNum)*SuperBlock->pages_per_cluster;
				}

				LogicalPageNum=0;

				if((result=ScanThroughDirEnt(port, slot, mode, ((struct SCE_MC_dirent *)PageBuffer)->length, &PageNum, &LogicalPageNum, dirent_name, &ClusterNum, SuperBlock, PageBuffer))>=0){
					AllocatedPageType=result;
					DEBUG_PRINTF("Detected free page: 0x%08x type %d\n", PageNum+LogicalPageNum, AllocatedPageType);
				}
			}

			if(result>=0){
				DEBUG_PRINTF("Found file.\n\n");

				if(entry!=NULL){
					memcpy(entry, PageBuffer, sizeof(struct SCE_MC_dirent));
				}

				if(NewDirEntData!=NULL){
					unsigned int offset;

					offset=((PageNum+LogicalPageNum-SuperBlock->alloc_start*SuperBlock->pages_per_cluster)-ClusterNum*SuperBlock->pages_per_cluster)*SuperBlock->page_len;

					result=UpdateDirEntInformation(port, slot, SuperBlock, SuperBlock->alloc_start+ClusterNum, offset, NewDirEntData);
				}
			}
		}

		free(PageBuffer);
	}
	else result=-ENOMEM;

	free(PathToFile);

	if((mode==1) && (result>=0)){
		result=AllocatedPageType;
	}

	return result;
}

/*
 *	Returns the information of a file. To search for the root folder, pass "." as the path.
 */
static int GetFileData(int port, int slot, const char *path, struct SCE_MC_dirent *entry, struct SCE_MC_Superblock *SuperBlock){
	return(GetSetFileData(port, slot, 0, path, entry, NULL, SuperBlock));
}

static int CreateNewVirtualDirent(unsigned char port, unsigned char slot, const char *path, struct SCE_MC_dirent *NewDirEntData, struct SCE_MC_Superblock *SuperBlock){
	struct SCE_MC_dirent ParentDirEnt;
	unsigned int TargetCluster, TargetSubClusterPageNum, ClusterSize, TargetOffsetWithinCluster;
	int result;

	DEBUG_PRINTF("CreateNewVirtualDirent: %s/%s\n", path, NewDirEntData->name);

	ClusterSize=SuperBlock->page_len*SuperBlock->pages_per_cluster;

	/* Get some information on the parent folder. */
	if((result=GetFileData(port, slot, path, &ParentDirEnt, SuperBlock))>=0){
		/* Decide whether the new record goes onto a page within an existing cluster or goes onto a page within a new cluster.
		 *
		 * First, look for an unused page within the currently allocated clusters.
		 */

		/* Attempt to create the new directory record within the page in the cluster before writing the cluster back */
		if((result=GetSetFileData(port, slot, 1, path, NULL, NewDirEntData, SuperBlock))<0){
			DEBUG_PRINTF("No free pages. Allocating new cluster.\n");

			result=0;
			TargetSubClusterPageNum=0;

			/* Add a new record to the FAT. */
			if((TargetCluster=AddFATRecord(port, slot, SuperBlock, ParentDirEnt.cluster))==0){
				result=-ENOMEM;
			}

			DEBUG_PRINTF("Allocated new cluster: 0x%08x\n", TargetCluster);

			TargetCluster+=SuperBlock->alloc_start;

			if(result>=0){
				/* Place the new record on the calculated page within the calculated cluster. */
				TargetOffsetWithinCluster=TargetSubClusterPageNum*SuperBlock->page_len;

				/* Create the new directory record within the page in the cluster before writing the cluster back */
				result=UpdateDirEntInformation(port, slot, SuperBlock, TargetCluster, TargetOffsetWithinCluster, NewDirEntData);
			}
		}

		if((result>=0) && (result!=2)){
			DEBUG_PRINTF("Updating parent directory information...\n");

			/* Update the parent directory's information. */
			ParentDirEnt.length++;
			result=GetSetFileData(port, slot, 0, path, NULL, &ParentDirEnt, SuperBlock);
		}
	}

	return result;
}

static int DeleteVirtualDirent(unsigned char port, unsigned char slot, const char *path, const char *name, struct SCE_MC_Superblock *SuperBlock){
	char *PathToFile;
	struct SCE_MC_dirent DirEntData, ParentDirEnt;
	int result;//, ClustersFreed;

	DEBUG_PRINTF("DeleteVirtualDirent: %s/%s\n", path, name);

	if((PathToFile=malloc(strlen(path)+strlen(name)+2))!=NULL){
		sprintf(PathToFile, "%s/%s", path, name);

		/* Get the information on the file/folder. */
		if((result=GetFileData(port, slot, PathToFile, &DirEntData, SuperBlock))>=0){
			DirEntData.mode=0x7FFF;	/* Clear the "exists" flag to make the entry now non-existent (SONY MCMAN sets mode to 0x7FFF). */
			result=GetSetFileData(port, slot, 0, PathToFile, NULL, &DirEntData, SuperBlock);
		}

		free(PathToFile);
	}
	else result=-ENOMEM;

	if(result>=0){
		/* Get some information on the parent folder. */
		if((result=GetFileData(port, slot, path, &ParentDirEnt, SuperBlock))>=0){
			//The SONY MCMAN module doesn't seem to do this, so don't do this.
#if 0
			/* Now for some housekeeping: Free up unused clusters */
			ClustersFreed=FreeUnusedFATRecords(port, slot, ParentDirEnt.cluster, &ParentDirEnt.length, SuperBlock);
			DEBUG_PRINTF("%d cluster(s) freed. Records existing: %u\n", ClustersFreed, ParentDirEnt.length);

			/* Update the length of the parent directory. */
			result=GetSetFileData(port, slot, 0, path, NULL, &ParentDirEnt, SuperBlock);
#endif
		}
	}

	return result;
}

/*	Creates a cross-linked entry
 *	WARNING! CREATING AN ENTRY OF AN EXISTING ENTRY MIGHT CAUSE DATA LOSS WHEN ONE IS DELETED!
 */
int CreateCrossLinkedFiles(int port, int slot, const char *path, struct FileAlias *NewAliases){
	struct SCE_MC_Superblock SuperBlock;
	struct SCE_MC_dirent DirEnt, NewDirEnt;
	int result;
	unsigned int i, length;
	char *PathToTarget, *pFilename;

	DEBUG_PRINTF("CreateCrossLinkedFiles\n");

	FlushMCMANClusterCache(port, slot);

	if((result=GetCardFilesystemSuperblock(port, slot, &SuperBlock))<0){
		DEBUG_PRINTF("Error getting card specs for MC at port %d slot %d! Code %d\n", port, slot, result);
		return result;
	}

	DEBUG_PRINTF("MC Port: %d Slot: %d alloc_start: %u pagesize: %u blocksize: %u numpages %d flags: 0x%02x\n", port, slot, SuperBlock.alloc_start, SuperBlock.page_len, SuperBlock.pages_per_block, SuperBlock.clusters*SuperBlock.pages_per_cluster, SuperBlock.card_flags);

	/* Get the entry of the specified original file. */
	if((result=GetFileData(port, slot, path, &DirEnt, &SuperBlock))>=0){
		memcpy(&NewDirEnt, &DirEnt, sizeof(struct SCE_MC_dirent));
		result=0;

		/* Create the aliases (Cross-linked files). */
		for(i=0; (i<MAX_RPC_FILE_ENTRIES) && (NewAliases[i].name[0]!='\0'); i++){
			if((PathToTarget=strchr(NewAliases[i].name, '/'))!=NULL){
				length=(unsigned int)PathToTarget-(unsigned int)NewAliases[i].name;
				pFilename=&NewAliases[i].name[length+1];
			}
			else{
				length=strlen(NewAliases[i].name);
				pFilename=NewAliases[i].name;
			}

			PathToTarget=malloc(length+1);
			strncpy(PathToTarget, NewAliases[i].name, length);
			PathToTarget[length]='\0';
			strncpy(NewDirEnt.name, pFilename, sizeof(NewDirEnt.name));
			NewDirEnt.mode=NewAliases[i].mode;

			if(CreateNewVirtualDirent(port, slot, PathToTarget, &NewDirEnt, &SuperBlock)>=0){
				result++;
			}

			free(PathToTarget);
		}
	}

	return(result);
}

static int DeleteCrossLinkedFiles(int port, int slot, const char *path, struct FileAlias *Aliases){
	struct SCE_MC_Superblock SuperBlock;
	int result;
	unsigned int i, length, PathLength;
	char *PathToTarget, *pFilename;

	DEBUG_PRINTF("DeleteCrossLinkedFiles\n");

	FlushMCMANClusterCache(port, slot);

	if((result=GetCardFilesystemSuperblock(port, slot, &SuperBlock))<0){
		DEBUG_PRINTF("Error getting card specs for MC at port %d slot %d! Code %d\n", port, slot, result);
		return result;
	}

	DEBUG_PRINTF("MC Port: %d Slot: %d alloc_start: %u pagesize: %u blocksize: %u numpages %d flags: %u\n", port, slot, SuperBlock.alloc_start, SuperBlock.page_len, SuperBlock.pages_per_block, SuperBlock.clusters*SuperBlock.pages_per_cluster, SuperBlock.card_flags);

	/* Delete the aliases (Cross-linked files). */
	for(i=0; (i<MAX_RPC_FILE_ENTRIES) && (Aliases[i].name[0]!='\0'); i++){
		if((PathToTarget=strchr(Aliases[i].name, '/'))!=NULL){
			length=(unsigned int)PathToTarget-(unsigned int)Aliases[i].name;
			pFilename=&Aliases[i].name[length+1];
		}
		else{
			length=strlen(Aliases[i].name);
			pFilename=Aliases[i].name;
		}

		PathLength=strlen(path);
		PathToTarget=malloc(PathLength+strlen(Aliases[i].name)+length+2);
		strcpy(PathToTarget, path);
		PathToTarget[PathLength]='/';
		strncpy(&PathToTarget[PathLength]+1, Aliases[i].name, length);
		PathToTarget[PathLength+1+length]='\0';

		if(DeleteVirtualDirent(port, slot, PathToTarget, pFilename, &SuperBlock)>=0){
			result++;
		}

		free(PathToTarget);
	}

	return result;
}

void *mctools_rpc_handler(int fno, void *buf, int size){
	unsigned int ClusterSize;
	struct MCTools_GetMCInfoRPCResData *McSpecResData;

	DEBUG_PRINTF("mctools_rpc_handler func: 0x%02x\n", fno);

	switch(fno){
		case MCTOOLS_Init:
			EEAlignmentDataStructure=*(void **)buf;
			*(int*)rpc_TX_buffer=0;
			break;
		case MCTOOLS_CreateCrossLinkedEnts:
			*(int*)rpc_TX_buffer=CreateCrossLinkedFiles(((struct MCTools_CrossLinkedEntRPCData *)buf)->port, ((struct MCTools_CrossLinkedEntRPCData *)buf)->slot, ((struct MCTools_CrossLinkedEntRPCData *)buf)->path, ((struct MCTools_CrossLinkedEntRPCData *)buf)->Aliases);
			break;
		case MCTOOLS_DeleteCrossLinkedEnts:
			*(int*)rpc_TX_buffer=DeleteCrossLinkedFiles(((struct MCTools_CrossLinkedEntRPCData *)buf)->port, ((struct MCTools_CrossLinkedEntRPCData *)buf)->slot, ((struct MCTools_CrossLinkedEntRPCData *)buf)->path, ((struct MCTools_CrossLinkedEntRPCData *)buf)->Aliases);
			break;
		case MCTOOLS_GetMCInfo:
			McSpecResData=(struct MCTools_GetMCInfoRPCResData*)rpc_TX_buffer;
			McSpecResData->result=GetCardSpecs(((struct MCTools_GetMCInfoRPCReqData *)buf)->port, ((struct MCTools_GetMCInfoRPCReqData *)buf)->slot, &McSpecResData->SpecData.PageSize, &McSpecResData->SpecData.BlockSize, &McSpecResData->SpecData.CardSize);
			break;
		case MCTOOLS_ReadCluster:
			ClusterSize=((struct MCTools_ReadWriteRPCData *)buf)->PageSize*((struct MCTools_ReadWriteRPCData *)buf)->BlockSize;

			*(int*)rpc_TX_buffer=ReadCluster_raw(((struct MCTools_ReadWriteRPCData *)buf)->port, ((struct MCTools_ReadWriteRPCData *)buf)->slot, ((struct MCTools_ReadWriteRPCData *)buf)->PageSize, ((struct MCTools_ReadWriteRPCData *)buf)->BlockSize, ((struct MCTools_ReadWriteRPCData *)buf)->offset, ((struct MCTools_ReadWriteRPCData *)buf)->IOPbuffer);
			EE_memcpy(((struct MCTools_ReadWriteRPCData *)buf)->IOPbuffer, ((struct MCTools_ReadWriteRPCData *)buf)->EEbuffer, ClusterSize);
			break;
		case MCTOOLS_WriteBlock:
			*(int*)rpc_TX_buffer=WriteBlock(((struct MCTools_ReadWriteRPCData *)buf)->port, ((struct MCTools_ReadWriteRPCData *)buf)->slot, ((struct MCTools_ReadWriteRPCData *)buf)->PageSize, ((struct MCTools_ReadWriteRPCData *)buf)->BlockSize, ((struct MCTools_ReadWriteRPCData *)buf)->offset, ((struct MCTools_ReadWriteRPCData *)buf)->IOPbuffer);
			break;
		case MCTOOLS_InitPageCache:
			*(int*)rpc_TX_buffer=InitPageCache(((struct MCTools_CacheRPCData *)buf)->port, ((struct MCTools_CacheRPCData *)buf)->slot);
			break;
		case MCTOOLS_FlushPageCache:
			*(int*)rpc_TX_buffer=FlushPageCache();
			break;
		case MCTOOLS_FlushMCMANClusterCache:
			*(int*)rpc_TX_buffer=FlushMCMANClusterCache(((struct MCTools_MCMANFlushCommandData*)buf)->port, ((struct MCTools_MCMANFlushCommandData*)buf)->slot);
			break;
		default:
			*(int*)rpc_TX_buffer=-EINVAL;
	}

	return rpc_TX_buffer;
}

