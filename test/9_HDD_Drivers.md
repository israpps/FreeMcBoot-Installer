---
sort: 9
---

# FreeMcBoot HDD Driver's

The FreeMcBoot HDD Drivers are a pack of 3 `.IRX` files required to activate a special feature in FreeMcBoot: the possibility of executing the MBR program from the internal HDD.

Now you might be asking to yourself:

> Why would you want to do that? My PS2 boots it on its own!

Well, the truth is that not every PS2 with internal HDD can Boot the MBR autonomously, the ProtoKernels (`SCPH-1X000`) and `SCPH-70XXX` Can't Boot the MBR on their own.

Those HDD drivers must be pasted into any of the system update folders.

The installer will paste those drivers into the Japanese system update folder automatically if it detects that your PS2 is a ProtoKernel or if you chose any install mode compatible with ProtoKernels.

The package provided here has a lovely custom icon for the folder. 


After pasting the HDD drivers, the only thing you need to do is setting the config `Skip HDD Updates` to `OFF` On the configurator
