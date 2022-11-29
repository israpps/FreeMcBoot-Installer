---
sort: 10
---

# System Update documentation


<div class="Subhead">
  <h2 class="Subhead-heading">System Folders</h2>
  <div class="Subhead-description">the directories where PS2 looks for updates and local settings</div>
</div>


__Region__   |__System update__ | __Data folder__ [^2] | __DVD Player Update__ [^1]|
------------- | --------------- | --------------- | --------------------- |
__Japanese__  | `BIEXEC-SYSTEM` | `BIDATA-SYSTEM` |  `BIEXEC-DVDPLAYER`   |
__American__  | `BAEXEC-SYSTEM` | `BADATA-SYSTEM` |  `BAEXEC-DVDPLAYER`   |
__Asian__     | `BAEXEC-SYSTEM` | `BADATA-SYSTEM` |  `BAEXEC-DVDPLAYER`   |
__European__  | `BEEXEC-SYSTEM` | `BEDATA-SYSTEM` |  `BEEXEC-DVDPLAYER`   |
__Chinese__   | `BCEXEC-SYSTEM` | `BCDATA-SYSTEM` |  `BCEXEC-DVDPLAYER`   |

[^1]: __DVD-player__ update executable name is: `dvdplayer.elf`
[^2]: __Data Folder:__ seen on the console browser as "Your System Configuration" this folder hold the play history file (a file that holds a record of played games, used to generate the towers on the console start animation), also, `TITLE.DB` is held on this folder, a file used by the PS1 retrocompatibility systems


<div class="Subhead">
  <h2 class="Subhead-heading">System executables</h2>
  <div class="Subhead-description">The filenames of the system updates depending on the console model</div>
</div>


__Region__| __Model__  |__Chassis__| __ROM__|__ELF filename__|
--------- | ---------- | --------- | ------ | ------------- |
__Japan__      | `SCPH-10000` |    `A`    | `1.00 J` |   `osdsys.elf` [^3] |
__Japan__      | `SCPH-10000` |    `A`    | `1.01 J` |	`osd110.elf` [^3] |
__Japan__      | `SCPH-15000` |    `A`    | `1.01 J` |   `osd110.elf` [^3] |
__Japan__      | `SCPH-18000` |  `A+/AB`  | `1.20 J` |	`osd130.elf`  |
__America__    | `SCPH-30001` |   `B/B'`  | `1.10 A` |   `osd120.elf`  |
__America__    | `SCPH-30001` |   `C/C'`  | `1.20 A` |	`osd130.elf`	|
__Europe__     | `SCPH-30002`/`3`/`4` | `C/C'` | `1.20 E` | `osd130.elf` |
__All__        | Most models  | `D` and newer | `1.50` and newer | `osdXXX.elf` or `osdmain.elf` (in that order)
__Japan__      | PSX (`DESR`)| - |  `1.80` or `2.10` | `xosdmain.elf`

[^3]: __Protokernel system update:__ theese files are used only by Protokernel PS2, FreeMcBoot installer pastes kernel patches that also redirect the system update into the executable used by the `SCPH-18000` patching the kernel and loading FreeMcBoot at the same time. However: Only Browser 2.0 is capable of patching properly and fully this early kernel. The source code of those kernel patches can be found [here](https://github.com/ps2homebrew/OSD-Initialization-Libraries/tree/main/kpatch)
