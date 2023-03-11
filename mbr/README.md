Homebrew MBR program	- 2019/01/07
------------------------------------

> This package contains the source code for the homebrew MBR program, which aims to function similarly to the Sony MBR program, which is used for booting HDDOSD installations.

It supports the same integrity checks as the Sony MBR program, and boots one of the following KELF targets:

- `pfs0:/osd/osdmain.elf`  [^1]
- `pfs0:/osd/hosdsys.elf`
- `pfs0:/osd100/hosdsys.elf`
 
If the HDD has been deemed to have filesystem corruption, FSCK in one of these locations will be launched, in this order:
- `pfs0:/fsck/fsck.elf`  [^1]
- `pfs0:/fsck/fsck.elf`
- `pfs0:/fsck100/fsck.elf`

Failing to do so, it'll fall back to the OSD.

Notes for programmers:
----------------------
This MBR is split into two parts: The main program and an embedded EELOAD module, which is used to load the HDDOSD.

The embedded __EELOAD__ module is hardcoded in the MBR program to expect the EELOAD module to run at `0x00084000`.
Unlike `rom0:EELOAD` and various other __EELOAD__ programs, the SDK used to build the MBR has the alarm functions patch (which resides at `0x00082000`).

The MBR program itself must run from `0x00100000` when decrypted by rom0:HDDLOAD or by a Sony OSD update.

Both programs are headerless, binary blobs of code.


[^1]: (Unofficial target, for FHDB)