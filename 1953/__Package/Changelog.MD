Package made by Matias Israelson (AKA: El_isra)
get the latest package here: https://github.com/israpps/FreeMcBoot-Installer/releases

This is a hybrid of FMCB 1.953 inside FMCBInstaller for FMCB 1.965.

Yielding the 1.953 modchip compatibility without the magicgate issues of the original installer bundled when it was released

changes:

- updated uLaunchELF (before 41e4ebe, now: 41e4ebe modded & compiled with PS2DEV1.0)

- added POWEROFF.XLF to package for:
	+ FreeMcBoot->`mc?:/SYS-CONF/POWEROFF/ELF`
	+ FreeHdBoot->`hdd0:/__system/osd/POWEROFF.ELF` 

- changed icon Flags:
	now:
	`B?EXEC-SYSTEM` will be shown as "ps2 software"
	`SYS-CONF` will be shown as "settings"


- enable APP execution for `PP.FHDB.APPS`(from HDD-OSD), to enable it place any PS2 app (in `.KELF` format, with the Name `EXEC.KELF`) at the `APPS-HDD` folder

replaced original FSCK package with 1.965 FSCK
