# FreeMcBoot-Installer

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3a7e81446817406a94eeb77bcc3762dd)](https://app.codacy.com/gh/israpps/FreeMcBoot-Installer?utm_source=github.com&utm_medium=referral&utm_content=israpps/FreeMcBoot-Installer&utm_campaign=Badge_Grade_Settings)
[![Build [All]](https://github.com/israpps/FreeMcBoot-Installer/actions/workflows/compile-core.yml/badge.svg)](https://github.com/israpps/FreeMcBoot-Installer/actions/workflows/compile-core.yml)

[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1966.7z?color=black&label=&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)
[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1965.7z?color=black&label=&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)
[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1964.7z?color=black&label=&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)
[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1963.7z?color=black&label=&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)
[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1953.7z?color=black&label=&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)

[![GitHub release (by tag)](https://img.shields.io/github/downloads/israpps/FreeMcBoot-Installer/APPS/total?color=000000&label=Apps%20Pack)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/APPS)

 Custom installers for FreeMcBoot 1.966, 1.965, 1.953, 1.964 and 1.963

They're packed with updated software.

In addition, several enhancements were made:
+ Installer:
  - Forbid multi install (corrupts memory card filesystem and doesn't achieve anything different than normal install)
  - Renamed normal install options to be user friendly
  - added manual HDD format option
  - added variant of installer that can be launched from exfat USB
+ Installation package:
  - updated Kernel patch updates for SCPH-10000 & SCPH-15000 to the one used on FreeMcBoot 1.966
  - Updated FreeHdBoot FSCK and MBR bootstraps to the one used on FreeHdBoot 1.966
  - added console shutdown ELF to all versions prior to 1.966
  - internal HDD APPS partition header data changed to allow KELF execution from HDD-OSD.

[Original source code and binaries](https://sites.google.com/view/ysai187/home/projects/fmcbfhdb)

Special Thanks to SP193 for leaving the installer source code! it will help me out to add features to mi wLE mod ^^

-----

<details>
  <summary> <b> APPS Package contents: </b> </summary>

```ini
ESR ESR r10f_direct
[Open PS2 Loader]
1.0.0
latest
0.9.3
0.9.2
0.9.1
0.9.0
0.8
0.7
0.6
0.5
[Cheats]
Cheat device (PAL)
Cheat device (NTSC)
[uLaunchELF]
4.43x_isr
4.43x_isr_hdd
4.43a 41e4ebe
4.43a_khn
4.43a latest
[MultiMedia]
SMS
Argon
[PS2ESDL]
v0.810 OB
v0.825 OB
[GSM]
v0.23x
v0.38
[Emulators]
FCEU
InfoNES
SNES Station (0.2.4S)
SNES Station (0.2.6C)
SNES9x
InfoGB
GPS2
GPSP-KAI
ReGBA
TempGBA
VBAM
PVCS
RetroArch (1.9.1)
[Utilities]
MechaPwn 2.0
LensChanger 1.2b
Padtest
RDRAM TEST
PS2 Ident
HDD Checker v0.964
Memory Card Anihilator 2.0
HWC Language Selector
Launch disc
Shutdown System app
```

</details>
