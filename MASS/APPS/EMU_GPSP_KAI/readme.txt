This is port of gpSP-kai 3.3 test 1 for Playstation 2. It's using sound core from gpspmod. Most of games should run at full speed without using scaling option. It's possible to use auto or manual frame skip to increase speed. There is also option to select interpreter mode but it's much slower than asm core. 

Main features:
- run unzipped and zipped roms (32MB roms must be uncompressed)
- selectable frameskip option
- support hd modes like: 480p, 720p and 1080i
- scaling options
- running roms from hdd, mass, mc

Installation
At startup gpsp will search for main path where must be placed: "cfg" folder with configuration files, bios file named "gba_bios.bin", "game_config.txt" file. There also will be saved configuration file "gpsp.cfg" while exiting. Main path can be set in three ways: 

- hdd boot path compatible with FHDB
- configuration file "mc0:/SYS-CONF/GPSP_KAI.CFG"
- boot path from mass or mc

In "cfg" folder there are config files "dir.cfg", "language.fnt" and translation files "language.msg". In "dir.cfg" can be set paths to separate folder for games cfg, savestates and etc. "language.fnt" sets path for font files "5x10rk.fbm" and "knj10.fbm". These are needed to run gpsp. It's possible to set paths to hdd partition (example: "hdd0:part_name/directory_name") but only one and the same can be used for main path and paths in cfg files. Gpsp language is automatically sets to Playstation 2 console language and can be changed in misc menu. Currently there is no other translation than English. 

Game compatibility should be same as psp version of gpsp-kai. If rom can't work properly try to unpack it (when was compressed) or use version from other region. Eventual use interpreter mode.

In this release there are two elfs. One with proper iop reset for protokernel consoles and second with normal. On my SCPH-30003 I'm getting wrong date using ps2time library (also in ps2linux) after iop reset compatible with protokernel. Because of that problem I made two version.
It's early release so treat it like a beta version.

I would like to thank SP193 for fixing dynarec core.

Special thanks for:
Exophase - main developer
takka -  for his work at gpsp-kai version
unknown coder for gpsp mod version