---
sort: 11
---

# How to properly install FreeHdBoot 

> This written tutorial was made to aid you on the procedure to properly format the ps2 HDD and install FreeHdBoot

First of all: 

## important notes:
```note
This tutorial will guide you on the format and install procedure assuming that you don't have access to any application launching utility.
If you posses a method to launch wLaunchELF on your own, start from [this step](https://israpps.github.io/FreeMcBoot-Installer/test/FHDB-TUTO.html#format-the-hdd) and Don't download the software requirements marked in red
```
## Requirements

### Hardware

- PS2 Network adapter 
  + for [expansion bay models](https://upload.wikimedia.org/wikipedia/commons/thumb/c/c3/PS2-Fat-Console-Back.jpg/1920px-PS2-Fat-Console-Back.jpg) `SCPH-3xxxx` or `SCPH-5xxxx` the adapters are [`SCPH-10281`](https://www.picclickimg.com/DLkAAOSwcCljAj5f/Sony-PlayStation-2-PS2-Network-Adapter-SCPH-10281.jpg) or [`SCPH-10350`](https://1.bp.blogspot.com/-fcSqrbFqwTc/YSyGFUhDyMI/AAAAAAAAh0Q/B8nFeTbNO_YofSe2c2YcwH6F94hOBkkIACLcBGAsYHQ/s2048/PS2%2BSCPH%2B30004%2BR%2B%25282%2529%2BRed2.jpg) - (generic adapters usage is discouraged due to their poor design)
  + for [PCMCIA models](https://media.karousell.com/media/photos/products/2021/7/1/looking_for_ps2_fat_pc_card_sl_1625155573_cbef2ae8_progressive.jpg) `SCPH-1X000` the needed parts are([`SCPH-20400`](https://i.ebayimg.com/images/g/1yIAAOSw3txiZ5hY/s-l500.jpg) + [`SCPH-10190`](https://i.ebayimg.com/images/g/K3AAAOSwsGFdWkC7/s-l400.jpg) + [`SCPH-10200`](https://auctions.c.yimg.jp/images.auctions.yahoo.co.jp/image/dr000/auc0410/users/3/6/4/5/no0123a-img900x1200-1540094399ub3feh3827.jpg)) you can also search for the full kit (either [`SCPH-10210`](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcT_vQK8vOaYRPKnXD_YVgvTMzejPte4Hh5YVg-r76MGNHKiBgjdT0t717mCk69d5EIXp8Q&usqp=CAU) or [`SCPH-10390`](https://rr2000.cwaboard.co.uk/gallery/albums/userpics/10001/normal_27768-scph10390.jpg))
- Hard-Drive with a size between 40gb and 2Tb
- USB storage device formatted into FAT32 (free space is not that inportant, 50mb free should suffice)

### Software
- Paste into the USB:
  - [FreeMcBoot/FreeHdBoot installer](./8_Downloads.html) **(DONT FORGET TO UNZIP THE FILE)**
  - wLaunchELF 4.43x_isr [__Download__](https://github.com/israpps/wLaunchELF_ISR/releases/download/latest/BOOT.ELF)

- Download into your PC:
  - [HDL Batch installer](https://www.psx-place.com/resources/hdl-batch-installer.1173/) _(Not used in this tutorial, however, it's the recommended tool to install games)_

{:.text-red}
  - HDD RAW Copy Tool [__Download__](https://hddguru.com/software/HDD-Raw-Copy-Tool/)
  - wLaunchELF KHN HDD image [__Download__](https://github.com/israpps/FreeMcBoot-Installer/raw/gh-pages/__mbr.raw)


## Installation

### Setup the entry point

```note
__This HDD image won't work on ProtoKernel (SCPH-10000, SCPH-15000) or pre-deckard slims (SCPH-70XXX) because those units can't load HDD software autonomously__

If you still want to use FreeHdBoot or HDD-OSD on the models mentioned before, take a look at [this](https://israpps.github.io/FreeMcBoot-Installer/test/9_HDD_Drivers.html)

Use FreeMcBoot as an alternative
```



```warning
There is a small chance that this HDD image won't work the first time on expansion bay models.

For those models, the HDD software execution is controlled by a configuration on the console EEPROM.

If the image doesn't Boot or HDD starts to power on and off in an endless loop, it means the HDD software execution is disabled, find a alternative method (FreeMcBoot or Freedvdboot)

Don't worry, FreeMcBoot installer will enable the HDD software execution when FreeHdBoot installation finishes
```


We are assuming that you don't have access to uLaunchELF.

So we will load a small image of uLaunchELF KHN into the HDD, that should grant access to the wLaunchELF mod that will be used to begin the setup

Using HDD raw copy tool, write the `__mbr.raw` file into the desired HDD.


### Proceed to the PS2

Disconnect the console form the power source (or use the switch on the back)

Plug the HDD to the console and the USB device into one of the USB ports

Connect the console back to he power source (or use the switch from the back again)

Turn on the console, the HDD should be powered on.

In less than 1 minute, you should see wLaunchELF KHN menu (it looks like [this](https://i.ibb.co/j3tPjFD/ule.png))

### Format the HDD

Now that you are in uLaunchELF, enter the filebrowser option.

Then navigate to the `mass:/` device (this is the USB device you plugged before)

Now try to locate the uLaunchELF ELF file that you downloaded before and open it (it should load a new uLaunchELF menú with a different color scheme like seen in [this video](https://youtu.be/AjPm9Pv8jd4))

When it opens up, open filebrowser again.

This time navigate to `MISC/` and open the `HDDManager` subprogram.

After a few seconds, a new menú should appear, showing information about the HDD unit.

Press `R1` and choose the option `Format`

After formatting (that takes aproximately 1 minute) the partition list should reload, showing the default partition structure:

```console
__mbr
__net
__sysconf
__system
__common
```

Now press `R1` again, this time choose the `Create` option

A keyboard should appear, write `+OPL` (__ALL UPPERCASE__), And hit ok.

After that a new dialog will appear, asking you to input partition size.

Choose `768` if your HDD size is smaller than 750gb

If it's larger than 750gb, choose `1024` or `1536` 

Now press ok, the partition table should reload, showing the new `+OPL` partition at the end of the list


Now press triangle, you should go back to the first menú.

### Installing FreeHdBoot

Open filebrowser again.

Navigate to `mass:/` and locate the FreeMcBoot installer folder.

Enter inside it and open the `FMCBinstaller.ELF` file

Wait untill the menú loads, then press R1 and choose __Install FHDB__

If it finished without errors then good job!

The HDD is ready to receive games.

And the FreeHdBoot installer automatically loaded wLaunchELF and OPL into the HDD.

The only thing left to do is configure OPL and install games from the PC

