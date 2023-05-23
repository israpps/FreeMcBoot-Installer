Free Memory Card Boot (FMCB) installer v0.987 - 2019/04/13
----------------------------------------------------------

As the title suggests, this program installs Free Memory Card Boot (FMCB) and Free HDD Boot (FHDB).

All retail consoles should be supported, and no additional files should be required.

A memory card dumping and restoration facility within the installer is provided, for users to back-up the content of their cards.
A dump of a memory card in slot 1 will get saved as mc0.bin, and a dump of a card in slot 1 will get saved as mc1.bin.
The file will be saved to the directory where the main executable of this installer resides in.

Caution! You can only make one dump of a card from each port. Making additional dumps will overwrite the previous dump.

Lastly, please do give feedback.
Other than the bugs listed below, I do hope that this release has met the requirements of the public.

How to setup this software:
---------------------------
Extract the whole archive to any location supported device. Do not delete any files, including the stuff in the lang folder.
If you omit files, the installer may not work (it will return back to the browser).

The only supported device is the USB mass storage device.

What's the difference between the different installation modes?
---------------------------------------------------------------

Installation types:
	Normal			- For your PlayStation 2 and similar models.
	Normal, cross-model	- For your PlayStation 2 and all models within your region.
	Normal, cross-region	- For all PlayStation 2 consoles, from all regions.
	Multi-installation	- For all PlayStation 2 consoles, from all regions. Uses controlled filesystem corruption to save space.

A long time ago when FMCB v1.8b was just released, only the multi-installation mode existed (other than the normal installation).
It allowed users to make an installation of FMCB that worked on all consoles within the same region by introducing crosslinking
(controlled corruption) into the memory card's filesystem. The multi-installation of v1.8C and later allows cross-region installations to be done,
but still uses controlled filesystem corruption.

That was to save space, as older FMCB installers used to create about 20 crosslinked files, due to the developers not knowing which exact
boot ROMs will look for version-specific update files. However, with the discovery of exactly which boot ROMs will look for version-specific update files, it was found that only a handful of PS2 models actually have that behaviour. This practice was changed later on at boot ROM v1.50.

With the smaller binary size coupled with the fact that only a handful of duplicates are required,
the amount of space required for a cross-regional installation without crosslinking was greatly reduced.

A normal cross-regional installation does exactly the same thing as a multi-installation,
but duplicates the FMCB binary instead of crosslinking entries.
It's safer and does not go against the design of the memory card filesystem.

!!! WARNING !!! If you make a multi-install, DO NOT DELETE ANY OF THE B*EXEC-SYSTEM FOLDERS OR ANY OF THE OSD*.elf FILES INSIDE THEM!
Doing so might result in data loss.
Do not delete uninstall.dat from the SYS-CONF folder either, or you will probably be stuck with the multi-install files forever (Unless you use the cleanup utility, but it has it's own limitations).

Supported installation media:
-----------------------------
*USB Mass Storage Device

Other devices are not supported.

Files and folders:
------------------
The following folders exist within this package.
The content of folders is not user-customizable, unless otherwise specified.

/INSTALL		- Installation files
	/APPS		- (User-customizable) Contains files/folders that will be copied to the APPS folder on the memory card
	/APPS-HDD	- (User-customizable) Contains files/folders that will be copied to the PP.FHDB.APPS partition on the HDD unit.
	/BOOT		- (User-customizable) Contains files/folders that will be copied to the BOOT folder on the memory card
	/BOOT-HDD	- (User-customizable) Contains files/folders that will be copied to hdd0:__sysconf/FMCB on the HDD unit.
	/SYS-CONF	- System configuration files
	/SYSTEM		- System files
/lang			- Language files for the installer
/licenses		- Licenses, of software libraries used within the installer.

The APPS folder is for user programs (apps).
The BOOT folder is for storing programs related to the boot process, like uLaunchELF (BOOT.ELF) and ESR (ESR.ELF).

If programs are added, the FMCB/FHDB configuration file (FREEMCB.CNF/FREEHDB.CNF) must be updated.
Otherwise, they will not be used by FMCB.

Credits:
--------
Jimmikaelkael and Neme, as they were the original developers of FMCB.
Silverbull, for help with UDNL's construction (and with various issues related to the kernels).
Jimmikaelkael (Yes, again), as his MCMAN and MCSP modules, and the MCID file of FMCB v1.7 were used as referrences. FMCB v1.8C was also released by *him.
Ross Ridge, as his page on the PS2 Memory Card filesystem was invaluable: http://www.csclub.uwaterloo.ca:11068/mymc/ps2mcfs.html
"Someone who wishes to remain anonymous"
Berion, for the icons.
Translators.
l_Oliveria for contributing information related to the special needs (required kernel patches) of the SCPH-10000 and SCPH-15000, as well as information related to the boot ROM updates.
...And to all contributers and testers!

Anyone else missed here was not intentional.
