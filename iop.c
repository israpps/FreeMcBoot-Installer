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

extern unsigned char IOMANX_irx[];
extern unsigned int size_IOMANX_irx;

extern unsigned char FILEXIO_irx[];
extern unsigned int size_FILEXIO_irx;

extern unsigned char SIO2MAN_irx[];
extern unsigned int size_SIO2MAN_irx;

extern unsigned char PADMAN_irx[];
extern unsigned int size_PADMAN_irx;

extern unsigned char MCMAN_irx[];
extern unsigned int size_MCMAN_irx;

extern unsigned char MCSERV_irx[];
extern unsigned int size_MCSERV_irx;

extern unsigned char SECRSIF_irx[];
extern unsigned int size_SECRSIF_irx;

extern unsigned char MCTOOLS_irx[];
extern unsigned int size_MCTOOLS_irx;

extern unsigned char USBD_irx[];
extern unsigned int size_USBD_irx;

extern unsigned char USBHDFSD_irx[];
extern unsigned int size_USBHDFSD_irx;

extern unsigned char POWEROFF_irx[];
extern unsigned int size_POWEROFF_irx;

extern unsigned char DEV9_irx[];
extern unsigned int size_DEV9_irx;

extern unsigned char ATAD_irx[];
extern unsigned int size_ATAD_irx;

extern unsigned char HDD_irx[];
extern unsigned int size_HDD_irx;

extern unsigned char PFS_irx[];
extern unsigned int size_PFS_irx;

extern unsigned char IOPRP_img[];
extern unsigned int size_IOPRP_img;

u8 dev9Loaded;

#define SYSTEM_INIT_THREAD_STACK_SIZE	0x1000

struct SystemInitParams{
	int InitCompleteSema;
	unsigned int flags;
};

static void SystemInitThread(struct SystemInitParams *SystemInitParams)
{
	static const char PFS_args[]="-n\0""24\0""-o\0""8";
	int i;

	if(SystemInitParams->flags & IOP_MOD_HDD)
	{
		if(SifExecModuleBuffer(ATAD_irx, size_ATAD_irx, 0, NULL, NULL) >= 0)
		{
			SifExecModuleBuffer(HDD_irx, size_HDD_irx, 0, NULL, NULL);
			SifExecModuleBuffer(PFS_irx, size_PFS_irx, sizeof(PFS_args), PFS_args, NULL);
		}
	}

	if(SystemInitParams->flags & IOP_MOD_SECRSIF)
	{
		SifExecModuleBuffer(SECRSIF_irx, size_SECRSIF_irx, 0, NULL, NULL);
		SecrInit();
	}

	if(SystemInitParams->flags & IOP_MOD_MCTOOLS)
	{
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

	if(!(flags & IOP_REBOOT))
	{
		SifInitRpc(0);
	} else {
		PadDeinitPads();
		sceCdInit(SCECdEXIT);
		DeinitMCTOOLS();
		SecrDeinit();
		fileXioExit();
	}

	if(!(flags & IOP_LIBSECR_IMG))
	{
		SifIopReset("", 0);
	} else {
		SifIopRebootBuffer(IOPRP_img, size_IOPRP_img);
	}

	//Do something useful while the IOP resets.
	sema.init_count=0;
	sema.max_count=1;
	sema.attr=sema.option=0;
	InitThreadParams.InitCompleteSema=CreateSema(&sema);
	InitThreadParams.flags = flags;

	while(!SifIopSync()){};

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
	dev9Loaded = (ret >= 0 && stat == 0);	//dev9.irx must have loaded successfully and returned RESIDENT END.

	SifExecModuleBuffer(SIO2MAN_irx, size_SIO2MAN_irx, 0, NULL, NULL);
	SifExecModuleBuffer(PADMAN_irx, size_PADMAN_irx, 0, NULL, NULL);
	SifExecModuleBuffer(MCMAN_irx, size_MCMAN_irx, 0, NULL, NULL);
	SifExecModuleBuffer(MCSERV_irx, size_MCSERV_irx, 0, NULL, NULL);

	SifExecModuleBuffer(USBD_irx, size_USBD_irx, 0, NULL, NULL);
	SifExecModuleBuffer(USBHDFSD_irx, size_USBHDFSD_irx, 0, NULL, NULL);

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

