#include <errno.h>
#include <iopheap.h>
#include <kernel.h>
#include <sifrpc.h>
#include <string.h>

#include "mctools_rpc.h"

static SifRpcClientData_t MCTOOLS_rpc_cd;
static struct MCTools_AlignmentData MCTools_BufferAlignmentData ALIGNED(16);
static void *IopBuffer;
static int AsyncRpcEndSema = -1;

static unsigned int IopBufferSize;
static unsigned char TransmitBuffer[1024] ALIGNED(64);
static unsigned char ReceiveBuffer[64] ALIGNED(64);

static void MCTOOLS_RpcEndFunc(void *AlignmentData)
{
    memcpy(((struct MCTools_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer1Address, ((struct MCTools_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer1, ((struct MCTools_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer1_len);
    memcpy(((struct MCTools_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer2Address, ((struct MCTools_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer2, ((struct MCTools_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer2_len);
}

static void MCTOOLS_ReadRpcEndFunc(void *end_param)
{
    MCTOOLS_RpcEndFunc(end_param);
    iSignalSema(AsyncRpcEndSema);
}

static void MCTOOLS_WriteRpcEndFunc(void *end_param)
{
    iSignalSema(AsyncRpcEndSema);
}

void InitMCTOOLS(void)
{
    void *data;
    ee_sema_t sema;

    while ((SifBindRpc(&MCTOOLS_rpc_cd, MCTOOLS_RPC_NUM, 0) < 0) || (MCTOOLS_rpc_cd.server == NULL))
        nopdelay();
    IopBufferSize = 8192;
    IopBuffer = SifAllocIopHeap(IopBufferSize);

    data = &MCTools_BufferAlignmentData;
    SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_Init, 0, &data, sizeof(void *), NULL, 0, NULL, NULL);

    sema.init_count = 1;
    sema.max_count = 1;
    sema.attr = 0;
    sema.option = 0;
    AsyncRpcEndSema = CreateSema(&sema);
}

void DeinitMCTOOLS(void)
{
    memset(&MCTOOLS_rpc_cd, 0, sizeof(SifRpcClientData_t));
    if (IopBuffer != NULL)
        SifFreeIopHeap(IopBuffer);
    if (AsyncRpcEndSema >= 0) {
        DeleteSema(AsyncRpcEndSema);
        AsyncRpcEndSema = -1;
    }
}

static int CheckIOPBufferSize(int size)
{
    int result;

    if (size > IopBufferSize) {
        if (IopBuffer != NULL)
            SifFreeIopHeap(IopBuffer);
        if ((IopBuffer = SifAllocIopHeap(size)) != NULL) {
            result = size;
            IopBufferSize = size;
        } else {
            result = -ENOMEM;
            IopBufferSize = 0;
        }
    } else
        result = 0;

    return result;
}

int MCToolsCreateCrossLinkedFiles(int port, int slot, const char *path, struct FileAlias *NewAliases, int count)
{
    struct MCTools_CrossLinkedEntRPCData *TargetData;
    int RPC_res;
    unsigned int i;

    TargetData = (struct MCTools_CrossLinkedEntRPCData *)TransmitBuffer;

    TargetData->port = port;
    TargetData->slot = slot;
    strncpy(TargetData->path, path, sizeof(TargetData->path));

    for (i = 0; (i < MAX_RPC_FILE_ENTRIES) && (i < count); i++)
        memcpy(&TargetData->Aliases[i], &NewAliases[i], sizeof(TargetData->Aliases[i]));
    if (i < MAX_RPC_FILE_ENTRIES)
        TargetData->Aliases[i].name[0] = '\0'; /* NULL terminate the last entry. */

    if ((RPC_res = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_CreateCrossLinkedEnts, 0, TargetData, sizeof(struct MCTools_CrossLinkedEntRPCData), ReceiveBuffer, sizeof(int), NULL, NULL)) >= 0)
        RPC_res = *(int *)ReceiveBuffer;

    return RPC_res;
}

int MCToolsDeleteCrossLinkedFiles(int port, int slot, const char *path, struct FileAlias *Aliases, int count)
{
    struct MCTools_CrossLinkedEntRPCData *TargetData;
    int RPC_res;
    unsigned int i;

    TargetData = (struct MCTools_CrossLinkedEntRPCData *)TransmitBuffer;

    TargetData->port = port;
    TargetData->slot = slot;
    strncpy(TargetData->path, path, sizeof(TargetData->path));

    for (i = 0; (i < MAX_RPC_FILE_ENTRIES) && (i < count); i++)
        memcpy(&TargetData->Aliases[i], &Aliases[i], sizeof(TargetData->Aliases[i]));
    if (i < MAX_RPC_FILE_ENTRIES)
        TargetData->Aliases[i].name[0] = '\0'; /* NULL terminate the last entry. */

    if ((RPC_res = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_DeleteCrossLinkedEnts, 0, TargetData, sizeof(struct MCTools_CrossLinkedEntRPCData), ReceiveBuffer, sizeof(int), NULL, NULL)) >= 0)
        RPC_res = *(int *)ReceiveBuffer;

    return RPC_res;
}

int MCToolsGetMCInfo(int port, int slot, struct MCTools_McSpecData *McSpecData)
{
    struct MCTools_GetMCInfoRPCReqData *MCInfoRPCData;
    int RPC_res;

    MCInfoRPCData = (struct MCTools_GetMCInfoRPCReqData *)TransmitBuffer;

    MCInfoRPCData->SpecData = McSpecData;
    MCInfoRPCData->port = port;
    MCInfoRPCData->slot = slot;

    if ((RPC_res = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_GetMCInfo, 0, MCInfoRPCData, sizeof(struct MCTools_GetMCInfoRPCReqData), ReceiveBuffer, sizeof(struct MCTools_GetMCInfoRPCResData), &MCTOOLS_RpcEndFunc, &MCTools_BufferAlignmentData)) >= 0) {
        RPC_res = ((struct MCTools_GetMCInfoRPCResData *)ReceiveBuffer)->result;
        memcpy(McSpecData, &((struct MCTools_GetMCInfoRPCResData *)ReceiveBuffer)->SpecData, sizeof(struct MCTools_McSpecData));
    }

    return RPC_res;
}

int MCToolsInitPageCache(int port, int slot)
{
    struct MCTools_CacheRPCData *CacheRPCData;
    int RPC_res;

    CacheRPCData = (struct MCTools_CacheRPCData *)TransmitBuffer;
    CacheRPCData->port = port;
    CacheRPCData->slot = slot;

    if ((RPC_res = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_InitPageCache, 0, CacheRPCData, sizeof(struct MCTools_CacheRPCData), ReceiveBuffer, sizeof(int), NULL, NULL)) >= 0)
        RPC_res = *(int *)ReceiveBuffer;

    return RPC_res;
}

int MCToolsFlushPageCache(void)
{
    int RPC_res;
    if ((RPC_res = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_FlushPageCache, 0, NULL, 0, ReceiveBuffer, sizeof(int), NULL, NULL)) >= 0)
        RPC_res = *(int *)ReceiveBuffer;
    return RPC_res;
}

/*
 *	Functions starting from here onwards are asynchronous or are helper functions for asynchronous functions.
 */
int MCToolsAsyncGetLastError(void)
{
    return *(int *)ReceiveBuffer;
}

int MCToolsReadCluster(int port, int slot, unsigned int cluster, unsigned short int ClusterSize, const struct MCTools_McSpecData *McSpecData, void *buffer)
{
    int result;
    struct MCTools_ReadWriteRPCData *ReadClusterRPCData;

    if (CheckIOPBufferSize(McSpecData->PageSize * ClusterSize) >= 0) {
        WaitSema(AsyncRpcEndSema);

        ReadClusterRPCData = (struct MCTools_ReadWriteRPCData *)TransmitBuffer;

        ReadClusterRPCData->port = port;
        ReadClusterRPCData->slot = slot;
        ReadClusterRPCData->offset = cluster;
        ReadClusterRPCData->PageSize = McSpecData->PageSize;
        ReadClusterRPCData->BlockSize = ClusterSize;
        ReadClusterRPCData->EEbuffer = buffer;
        ReadClusterRPCData->IOPbuffer = IopBuffer;

        SifWriteBackDCache(buffer, McSpecData->PageSize * ClusterSize);

        if ((result = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_ReadCluster, SIF_RPC_M_NOWAIT, ReadClusterRPCData, sizeof(struct MCTools_ReadWriteRPCData), ReceiveBuffer, sizeof(int), &MCTOOLS_ReadRpcEndFunc, &MCTools_BufferAlignmentData)) < 0) {
            SignalSema(AsyncRpcEndSema);
        }
    } else
        result = -ENOMEM;

    return result;
}

int MCToolsWriteBlock(int port, int slot, unsigned int block, const struct MCTools_McSpecData *McSpecData, void *buffer)
{
    struct MCTools_ReadWriteRPCData *WriteBlockRPCData;
    int DMATransferID, BlockSize, result;
    SifDmaTransfer_t dmat;

    BlockSize = McSpecData->PageSize * McSpecData->BlockSize;

    if (CheckIOPBufferSize(BlockSize) >= 0) {
        WaitSema(AsyncRpcEndSema);

        WriteBlockRPCData = (struct MCTools_ReadWriteRPCData *)TransmitBuffer;

        dmat.src = buffer;
        dmat.dest = IopBuffer;
        dmat.size = BlockSize;
        dmat.attr = 0;
        SifWriteBackDCache(buffer, BlockSize);
        while ((DMATransferID = SifSetDma(&dmat, 1)) == 0) {};

        WriteBlockRPCData->port = port;
        WriteBlockRPCData->slot = slot;
        WriteBlockRPCData->offset = block;
        WriteBlockRPCData->PageSize = McSpecData->PageSize;
        WriteBlockRPCData->BlockSize = McSpecData->BlockSize;
        WriteBlockRPCData->IOPbuffer = dmat.dest;

        while (SifDmaStat(DMATransferID) >= 0) {};

        if ((result = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_WriteBlock, SIF_RPC_M_NOWAIT, WriteBlockRPCData, sizeof(struct MCTools_ReadWriteRPCData), ReceiveBuffer, sizeof(int), &MCTOOLS_WriteRpcEndFunc, NULL)) < 0) {
            SignalSema(AsyncRpcEndSema);
        }
    } else
        result = -ENOMEM;

    return result;
}

int MCToolsSync(int mode)
{
    int result;

    switch (mode) {
        case 1:
            if (PollSema(AsyncRpcEndSema) == AsyncRpcEndSema) {
                SignalSema(AsyncRpcEndSema);
                return 0;
            } else {
                return 1;
            }
            break;
        default:
            WaitSema(AsyncRpcEndSema);
            SignalSema(AsyncRpcEndSema);
            result = 0;
    }

    return result;
}

int MCToolsFlushMCMANClusterCache(int port, int slot)
{
    int result;

    ((struct MCTools_MCMANFlushCommandData *)TransmitBuffer)->port = port;
    ((struct MCTools_MCMANFlushCommandData *)TransmitBuffer)->slot = slot;

    return (((result = SifCallRpc(&MCTOOLS_rpc_cd, MCTOOLS_FlushMCMANClusterCache, 0, TransmitBuffer, sizeof(struct MCTools_MCMANFlushCommandData), ReceiveBuffer, sizeof(int), NULL, NULL)) >= 0) ? *(int *)ReceiveBuffer : result);
}
