#include <iopcontrol.h>
#include <iopcontrol_special.h>
#include <iopheap.h>
#include <kernel.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>

#include <hdd-ioctl.h>
#include <fileXio_rpc.h>

#include "main.h"

static void BootError(void)
{
	static char *argv[]={
		"BootError",
		"EXEC_HDD_MODULE"
	};

	ExecOSD(2, argv);
}

static void InitializeUserMemory(void *start, void *end)
{
	u8 *ptr;

	//clear memory.
	for (ptr = (u8*)start; ptr < (u8*)end; ptr += 64) {
		asm (
			"\tsq $0, 0(%0) \n"
			"\tsq $0, 16(%0) \n"
			"\tsq $0, 32(%0) \n"
			"\tsq $0, 48(%0) \n"
			:: "r" (ptr)
		);
	}

	FlushCache(0);
	FlushCache(2);
}

/* Set the DEV9 expansion device into the appropriate power state.
   When HddNic is specified, the HDD is left active with the default standby timer set to 0xff (21 minutes & 15 seconds).
   When Nic is specified, the HDD is put into IDLE state.
   When nothing is specified, then DEV9 is shut down. */
static void Shutdown(const char *arg)
{
	if(arg[0] == '\0' || arg[0] == 'N')
	{	//Either a blank or "Nic" was specified.
		/* In the HDD Browser, Sony always used HDIOC_IDLE with a standby timer value of 0 (timer disabled).
		   This put the HDD into IDLE state, but also prevented it from ever entering STANDBY state.
		   In the PSBBN, the code was changed to use HDIOC_IDLEIMM instead, which only put the HDD in IDLE state without changing the standby timer value. */
		fileXioDevctl("hdd0:", HDIOC_IDLEIMM, NULL, 0, NULL, 0);
		fileXioDevctl("hdd1:", HDIOC_IDLEIMM, NULL, 0, NULL, 0);
	/*
		u8 standbytimer = 0;
		fileXioDevctl("hdd0:", HDIOC_IDLE, &standbytimer, sizeof(standbytimer), NULL, 0);
		fileXioDevctl("hdd1:", HDIOC_IDLE, &standbytimer, sizeof(standbytimer), NULL, 0);	*/
	}
	if(arg[0] == '\0')
	{	//If a blank was specified.
		fileXioDevctl("hdd:", HDIOC_DEV9OFF, NULL, 0, NULL, 0);
	}
}

#define IMAGE_SIZE	0x00100000

/*	After the arguments are three special arguments, which are for EELOAD:
		argv[argc-1] =	Power argument (a blank, "HddNic" or "Nic")
		argv[argc-2] =	IOP boot argument (':' to reboot with default
				modules, a path to the UDNL module to reboot
				with or a blank to do nothing)
		argv[argc-3] =	Boot filename	*/
int main(int argc, char *argv[])
{
	t_ExecData ElfData;
	int result, fd, UDNLSize;
	const char *filename, *iopboot, *power;
	void *UDNLBuffer;

	filename = argv[argc-3];
	iopboot = argv[argc-2];
	power = argv[argc-1];
	UDNLBuffer = (void*)(GetMemorySize() - IMAGE_SIZE);
	InitializeUserMemory((void*)0x00100000, UDNLBuffer);

	SifInitRpc(0);

	/* Attempt to load and execute specified program. */
	result = SifLoadElfEncrypted(filename, &ElfData);

	if(result < 0)
	{
		Shutdown(power);
		BootError();
	}

	//Check IOP boot argument
	if(iopboot[0] != '\0')
	{
		if(iopboot[0] == ':')
		{
			Shutdown(power);
			SifIopReset("", 0);
		}
		else
		{
			if((fd = fileXioOpen(iopboot, O_RDONLY)) < 0)
				BootError();
			if((UDNLSize = fileXioRead(fd, UDNLBuffer, IMAGE_SIZE)) < 0)
				BootError();
			fileXioClose(fd);

			Shutdown(power);

			SifIopRebootBufferEncrypted(UDNLBuffer, UDNLSize);
		}

		while(!SifIopSync()){};

		//InitializeUserMemory((void*)0x01e00000, (void*)0x01e80000);			//Wipe memory used by LMB patch. Our LMB patch does not use extra memory, hence no requirement to perform this wipe.
		InitializeUserMemory(UDNLBuffer, (void*)((u8*)UDNLBuffer + IMAGE_SIZE));	//Wipe memory used by the UDNL module.
	}
	else
	{
		//No IOP reboot
		Shutdown(power);
	}

	SifExitRpc();
	ExecPS2((void *)ElfData.epc, (void *)ElfData.gp, argc-3, argv);

	return 0;
}
