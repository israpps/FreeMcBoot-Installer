---
sort: 6
---

# Frequent Questions


## What is FreeMcBoot (FMCB)?

Free Memory Card Boot (FMCB) is a homebrew software which is designed to setup your PlayStation 2 console and provide you with a means of launching homebrew software, without the need for any extra hardware, modifications to your console or dangerous tricks like the legendary "swap trick".

FMCB was originally developed by Neme and Jimmikaelkael, but they have been kind enough to provide sp193 with access to its source code, he continued with the creation and development of the 1.9 series.

## What is FreeHdBoot (FHDB)?

Free Harddisk Drive Boot (FHDB) is basically FMCB that gets installed onto the PlayStation 2 console's HDD unit. It does everything like FMCB does, it was meant to be used with the Sony HDD-OSD (Browser update v2.00), but it can be used individually.

## Does FMCB/FHDB support my console?

All PlayStation 2 consoles excluding the late models and PS2TV should be supported, although FHDB can only be used on consoles that can have a HDD connected to them (From `SCPH-10000` to `SCPH-70XXX`).

The unsupported late PlayStation 2 console models start from the SCPH-90000, manufactured starting in the 3rd quarter of year 2008 (datecode 8C). They are __usually__ R-chassis models. They have boot ROM v2.30 or newer, and will never Boot FreeMcBoot autonomously. 

However, not all R-chassis and/or consoles with datecode 8C cannot work with FMCB (since the patch was made during that time), To be sure you have to check the ROM version from uLaunchELF.

## Why are there so many versions of FreeMcBoot on the download page?

FreeMcBoot is one of the most famous and massively consumed homebrew softwares around. but it has one major issue, due to how it works and was designmed, it has __LOTS__ of conflicts with modchiped consoles. the newer the FreeMcBoot version, the less chances of working properly.

So follow this rule:
If your console has no modchip install v1.966. else, try another one of the offered options.

FreeMcBoot v1.8 is excluded from this repository due to many flaws that it has, that were corrected during development of v1.9xx series.

## What is the "OSD"?

The "OSD" is the PlayStation 2 browser, the user-interface which you see when you switch on the console without a game inserted.

## What's the "HDD OSD"?

The "HDD OSD" is also known as the "Browser update v2.00", which adds support for the HDD unit. Only officially [^70kHDD] supported by "FAT" PlayStation 2 consoles and was only officially released in territories which received the HDD unit.

[^70kHDD]: HDD-OSD can work on any model with HDD. including `SCPH-70xxx` and PSX-DESR. but sony only wanted it to be used on FAT models.

## What's the boot ROM?

As for the boot ROM, it refers to the chip on the console which contains the OSD (amongst a lot of things). Sony calls it the boot ROM, and it makes sense because it doesn't really provide basic input/output like the term BIOS describes.

## What happened to FMCB original devs?
Jimmikaelkael and neme were very busy with life for quite a long while, and sp193 had a number of ideas that he wanted to try out.

Jimmikaelkael gave sp193 permission to work on FMCB, starting from February 2013.

## Will FMCB/FHDB allow me to play pirated game discs?

No. It's a hardware limitation, which cannot and won't be circumvented by FMCB alone, take a look at MechaPWN.

## Why FMCB doesn't work on some SCPH-9XXXX?

Most PS2 units have a system update feature. (Basically, the PS2 can launch a special encrypted executable from any memory card slot, from a specific path)

Normal FreeMcBoot pretends to be a system update.

When Sony discovered the existence of FreeMcBoot (mid 2008), they made a BOOT ROM update for the console model that was being manufactured during that time (the SCPH-90XXX).

That BOOT ROM update basically consisted on the total removal of both system and DVD-player updates.

To put it short:

A lot of SCPH-90XXX units and the [PS2-TV](https://upload.wikimedia.org/wikipedia/commons/thumb/1/16/SonyBRAVIA_PS2.jpg/800px-SonyBRAVIA_PS2.jpg) can't Boot FreeMcBoot autonomously

## Does FHDB support non-Sony HDDs?

Yes!

If you want to use HDD-OSD make sure to get a patched version


## What's the maximum HDD size?

Disks up to 2TB are supported by FHDB.

If you want to use HDD-OSD make sure to get a copy that has patched ATAD.IRX, otherwise it Will only recognize the first 127GB of the drive.

## Where can I get HDD-OSD?

HDD-OSD can't be redistributed freely, it's copyrighted software, made by $ony, however, you can visit the [official PS2 homebrew community discord server](https://discord.gg/jke69mRCYd) and ask me (El_isra) or krHACKen for help.

## I see weird icons in the OSD!

Both HDD-OSD and console ROM OSDs have a different icon map. This is why CNFs shouldn't be just copied over (FMCB CNF for FHDB, or vice-versa) or resaved on a different device (HDD to memory card, or vice-versa).

This can be manually corrected via configurator.

## I see weird text in the OSD of my `SCPH-1X000`!

These early consoles have OSD in their boot ROMs that doesn't support the text formatting syntax which all newer console OSDs support. 

You can edit FREEMCB.CNF to use ASCII instead of the special PS2 characters.

Special chars will be seen on the configurator as an "inverted L", followed by the letter `c` and some numbers or chars after it.




