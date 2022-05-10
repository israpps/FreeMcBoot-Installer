Package made by Matias Israelson (AKA: El_isra)
get the latest package here: https://github.com/israpps/FreeMcBoot-Installer/releases
FreeMcBoot installer originally made by sp193

Changes:

- changed icon Flags:
	* `B?EXEC-SYSTEM` will be shown as "ps2 software"
	* `SYS-CONF` will be shown as "settings"
- Changed icons:
  * `B?EXEC-SYSTEM` By SpaceCoyote
  * `SYS-CONF` By SpaceCoyote
- added 1.965 version with:
  * Poweroff external utility bundled
- changed background, font, and font color for all installers
- Added 1.953 version fused inside 1.965 package to avoid old magicgate binding issues caused by the program
- 1.965 and 1.953: replace original FSCK with 1.966 FSCK (updated), and removed old font, wose size was 16mb (thats a lot because FSCK goes to `__system` partition, wich is 128mb sized, (so that font took 1/8 of total size)
- added manual HDD formatting feature
- updated lang
- normal install variants names made noob-friendly
- installers can now detect and inform rare consoles and tell the user (ie: normal PS2 with `1.80` ROM)
- blocked multi-install, no need for it
- System update folders `B?EXEC-SYSTEM` will have icon.sys variations for easy identification. ie: japanese system update folder (`BIEXEC-SYSTEM`) will be shown on OSD as "FreeMcBoot (japan)"
- OPL 1.0.0 bundled in package
- replaced uLaunchELF 4.43x `41e4ebe` with uLaunchELF 4.43x_isr
- add uLaunchELF 4.43x_isr_hdd on FreeHdBoot install pacakge
- HDD APPS partition will hold OPL 1.0.0 and uLaunchELF 4.43x_isr in KELF format, ready to be executed from HDD-OSD
- (related to previous entry) modified HDD APPS partition Header attributes to allow executiuon of uLaunchELF KELF
- all the installers are rebuilt with ps2dev:v1.0
