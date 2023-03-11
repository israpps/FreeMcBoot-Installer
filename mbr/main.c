#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>

#include <sys/fcntl.h>
#include <hdd-ioctl.h>

#include <fileXio_rpc.h>

extern unsigned char DEV9_irx[];
extern unsigned int size_DEV9_irx;

extern unsigned char ATAD_irx[];
extern unsigned int size_ATAD_irx;

extern unsigned char HDD_irx[];
extern unsigned int size_HDD_irx;

extern unsigned char PFS_irx[];
extern unsigned int size_PFS_irx;

extern unsigned char IOMANX_irx[];
extern unsigned int size_IOMANX_irx;

extern unsigned char FILEXIO_irx[];
extern unsigned int size_FILEXIO_irx;

#include "main.h"

static void BootError(int argc, char **argv){
	static char *argv_BootBrowser[2]={
		"BootBrowser",
		NULL
	};

	fileXioDevctl("hdd:", HDIOC_DEV9OFF, NULL, 0, NULL, 0);

	if(argc<2){
		ExecOSD(1, argv_BootBrowser);
	}
	else{
		ExecOSD(argc, argv);
	}
}

extern unsigned char EmbeddedEELOAD_bin[];
extern unsigned int size_EmbeddedEELOAD_bin;

static void LoadExecEELOAD(const char *path, const char *IOPRPImgPath, int mode, int argc, char *argv[]){
	int TotalArgc, index;
	const char *dev9_command;
	static const char *args_list[32];
	static const char BlankArg[]="";

	TotalArgc=(argc<0x11)?argc:0x10;
	if(TotalArgc>0){
		index=0;
		do{
			args_list[index]=argv[index];
			argc--;
			index++;
		}while(argc>0);

		argc=TotalArgc;
	}

	args_list[argc]=path;

	if(IOPRPImgPath==NULL){
		args_list[argc+1]=BlankArg;
	}
	else{
		args_list[argc+1]=IOPRPImgPath;
	}

	switch(mode){
		case 0:
			index=argc+2;
			dev9_command="HddNic";
			break;
		case 1:
			index=argc+2;
			dev9_command="Nic";
			break;
		default:
			index=argc+2;
			dev9_command=BlankArg;
	}

	args_list[index]=dev9_command;

	memcpy((void*)0x00084000, EmbeddedEELOAD_bin, size_EmbeddedEELOAD_bin);
	FlushCache(0);
	FlushCache(2);
	SifExitRpc();

	ExecPS2((void*)0x00084000, NULL, TotalArgc+3, (char**)args_list);
}

struct BootDestination{
	const char *StartupPath;	/* The path that will be passed to the program as argv[0] */
	const char *path;		/* The full path to the program, which is to be loaded. */
};

#define NUM_BOOT_TARGETS	3

static const struct BootDestination BootDestinations[2][NUM_BOOT_TARGETS]={
	{
		{	//Unofficial target, for FHDB
			"hdd0:__system:pfs:/osd/osdmain.elf",
			"pfs0:/osd/osdmain.elf"
		},
		{
			"hdd0:__system:pfs:/osd/hosdsys.elf",
			"pfs0:/osd/hosdsys.elf"
		},
		{
			"hdd0:__system:pfs:/osd100/hosdsys.elf",
			"pfs0:/osd100/hosdsys.elf"
		}
	},
	{
		{	//Unofficial target, for FHDB
			"hdd0:__system:pfs:/fsck/fsck.elf",
			"pfs0:/fsck/fsck.elf"
		},
		{
			"hdd0:__system:pfs:/fsck/fsck.elf",
			"pfs0:/fsck/fsck.elf"
		},
		{
			"hdd0:__system:pfs:/fsck100/fsck.elf",
			"pfs0:/fsck100/fsck.elf"
		}
	}
};

int main(int argc, char *argv[]){
	char ErrorPartName[64], **arg;
	const char *BootPath, *TargetStartupPath;
	int ExecuteFsck, i, fd;

/*	No need to reboot the IOP, since we use the default modules.
	SifInitRpc(0);
	SifIopRebootBuffer(IOPRP, size_IOPRP_img);
	while(!SifIopSync()){}; */

	SifInitRpc(0);
	SifInitIopHeap();
	SifLoadFileInit();

	sbv_patch_enable_lmb();

	SifExecModuleBuffer(IOMANX_irx, size_IOMANX_irx, 0, NULL, NULL);
	SifExecModuleBuffer(FILEXIO_irx, size_FILEXIO_irx, 0, NULL, NULL);

	SifExecModuleBuffer(DEV9_irx, size_DEV9_irx, 0, NULL, NULL);
	SifExecModuleBuffer(ATAD_irx, size_ATAD_irx, 0, NULL, NULL);
	SifExecModuleBuffer(HDD_irx, size_HDD_irx, 0, NULL, NULL);
	SifExecModuleBuffer(PFS_irx, size_PFS_irx, 0, NULL, NULL);

	fileXioInit();

	/* Check the HDD here. */
	/* Determine whether the device is connected. */
	if(fileXioDevctl("hdd0:", HDIOC_STATUS, NULL, 0, NULL, 0)!=0){
		BootError(argc, argv);
	}
	/* Check ATA device S.M.A.R.T. status. */
	if(fileXioDevctl("hdd0:", HDIOC_SMARTSTAT, NULL, 0, NULL, 0)!=0){
		BootError(argc, argv);
	}
	/* Check for unrecoverable I/O errors on sectors. */
	if(fileXioDevctl("hdd0:", HDIOC_GETSECTORERROR, NULL, 0, NULL, 0)!=0){
		BootError(argc, argv);
	}
	/* Check for partitions that have errors. */
	if(fileXioDevctl("hdd0:", HDIOC_GETERRORPARTNAME, NULL, 0, ErrorPartName, sizeof(ErrorPartName))!=0){
		ExecuteFsck=1;
		/* Do not continue if it is __system that has the error, since fsck is stored there. */
		if(strcmp(ErrorPartName, "__system")==0) BootError(argc, argv);
	}
	else ExecuteFsck=0;

	if(argc>1){
		for(i=1; i<argc; i++){
			if(strcmp(argv[i], "SkipFsck")==0){
				ExecuteFsck=0;
			}
		}
	}

	if(fileXioMount("pfs0:", "hdd0:__system", FIO_MT_RDONLY)>=0){
		//The original did not use a loop.
		for(i=0; i<NUM_BOOT_TARGETS; i++){
			if((fd=fileXioOpen((BootPath=BootDestinations[ExecuteFsck][i].path), O_RDONLY))>=0){
				TargetStartupPath=BootDestinations[ExecuteFsck][i].StartupPath;

				fileXioClose(fd);
				SifExitRpc();
				argv[0]=(char*)TargetStartupPath;
				LoadExecEELOAD(BootPath, ":", 0, argc, argv);
				break;
			}
		}

		fileXioUmount("pfs0:");
	}

	BootError(argc, argv);

	return 0;
}
