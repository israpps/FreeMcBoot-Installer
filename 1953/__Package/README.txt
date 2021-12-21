FMCB v1.965	-	release 2018/12/08
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
*The PSBBN is not supported.

*Although the HDDOSD is a requirement for using FHDB, it is still possible to use FHDB without the HDDOSD.
 However, if no valid HDDOSD installation is detected, FHDB will power-off the HDD Unit and
 attempt to use the ROM OSD instead.

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

Credits
-------
Jimmikaelkael and Neme, the original developers and maintainers of FMCB.
l_Oliveria, for providing comments, being the solo tester for the early builds of FMCB,
	as well as providing information regarding compatibility for the SCPH-10000, SCPH-15000 and SCPH-18000.
krHACKen, for providing the resources I used for getting HDD OSD support working (sample files and test results).
Someone who wants to remain anonymous.
...And all testers!

Custom packages made by Matías israelson (AKA: El_isra)
Icons made by SpaceCoyote#6585
