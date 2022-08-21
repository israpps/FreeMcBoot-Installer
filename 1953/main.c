#include <iopheap.h>
#include <kernel.h>
#include <libcdvd.h>
#include <libmc.h>
#include <fileXio_rpc.h>
#include <hdd-ioctl.h>
#include <loadfile.h>
#include <malloc.h>
#include <sbv_patches.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <libgs.h>

#include "main.h"
#include "iop.h"
#include "pad.h"
#include "graphics.h"
#include "font.h"

#include "libsecr.h"
#include "mctools_rpc.h"
#include "system.h"
#include "ReqSpaceCalc.h"
#include "UI.h"
#include "menu.h"

int IsHDDUnitConnected=0;

int VBlankStartSema;

static int VBlankStartHandler(int cause){
	ee_sema_t sema;
	iReferSemaStatus(VBlankStartSema, &sema);
	if(sema.count<sema.max_count) iSignalSema(VBlankStartSema);
	ExitHandler();
	return 0;
}

static void DeinitServices(void)
{
	DisableIntc(kINTC_VBLANK_START);
	RemoveIntcHandler(kINTC_VBLANK_START, 0);
	DeleteSema(VBlankStartSema);

	IopDeinit();
}

int main(int argc, char *argv[])
{
	int SystemType, InitSemaID, BootDevice, result;
	unsigned int FrameNum;
	ee_sema_t sema;

	//chdir("mass:/FMCBInstaller/");
	if((BootDevice = GetBootDeviceID()) == BOOT_DEVICE_UNKNOWN)
		Exit(-1);

	InitSemaID = IopInitStart(IOP_MOD_SET_MAIN);

	sema.init_count=0;
	sema.max_count=1;
	sema.attr=sema.option=0;
	VBlankStartSema=CreateSema(&sema);

	AddIntcHandler(kINTC_VBLANK_START, &VBlankStartHandler, 0);
	EnableIntc(kINTC_VBLANK_START);

	if(InitializeUI(0)!=0){
		SifExitRpc();
		Exit(-1);
	}

	FrameNum=0;
	/* Draw something nice here while waiting... */
	do{
		RedrawLoadingScreen(FrameNum);
		FrameNum++;
	}while(PollSema(InitSemaID)!=InitSemaID);
	DeleteSema(InitSemaID);

	DEBUG_PRINTF("Modules loaded.\n");

	StartWorkerThread();

	UpdateRegionalPaths();

	SystemType = GetPs2Type();
	if(SystemType == PS2_SYSTEM_TYPE_PS2 || SystemType == PS2_SYSTEM_TYPE_DEX)
	{
		result = HDDCheckStatus();
		if(result >= 0 && result <= 1)
		{	//Accept only if a usable disk is connected (formatted or not).
			IsHDDUnitConnected=1;
		}
	}

	MainMenu();

ShutdownRPCsAndExit:
	StopWorkerThread();

	if(!IsHDDBootingEnabled())
		fileXioDevctl("hdd0:", HDDCTL_DEV9_SHUTDOWN, NULL, 0, NULL, 0);

	DeinitializeUI();
	DeinitServices();

	return 0;
}
