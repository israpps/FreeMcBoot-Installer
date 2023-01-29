#include <iopcontrol.h>
#include <iopcontrol_special.h>
#include <iopheap.h>
#include <kernel.h>
#include <libcdvd.h>
#include <libmc.h>
#include <libpwroff.h>
#include <loadfile.h>
#include <libpad.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>
#include <fileXio_rpc.h>

#include "main.h"
#include "iop.h"
#include "system.h"
#define IMPORT_IRX(_IRX) \
extern unsigned char _IRX[]; \
extern unsigned int size_##_IRX;

IMPORT_IRX(IOMANX_irx);
IMPORT_IRX(FILEXIO_irx);
IMPORT_IRX(SIO2MAN_irx);
IMPORT_IRX(PADMAN_irx);
IMPORT_IRX(MCMAN_irx);
IMPORT_IRX(MCSERV_irx);
IMPORT_IRX(SECRSIF_irx);
IMPORT_IRX(MCTOOLS_irx);
IMPORT_IRX(USBD_irx);
#ifdef EXFAT
IMPORT_IRX(usbmass_bd_irx);
IMPORT_IRX(bdm_irx);
IMPORT_IRX(bdmfs_fatfs_irx);
#else
IMPORT_IRX(USBHDFSD_irx);
#endif
IMPORT_IRX(POWEROFF_irx);
IMPORT_IRX(DEV9_irx);
IMPORT_IRX(ATAD_irx);
IMPORT_IRX(HDD_irx);
IMPORT_IRX(PFS_irx);
IMPORT_IRX(IOPRP_img);

u8 dev9Loaded;

#define SYSTEM_INIT_THREAD_STACK_SIZE 0x1000

struct SystemInitParams
{
    int InitCompleteSema;
    unsigned int flags;
};

static void SystemInitThread(struct SystemInitParams *SystemInitParams)
{
    static const char PFS_args[] = "-n\0"
                                   "24\0"
                                   "-o\0"
                                   "8";
    int i;

    if (SystemInitParams->flags & IOP_MOD_HDD) {
        if (SifExecModuleBuffer(ATAD_irx, size_ATAD_irx, 0, NULL, NULL) >= 0) {
            SifExecModuleBuffer(HDD_irx, size_HDD_irx, 0, NULL, NULL);
            SifExecModuleBuffer(PFS_irx, size_PFS_irx, sizeof(PFS_args), PFS_args, NULL);
        }
    }

    if (SystemInitParams->flags & IOP_MOD_SECRSIF) {
        SifExecModuleBuffer(SECRSIF_irx, size_SECRSIF_irx, 0, NULL, NULL);
        SecrInit();
    }

    if (SystemInitParams->flags & IOP_MOD_MCTOOLS) {
        SifExecModuleBuffer(MCTOOLS_irx, size_MCTOOLS_irx, 0, NULL, NULL);
        InitMCTOOLS();
    }

    SifExitIopHeap();
    SifLoadFileExit();

    SignalSema(SystemInitParams->InitCompleteSema);
    ExitDeleteThread();
}

int IopInitStart(unsigned int flags)
{
    ee_sema_t sema;
    static struct SystemInitParams InitThreadParams;
    static unsigned char SysInitThreadStack[SYSTEM_INIT_THREAD_STACK_SIZE] __attribute__((aligned(16)));
    int stat, ret;

    if (!(flags & IOP_REBOOT)) {
        SifInitRpc(0);
    } else {
        PadDeinitPads();
        sceCdInit(SCECdEXIT);
        DeinitMCTOOLS();
        SecrDeinit();
        fileXioExit();
    }

    if (!(flags & IOP_LIBSECR_IMG)) {
        SifIopReset("", 0);
    } else {
        SifIopRebootBuffer(IOPRP_img, size_IOPRP_img);
    }

    // Do something useful while the IOP resets.
    sema.init_count = 0;
    sema.max_count = 1;
    sema.attr = sema.option = 0;
    InitThreadParams.InitCompleteSema = CreateSema(&sema);
    InitThreadParams.flags = flags;

    while (!SifIopSync()) {};

    SifInitRpc(0);
    SifInitIopHeap();
    SifLoadFileInit();

    sbv_patch_enable_lmb();

    SifExecModuleBuffer(IOMANX_irx, size_IOMANX_irx, 0, NULL, NULL);
    SifExecModuleBuffer(FILEXIO_irx, size_FILEXIO_irx, 0, NULL, NULL);

    fileXioInit();
    sceCdInit(SCECdINoD);

    SifExecModuleBuffer(POWEROFF_irx, size_POWEROFF_irx, 0, NULL, NULL);
    ret = SifExecModuleBuffer(DEV9_irx, size_DEV9_irx, 0, NULL, &stat);
    dev9Loaded = (ret >= 0 && stat == 0); // dev9.irx must have loaded successfully and returned RESIDENT END.

    SifExecModuleBuffer(SIO2MAN_irx, size_SIO2MAN_irx, 0, NULL, NULL);
    SifExecModuleBuffer(PADMAN_irx, size_PADMAN_irx, 0, NULL, NULL);
    SifExecModuleBuffer(MCMAN_irx, size_MCMAN_irx, 0, NULL, NULL);
    SifExecModuleBuffer(MCSERV_irx, size_MCSERV_irx, 0, NULL, NULL);

#ifdef EXFAT
    SifExecModuleBuffer(bdm_irx, size_bdm_irx, 0, NULL, NULL);
    SifExecModuleBuffer(bdmfs_fatfs_irx, size_bdmfs_fatfs_irx, 0, NULL, NULL);
#endif

    SifExecModuleBuffer(USBD_irx, size_USBD_irx, 0, NULL, NULL);

#ifdef EXFAT
    SifExecModuleBuffer(usbmass_bd_irx, size_usbmass_bd_irx, 0, NULL, NULL);
    sleep(3);
#else
    SifExecModuleBuffer(USBHDFSD_irx, size_USBHDFSD_irx, 0, NULL, NULL);
    sleep(1);
#endif

    SysCreateThread(SystemInitThread, SysInitThreadStack, SYSTEM_INIT_THREAD_STACK_SIZE, &InitThreadParams, 0x2);

    poweroffInit();
    poweroffSetCallback(&poweroffCallback, NULL);
    mcInit(MC_TYPE_XMC);
    PadInitPads();

    return InitThreadParams.InitCompleteSema;
}

void IopDeinit(void)
{
    PadDeinitPads();
    sceCdInit(SCECdEXIT);
    DeinitMCTOOLS();
    SecrDeinit();

    fileXioExit();
    SifExitRpc();
}
