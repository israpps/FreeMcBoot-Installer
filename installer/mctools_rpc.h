#include "mctools.h"

/* Function protoypes */
void InitMCTOOLS(void);
void DeinitMCTOOLS(void);
int MCToolsCreateCrossLinkedFiles(int port, int slot, const char *path, struct FileAlias *NewAliases, int count);
int MCToolsDeleteCrossLinkedFiles(int port, int slot, const char *path, struct FileAlias *NewAliases, int count);
int MCToolsGetMCInfo(int port, int slot, struct MCTools_McSpecData *McSpecData);
int MCToolsInitPageCache(int port, int slot);
int MCToolsFlushPageCache(void);
int MCToolsAsyncGetLastError(void);
int MCToolsReadCluster(int port, int slot, unsigned int cluster, unsigned short int ClusterSize, const struct MCTools_McSpecData *McSpecData, void *buffer);
int MCToolsWriteBlock(int port, int slot, unsigned int block, const struct MCTools_McSpecData *McSpecData, void *buffer);
int MCToolsSync(int mode);
int MCToolsFlushMCMANClusterCache(int port, int slot);
