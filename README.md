# FreeMcBoot-Installer

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3a7e81446817406a94eeb77bcc3762dd)](https://app.codacy.com/gh/israpps/FreeMcBoot-Installer?utm_source=github.com&utm_medium=referral&utm_content=israpps/FreeMcBoot-Installer&utm_campaign=Badge_Grade_Settings)
[![Build [All]](https://github.com/israpps/FreeMcBoot-Installer/actions/workflows/compile-core.yml/badge.svg)](https://github.com/israpps/FreeMcBoot-Installer/actions/workflows/compile-core.yml)

[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1966.7z?color=black&label=1.966&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)
[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1953.7z?color=black&label=1.953&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)
[![GitHub release (latest by SemVer and asset including pre-releases)](https://img.shields.io/github/downloads-pre/israpps/FreeMcBoot-Installer/latest/FMCB-1965.7z?color=black&label=1.965&logo=GitHub)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/latest)

[![GitHub release (by tag)](https://img.shields.io/github/downloads/israpps/FreeMcBoot-Installer/APPS/total?color=000000&label=Apps%20Pack)](https://github.com/israpps/FreeMcBoot-Installer/releases/tag/APPS)

 Custom installers for FreeMcBoot 1966 & 1.965


[Original source code and binaries](https://sites.google.com/view/ysai187/home/projects/fmcbfhdb)

Special Thanks to SP193 for leaving the installer source code! it will help me out to add features to mi wLE mod ^^

-----

### PS2 system documentation

<details>
  <summary> <b> PS2 System Paths </b> </summary>

 __Region__   |__System update__ __*1__| __Data folder__ | __DVD Player Update__ |
------------- | --------------- | --------------- | --------------------- |
__Japanese__  | `BIEXEC-SYSTEM` | `BIDATA-SYSTEM` |  `BIEXEC-DVDPLAYER`   |
__American__  | `BAEXEC-SYSTEM` | `BADATA-SYSTEM` |  `BAEXEC-DVDPLAYER`   |
__Asian__     | `BAEXEC-SYSTEM` | `BADATA-SYSTEM` |  `BAEXEC-DVDPLAYER`   |
__European__  | `BEEXEC-SYSTEM` | `BEDATA-SYSTEM` |  `BEEXEC-DVDPLAYER`   |
__Chinese__   | `BCEXEC-SYSTEM` | `BCDATA-SYSTEM` |  `BCEXEC-DVDPLAYER`   |

##### notes:
__*1__: FreeMcBoot is installed on these folders
</details>

<details>
  <summary> <b> PS2 Update executables </b> </summary>
<p>

__Region__| __Model__  |__Chassis__| __ROM__|__ELF filename__|
--------- | ---------- | --------- | ------ | ------------- |
__Japan__      | `SCPH-10000` |    `A`    | `1.00 J` |   `osdsys.elf`  |
__Japan__      | `SCPH-10000` |    `A`    | `1.01 J` |	`osd110.elf`  |
__Japan__      | `SCPH-15000` |    `A`    | `1.01 J` |   `osd110.elf`  |
__Japan__      | `SCPH-18000` |  `A+/AB`  | `1.20 J` |	`osd130.elf`  |
__America__    | `SCPH-30001` |   `B/B'`  | `1.10 A` |   `osd120.elf`  |
__America__    | `SCPH-30001` |   `C/C'`  | `1.20 A` |	`osd130.elf`	|
__Europe__     | `SCPH-30002`/`3`/`4` | `C/C'` | `1.20 E` | `osd130.elf` |
__All__        | Most models  | `D` and newer | `1.50` and newer | `osdmain.elf` or `osdXXX.elf`
__Japan__      | PSX (`DESR`)| - |  `1.80` or `2.10` | `xosdmain.elf`

</p>
</details>

<details>
  <summary> <b> FreeMcBoot Install modes </b> </summary>
<p>

__Install Mode__ | __Description__  | __where will it work?__
---------------- | ---------------- | ---------
`Normal`         | installs FreeMcBoot on the system path used by your console (`osdmain.elf` has higher priority than `osdXXX.elf` if your console supports both files) |  the PS2 that you're using to install FreeMcBoot and any other consoles that supports the same path __*1__
`Cross-model`    | installs FreeMcBoot into every needed path for your `B?EXEC-SYSTEM` Folder |  every PS2 that uses the same region for the system update folder than the PS2 used for installation
`Cross-region`   | installs FreeMcBoot for every file needed for every system folder | every PS2 excluding `SCPH-9XXXX` and __PS2TV__
`Multi-Install` (the one on main menu)| same as `Cross-region`, but __CORRUPTS MEMORY CARD FILESYSTEM IN THE PROCESS__ | every PS2 excluding `SCPH-9XXXX` and __PS2TV__

##### notes:

__*1__: take a look at the previous table (_PS2 Update executables_)

</p>
</details>

<details>
  <summary> <b> installing on a <code>DTL-H</code> unit</b> </summary>
<p>

Installing on these units can be tricky, because the mechacon will use a different KELF binding than `SCPH` units

To circumvent this you could install [OpenTuna](https://github.com/ps2homebrew/opentuna-installer/releases), then use it to access uLaunchELF filebrowser and launch FreeMcBoot installer from a USB device

<details>
  <summary> <b> Make a Hybrid card </b> </summary>
<p>

Keep in mind that you can make a hybrid cards if you posses both `DTL-H` and `SCPH`

Make a normal install on the `DTL-H` and backup the ELF file from the `B?EXEC-SYSTEM` folder generated during install

Then install FreeMcBoot on the `SCPH` (preferably the cross model variant)

Then copy the ELF file from the `DTL-H` renamed as a specific system update, paste it on the folder where you got it

Now, how do you know the filename for the specific update?

Launch uLaunchELF on the `DTL-H` go fo filebrowser, MISC, debug info

Read the first 4 numbers of ROMVER

Lets imagine it says `0180` (if it says that number, then you're very lucky, that console is worth a lot of money, because `0180` was supposed to be exclusive of PSX-DESR)

The specific system update will be `osd180.elf`

You can use this freely unless both your `DTL-H` and the `SCPH` have same region and ROMVER lower than `0130` 

Why? Because those consoles won't use the `osdmain.elf`

Anyway, give it a try...

</p>
</details>

</p>
</details>

------

------




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
