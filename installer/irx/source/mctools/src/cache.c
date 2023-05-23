#include <errno.h>
#include <mcman.h>
#include <stdio.h>
#include <sysclib.h>

#include "mctools_internal.h"
#include "mctools.h"
#include "cache.h"
#include "MCIO.h"

static struct PageCache PageCache[PAGE_CACHE_SIZE];
static int CacheInitialized=0;
static unsigned short int CachePageLen=512, CacheBlockSize=2;
static unsigned int CacheCardSizeBlocks;
static int CacheMcPort, CacheMcSlot;

static void DeinitPageCache(void){
	unsigned int i;

	if(CacheInitialized){
		/* Free the memory allocated for the cache. */
		for(i=0; i<PAGE_CACHE_SIZE; i++){
			if(PageCache[i].buffer!=NULL) free(PageCache[i].buffer);
		}

		CacheInitialized=0;
	}

	memset(PageCache, 0, sizeof(PageCache));
}

/* A fact: Each page cannot occur twice in the cache. */

int InitPageCache(int port, int slot){
	int result;
	unsigned int i, CardSize;
	u8 flags;

	if((result=McGetCardSpec(port, slot, &CachePageLen, &CacheBlockSize, &CardSize, &flags))==sceMcResSucceed){
		result=0;
		DeinitPageCache();

		CacheMcPort=port;
		CacheMcSlot=slot;
		CacheCardSizeBlocks=CardSize/CacheBlockSize;
		for(i=0; i<PAGE_CACHE_SIZE; i++){
			if((PageCache[i].buffer=malloc(CachePageLen))==NULL){
				result=-ENOMEM;
				break;
			}
		}
	}

	if(result>=0) CacheInitialized=1;
	else DeinitPageCache();

	return result;
}

static void AddCacheBlockFlushList(struct CachePageFlushList *FlushList, unsigned int BlockPageOffset, void *buffer){
	unsigned int i;

	for(i=0; ; i++){
		if(FlushList[i].BlockPageOffset==(-1)){
			FlushList[i].BlockPageOffset=BlockPageOffset;
			FlushList[i].buffer=buffer;
			break;
		}
	}
}

static int FlushBlock(unsigned port, unsigned char slot, unsigned char *BlockBuffer, unsigned int BlockNum, struct CachePageFlushList *FlushList, unsigned int NumPagesToFlush){
	unsigned int i;
	int result;

	result=NumPagesToFlush;
	if(NumPagesToFlush>0){
		DEBUG_PRINTF("Flushing block 0x%08x\n", BlockNum);

		if((result=ReadBlock(port, slot, CachePageLen, CacheBlockSize, BlockNum, BlockBuffer))>=0){
			for(i=0; i<NumPagesToFlush; i++){
				memcpy(&BlockBuffer[CachePageLen*FlushList[i].BlockPageOffset], FlushList[i].buffer, CachePageLen);
			}

			if(result>=0) result=WriteBlock(port, slot, CachePageLen, CacheBlockSize, BlockNum, BlockBuffer);
		}
	}

	return result;
}

