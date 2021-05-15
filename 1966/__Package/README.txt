FMCB v1.966	-	release 2019/04/13
-------------------------------------------

What is FMCB?
-------------

Free Memory Card Boot (FMCB) is a homebrew software which is designed to setup your PlayStation 2 console
and provides you with a means of launching homebrew software, without the need for any extra hardware,
modifications to your console or dangerous tricks like the legendary swap trick.

FMCB was originally developed by Neme and Jimmikaelkael,
but they have been kind enough to provide me with access to its source code, for me to make some improvements to it.

What is FHDB?
-------------

Free Harddisk Drive Boot (FHDB) is basically FMCB that gets installed onto the PlayStation 2 console's HDD unit.
It does everything like FMCB does, but is meant to be used with the SONY HDDOSD (Browser update v2.00).

Patched/modified copies of the HDDOSD may not work, although non-Sony disks are supported by FHDB itself.
***Whether the hacked HDD OSD supports non-Sony disks or not depends on whether it was modified to support non-Sony disks.***

Modifying or getting a pre-modified copy of the HDDOSD is not under this project,
which is why that topic will not be discussed here.

What does FMCB v1.9 offer?
--------------------------

For humans:
	*Smaller, faster and more stable.
	*The HDD unit is now supported (FHDB).
	*The "Early Japanese"/PCMCIA units (SCPH-10000, SCPH-15000 and SCPH-18000) are fully supported.
	*The PSX (DVR unit) and compatible Debugstation (e.g. "TEST" consoles) consoles are supported.
	*Chinese consoles are now supported (e.g. SCPH-50009).
	*When you leave the CD/DVD tray ejected, FMCB/FHDB will not stall.
	*Your play history will now be updated (The towers in the background of the "Sony computer entertainment" screen should continue to grow taller and greater in numbers)
	*A new shutdown function, for shutting down the PS2 with the appropriate process for some (2.5") HDDs.

Technical changes:
	*Initialization code has been cut down.
	*Some IRX modules (init.irx and chkesr.irx) have been removed
		(superceded by code that runs from the EE, like their Sony originals).
	*DVD player selection code has been replaced with one similar to the one used by Sony.
	*The console initialization code has been replaced with a Sony-like version.
	*CNF parsing code has been optimized.
	*The memory map has been adjusted slightly, so everything now fits below Sony's idea of "user memory" (below 0x00100000).
	*Support for the Protokernel and all HDD OSDs have been added.
	*(For the HDDOSD version of FMCB only) ELFs can be booted from any partition on the HDD.
		The format of the path is <partition>:<full path to file on pfs:/>.
		(e.g. hdd0:__sysconf:pfs:/FMCB/FMCB_configurator.elf).
		This can be easily set using the included FMCB/FHDB configurator.
	*The CD/DVD drive will now be stopped if a normal ELF is started
		(Under the assumption that most homebrew ELFs probably won't ever use the drive).
	*The user's history file (the file which affects the towers within the "Sony Computer Entertainment" startup screen)
		will now be updated whenever Playstation/Playstation 2 game/software discs are launched,
		as well as DVD video discs.
	*Reloading the OSD from within the OSD triggers a full reload of FMCB, because I felt that it'll be cleaner.
	*The location of ESR will now be checked only when ESR is requested to be loaded.
	*The embedded EELOAD module has been cleaned up, and loads at 0x00082000 instead (Like rom0:EELOAD does).
	*FMCB has been split into two. The part of FMCB which remains resident will be copied into its place during runtime,
		allowing the initialization part of it to have the entire PlayStation 2 for initialization
		and drawing something nice on the screen (Not done at the moment as it makes FMCB quite a fair bit larger).
	*Full support for the SCPH-10000 and SCPH-15000.
		Not only will it patch up the SCPH-10000 and SCPH-15000 kernels fully (Applies OSD argument-passing patch),
		it also includes its own HDD support modules and can act as a replacement for the Sony system driver update.
	*Support for the PSX. Its XMB is not supported, meaning that there will be no hacked XMB for the PSX.
	*Supports non-Sony HDD units
		(Support for non-Sony HDD units within the HDD OSD/browser v2.00 update depends on the HDD OSD itself).
	*New FMCB configurator program which supports the HDD unit.
	*Added support for the Chinese consoles (Magicgate region 09, folder letter C. e.g. SCPH-50009).
	*Added support for DebugStation consoles that can boot FMCB (DTL-H101xx, DTL-H301xx and later).

Other notes/known bugs/known issues:
------------------------------------
*The PSBBN is not supported by FHDB. Please do not install FHDB if you use the PSBBN because that will ruin the PSBBN (its MBR program is incompatible).

*Although FHDB was meant to be used with the HDD browser, it is still possible to use FHDB without the HDD browser.
 However, if no valid HDD browser installation is detected, FHDB will power-off the HDD Unit and
 attempt to use the default browser from ROM instead.

*Although the new FMCB configurator supports saving of the FMCB configuration file to the HDD,
 only FHDB will be able to access it. Do not save the FMCB configuration file to the HDD if it is to be used with FMCB.

*When ATA support is installed (installed whenever compatibility with the SCPH-10000, SCPH-15000 and SCPH-18000 is required),
 FMCB will always attempt to load the HDD update from the HDD unit.
 There is currently no way to disable this.
 This will also happen on Japanese expansion-bay consoles,
 if an installation of FMCB which was made with compatibility for the SCPH-10000, SCPH-15000 and SCPH-18000 is run on it.

*If you wish to go back to entirely using the Sony stuff, use the Sony Utility Disc to re-install the HDDOSD.
 The full FMCB installer installs a homebrew MBR program, which cannot be uninstalled because that would leave
 your HDD will be unbootable (even if it has the HDD Browser installed there).

*The FMCB configurator's code is messy and I don't feel good about the way it currently works.
 I heard (and feel) that it's occasionally unstable, so please don't do weird things with it like spamming on
 the load/save configuration buttons.

*The icon mapping is different for the HDD OSD. The early OSDs of the SCPH-10000 and SCPH-15000 on the other hand,
 totally do not support the text formatting syntax.
 This means that the OSD cannot be nicely decorated and only normal ASCII can be used.
 This also means that exchanging the configuration files for FHDB and FMCB will result in cosmetic issues.

*When a program/game quits, the FMCB logo will not be shown before the OSD is loaded.
 This is so that it'll fit in with the design of the Sony OSD, but the trigger keys will still work like normal.

*When the "inner browser" setting is set to anything other than "AUTO",
 settings like "Skip logo" will have no effect (The Sony logo will always get skipped).
 This is due to how the browser behaves upon receiving commands like "BootBrowser" and "BootClock".

*Some 2.5" HDDs (used within USB enclosures of as the PS2 HDD) require their heads to be parked in software. Otherwise, their lifespans could be reduced significantly when power is removed and they attempt an emergency park.
 The User should use the shutdown function to shut down the PS2 and these devices properly.
 Please note that despite pressing the power button of an expansion bay PS2 will cause new (built in 2019) software to park the internal HDD's heads, this is presently not supported by FHDB because of the Browser.
 The exact capabilities of the software, depend on the software itself.

*FMCB/FHDB will issue the SCSI UNIT STOP command to USB devices connected when a disc is booted via fastboot, to prolong the lifespan of those (2.5") HDDs that require proper shutdown.
 However, this will not happen if the disc is booted from the browser because the disc boot process is not conducted by FMCB/FHDB.

Recovery Mode
-------------
Recovery mode allows you to boot a single ELF, in case your FMCB/FHDB installation was rendered unusable.
Place the ELF as mass:/RESCUE.ELF and FMCB/FHDB will boot it when you switch on the PlayStation 2.

If you are having difficulties with getting RESCUE.ELF to be booted, press and hold START+R1. FMCB/FHDB will make an infinite number of attempts to boot the file.

Credits
-------
Jimmikaelkael and Neme, the original developers and maintainers of FMCB.
l_Oliveria, for providing comments, being the solo tester for the early builds of FMCB,
	as well as providing information regarding compatibility for the SCPH-10000, SCPH-15000 and SCPH-18000.
krHACKen, for providing the resources I used for getting HDD OSD support working (sample files and test results).
Someone who wants to remain anonymous.
Translators.
...And all users!


Custom package made by Matías israelson (AKA: El_isra)

Icons made by SpaceCoyote#6585
