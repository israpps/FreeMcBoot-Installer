---
sort: 7
---

# Troubleshooting

## Newer versions of FMCB/FHDB doesn't seem to work on my console.

Modchips usually affect some of the newer versions of FMCB/FHDB.

Fake network adapters also tend to affect FreeHdBoot MBR.

## I installed the HDD-OSD, but FHDB switches off my HDD unit and I can't see the HDD from the browser. Why does that happen?

It means that FHDB couldn't locate a HDD-OSD installation. You're probably using a heavily-hacked copy that doesn't have its files stored in the original locations.

## I have the HDD-OSD and FHDB installed properly, but I get a black screen after the "Sony Computer Entertainment" screen. Why?

To aid troubleshooting this symptom, sp193 released a diagnostic tool for identifying potential problems with the HDD unit: HDDChecker

All checks must pass, for the disk to be suitable for running FHDB. This includes the __S.M.A.R.T.__ status. Disks that fail the __S.M.A.R.T.__ test are about to fail, and should be replaced.

If the partition bad sector check fails, it means that your disk has been marked as having bad sectors by a filesystem driver. Using WinHIIP's scan and repair function should solve that, assuming that the bad sector(s) was remapped by the drive successfully. 

Alternatively, a format uLaunchELF 4.43x_isr should solve it too, at the expense of all unbacked-up data
