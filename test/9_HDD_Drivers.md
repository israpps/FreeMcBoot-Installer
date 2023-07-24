---
sort: 9
---

# FreeMcBoot HDD Driver's

The FreeMcBoot HDD Drivers are a pack of 3 `.IRX` files required to activate a special feature in FreeMcBoot: the possibility of executing the MBR program from the internal HDD.

Now you might be asking to yourself:

> Why would you want to do that? My PS2 boots it on its own!

Well, the truth is that not every PS2 with internal HDD can Boot the MBR autonomously, the PCMCIA models (`SCPH-1X000`) and `SCPH-70XXX` Can't Boot the MBR on their own.

Those HDD drivers must be pasted into the system update folder used by the console that needs this update.

The installer will paste those drivers into the Japanese system update folder automatically if it detects that your PS2 is a PCMCIA model or if you chose any install mode compatible with PCMCIA models.


since these modules are pasted on the system update folder, they're used only by the specified regiom...

if you have a PCMCIA PS2, paste them straight into the `BIEXEC-SYSTEM` folder of your memory card.

If you have a 70xxx model, you must paste it into the system folder used by that console. to find out, boot wLaunchELF 4.43x_ISR, go to 
> filebrowser > MISC > Debug Info

there, check on the line that says "main system update KELF". it will tell you the system folder used by your console. and dont worry. only 4 of them exist

- `BIEXEC-SYSTEM`: for japanese PS2
- `BCEXEC-SYSTEM`: for Chinese PS2
- `BEEXEC-SYSTEM`: for PAL PS2 (oceania, england, europe, russia : SCPH-xxxZZ, where ZZ can be: `02`, `03`, `04`, `08`)
- `BAEXEC-SYSTEM`: for North america PS2 models and any region that doesnt fall into the previous categories (eg: `SCPH-xxx06` from Hong Kong)

FreeMcBoot will try to boot the MBR program if the 3 of these drivers are present and all of them return an OK state to the program...
 the first two will always run OK... but HDDLOAD will return an error if the console does not have HDD boot enabled via FreeMcBoot installer, softdev2 installer or [KELFBinder 2](https://www.psx-place.com/resources/kelfbinder-2.1324/).
