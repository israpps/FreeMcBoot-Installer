---
sort: 4
---

# Warnings

## Multi install

If you make a multi-install, __do not delete any of the__ `B?EXEC-SYSTEM` __folders or any of the__ `osd*.elf` __files inside them!__

Doing so might result in data loss. __Do not delete__ `uninstall.dat` __from the__ `SYS-CONF` __folder either, or you will probably be stuck with the multi-install files forever.__ 



Also, multi install has proven to be capable of causing random corruptions of the memory card, unlike the normal install, Wich is safer.


## FreeHdBoot install

Make sure to format the HDD with [wLaunchELF 4.43x_isr](https://github.com/israpps/wLaunchELF_ISR/releases/download/latest/BOOT.ELF) before installing FreeHdBoot, __DON'T USE WINHIIP OR ANY OTHER OUTDATED TOOL__.

## List of Conflictive Modchips:
- Mars Pro GM-816-HD: OSD freezes when its about to render the menu items
- DMS4 toxic os based clones: OSDSYS hacking crashes
- Matrix 1.99: Breaks ELF loader of FreeMcBoot 1.966