int FlushPageCache(void){
	unsigned char *BlockBuffer;
	int result;
	struct CachePageFlushList *FlushList;
	unsigned int i, BlockNum, NumPagesToFlush;

	DEBUG_PRINTF("Flushing cache...\n");

	result=0;
	if(CacheInitialized){
		if((FlushList=malloc(sizeof(struct CachePageFlushList)*CacheBlockSize))==NULL) return -ENOMEM;
		if((BlockBuffer=malloc(CachePageLen*CacheBlockSize))==NULL){
			free(FlushList);
			return -ENOMEM;
		}

		for(BlockNum=0; BlockNum<CacheCardSizeBlocks; BlockNum++){
			memset(FlushList, -1, sizeof(struct CachePageFlushList)*CacheBlockSize);
			NumPagesToFlush=0;

			/* Identify and group pages that belong to the same block together, and then write the block out. */
			for(i=0; i<PAGE_CACHE_SIZE; i++){
				if((!(PageCache[i].flags&PAGE_CACHE_PAGE_EXISTS)) || (!(PageCache[i].flags&PAGE_CACHE_PAGE_IS_DIRTY))) continue;

				if((PageCache[i].PageNum/CacheBlockSize)==BlockNum){
					DEBUG_PRINTF("Cache page flush 0x%08x BlockNum=0x%08x blocksz: %u\n", PageCache[i].PageNum, BlockNum, CacheBlockSize);
					AddCacheBlockFlushList(FlushList, PageCache[i].PageNum-(BlockNum*CacheBlockSize), PageCache[i].buffer);
					NumPagesToFlush++;
				}
			}

			result=FlushBlock(CacheMcPort, CacheMcSlot, BlockBuffer, BlockNum, FlushList, NumPagesToFlush);
		}

		free(FlushList);
		free(BlockBuffer);

		/* Mark all cache pages as available. */
		for(i=0; i<PAGE_CACHE_SIZE; i++){
			PageCache[i].flags=0;
			PageCache[i].PageNum=0;
		}
	}

	return result;
}

int ReadPageCached(unsigned int PageNum, void *buffer){
	int result;
	unsigned int i;

	result=0;
	if(CacheInitialized){
		for(i=0; i<PAGE_CACHE_SIZE; i++){
			if(PageCache[i].PageNum==PageNum){
				memcpy(buffer, PageCache[i].buffer, CachePageLen);
				break;
			}
		}

		/* If the cache was not hit */
		if(i==PAGE_CACHE_SIZE){
RetryPageRead:
			for(i=0; i<PAGE_CACHE_SIZE; i++){
				if(!(PageCache[i].flags&PAGE_CACHE_PAGE_EXISTS)){
					if((result=McReadPage(CacheMcPort, CacheMcSlot, PageNum, PageCache[i].buffer))==sceMcResSucceed){
						memcpy(buffer, PageCache[i].buffer, CachePageLen);
						PageCache[i].PageNum=PageNum;
						PageCache[i].flags=PAGE_CACHE_PAGE_EXISTS;
					}
					break;
				}
			}

			/* If an empty slot in the cache cannot be found, flush the cache first. */
			if(i==PAGE_CACHE_SIZE){
				if((result=FlushPageCache())>=0) goto RetryPageRead;
			}
		}
	}
	else{
		result=McReadPage(CacheMcPort, CacheMcSlot, PageNum, buffer);
	}

	return result;
}

int WritePageCached(unsigned int PageNum, void *buffer){
	int result;
	unsigned int i;

	DEBUG_PRINTF("Write page 0x%08x\n", PageNum);

	result=0;
	if(CacheInitialized){
		for(i=0; i<PAGE_CACHE_SIZE; i++){
			if(PageCache[i].PageNum==PageNum){
				memcpy(PageCache[i].buffer, buffer, CachePageLen);
				PageCache[i].flags|=PAGE_CACHE_PAGE_IS_DIRTY;
				break;
			}
		}

		/* If the cache was not hit */
		if(i==PAGE_CACHE_SIZE){
RetryPageWrite:
			for(i=0; i<PAGE_CACHE_SIZE; i++){
				if(!(PageCache[i].flags&PAGE_CACHE_PAGE_EXISTS)){
					memcpy(PageCache[i].buffer, buffer, CachePageLen);
					PageCache[i].PageNum=PageNum;
					PageCache[i].flags=PAGE_CACHE_PAGE_IS_DIRTY|PAGE_CACHE_PAGE_EXISTS;
					break;
				}
			}

			/* If an empty slot in the cache cannot be found, flush the cache first. */
			if(i==PAGE_CACHE_SIZE){
				if((result=FlushPageCache())>=0) goto RetryPageWrite;
			}
		}
	}
	else{
		result=-EINVAL;
	}

	return result;
}
