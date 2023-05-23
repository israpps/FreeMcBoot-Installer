#define PAGE_CACHE_SIZE			128	/* Cache 256 pages (256*512=128KB) */

#define PAGE_CACHE_PAGE_EXISTS		0x01
#define PAGE_CACHE_PAGE_IS_DIRTY	0x02

struct PageCache{
	unsigned int PageNum;
	void *buffer;
	unsigned int flags;
};

struct CachePageFlushList{
	int BlockPageOffset;
	void *buffer;
};

int InitPageCache(int port, int slot);
int FlushPageCache(void);
int ReadPageCached(unsigned int PageNum, void *buffer);
int WritePageCached(unsigned int PageNum, void *buffer);

