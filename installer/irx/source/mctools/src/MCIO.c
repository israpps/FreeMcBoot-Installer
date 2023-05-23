#include <errno.h>
#include <mcman.h>
#include <sysclib.h>

#include "mctools_internal.h"
#include "mctools.h"
#include "cache.h"
#include "MCIO.h"

int ReadBlock(int port, int slot, unsigned short int PageSize, unsigned short int BlockSize, unsigned int block, void *buffer){
	unsigned int i;
	int result;

	result=sceMcResSucceed;
	for(i=0; i<BlockSize; i++){
		if((result=McReadPage(port, slot, block*BlockSize+i, &((unsigned char *)buffer)[i*PageSize]))!=sceMcResSucceed){
			break;
		}
	}

	return result;
}

int WriteBlock(int port, int slot, unsigned short int PageSize, unsigned short int BlockSize, unsigned int block, void *buffer){
	unsigned char *ECC_data;
	unsigned int i, ECCBlockLen, ECCBlockSect;
	int result;

	ECCBlockLen=PageSize/32; /* Size calculated based on information extracted from Jimmikaelkael's MCMAN module */

	if((ECC_data=malloc(ECCBlockLen))!=NULL){
		memset(ECC_data, 0, ECCBlockLen);

		/* If someone can tell me how the EraseBlock function really works, that will be informative. The Sony MCSERV module calculates the checksum of the block to be written separately.
			So since EraseBlock probably doesn't calculate the checksums of the blocks... what does it really calculate?
		*/
		if((result=McEraseBlock2(port, 0, block, NULL, NULL))>=0){
			for(i=0; i<BlockSize; i++){
				/* Manually calculate the ECC for each 128-byte block for each page of the block to be written. See the comment above for more details. */
				for(ECCBlockSect=0; ECCBlockSect<(PageSize/128); ECCBlockSect++) McDataChecksum(&((unsigned char *)buffer)[i*PageSize+ECCBlockSect*128], &ECC_data[ECCBlockSect*3]);

				if((result=McWritePage(port, slot, block*BlockSize+i, &((unsigned char *)buffer)[i*PageSize], ECC_data))!=sceMcResSucceed){
					break;
				}
			}
		}

		free(ECC_data);
	}
	else result=-ENOMEM;

	return result;
}

int ReadCluster(unsigned short int PageSize, unsigned short int ClusterSize, unsigned int cluster, void *buffer){
	unsigned int i;
	int result;

	result=sceMcResSucceed;
	for(i=0; i<ClusterSize; i++){
		if((result=ReadPageCached(cluster*ClusterSize+i, &((unsigned char *)buffer)[i*PageSize]))<0){
			break;
		}
	}

	return result;
}

int ReadCluster_raw(int port, int slot, unsigned short int PageSize, unsigned short int ClusterSize, unsigned int cluster, void *buffer){
	unsigned int i;
	int result;

	result=sceMcResSucceed;
	for(i=0; i<ClusterSize; i++){
		if((result=McReadPage(port, slot, cluster*ClusterSize+i, &((unsigned char *)buffer)[i*PageSize]))!=sceMcResSucceed){
			break;
		}
	}

	return result;
}

int WriteCluster(unsigned short int PageSize, unsigned short int ClusterSize, unsigned int cluster, void *buffer){
	unsigned int i;
	int result;

	result=sceMcResSucceed;
	for(i=0; i<ClusterSize; i++){
		if((result=WritePageCached(cluster*ClusterSize+i, &((unsigned char *)buffer)[i*PageSize]))<0){
			break;
		}
	}

	return result;
}

