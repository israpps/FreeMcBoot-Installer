EE_BIN = FMCBInstaller.elf

#IOP modules
EE_IOP_OBJS = IOPRP_img.o IOMANX_irx.o FILEXIO_irx.o SIO2MAN_irx.o PADMAN_irx.o MCMAN_irx.o MCSERV_irx.o SECRSIF_irx.o MCTOOLS_irx.o USBD_irx.o USBHDFSD_irx.o POWEROFF_irx.o DEV9_irx.o ATAD_irx.o HDD_irx.o PFS_irx.o
IRX_DIR = irx/compiled

EE_RES_OBJS = background.o buttons.o
EE_OBJS = main.o iop.o UI.o menu.o libsecr.o pad.o system.o graphics.o ReqSpaceCalc.o font.o $(EE_RES_OBJS) $(EE_IOP_OBJS) mctools_rpc.o

EE_INCS := -I$(PS2SDK)/ports/include -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I./irx/source/secrsif/src -I./irx/source/mctools/src/
EE_LDFLAGS := -L$(PS2SDK)/ports/lib -L$(PS2SDK)/ee/lib -L$(PS2DEV)/ee/ee/lib -Tlinkfile -s
EE_LIBS = -lgs -lpng -lz -lm -lfreetype -lpoweroff -lcdvd -lmc -lpadx -lhdd -lfileXio -lpatches -liopreboot -lc -lkernel
EE_GPVAL = -G8192
EE_CFLAGS += -Os -mgpopt $(EE_GPVAL)

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

%.o : %.S
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

%.o : %.s
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

$(EE_BIN) : $(EE_OBJS) $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) $(EE_CFLAGS) -nostartfiles $(EE_LDFLAGS) -o $(EE_BIN) $(EE_OBJS) $(PS2SDK)/ee/startup/crt0.o $(EE_LIBS)

all:
	$(MAKE) $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_BIN_REL) $(EE_OBJS) *_irx.c background.c buttons.c IOPRP_img.c

background.c: resources/background.png
	bin2c resources/background.png background.c background

buttons.c: resources/buttons.png
	bin2c resources/buttons.png buttons.c buttons

POWEROFF_irx.c: $(PS2SDK)/iop/irx/poweroff.irx
	bin2c $(PS2SDK)/iop/irx/poweroff.irx POWEROFF_irx.c POWEROFF_irx

DEV9_irx.c: $(PS2SDK)/iop/irx/ps2dev9.irx
	bin2c $(PS2SDK)/iop/irx/ps2dev9.irx DEV9_irx.c DEV9_irx

IOMANX_irx.c: $(PS2SDK)/iop/irx/iomanX.irx
	bin2c $(PS2SDK)/iop/irx/iomanX.irx IOMANX_irx.c IOMANX_irx

FILEXIO_irx.c: $(PS2SDK)/iop/irx/fileXio.irx
	bin2c $(PS2SDK)/iop/irx/fileXio.irx FILEXIO_irx.c FILEXIO_irx

SIO2MAN_irx.c: $(PS2SDK)/iop/irx/freesio2.irx
	bin2c $(PS2SDK)/iop/irx/freesio2.irx SIO2MAN_irx.c SIO2MAN_irx

PADMAN_irx.c: $(PS2SDK)/iop/irx/freepad.irx
	bin2c $(PS2SDK)/iop/irx/freepad.irx PADMAN_irx.c PADMAN_irx

MCMAN_irx.c: $(PS2SDK)/iop/irx/mcman.irx
	bin2c $(PS2SDK)/iop/irx/mcman.irx MCMAN_irx.c MCMAN_irx

MCSERV_irx.c: $(PS2SDK)/iop/irx/mcserv.irx
	bin2c $(PS2SDK)/iop/irx/mcserv.irx MCSERV_irx.c MCSERV_irx

SECRSIF_irx.c: $(IRX_DIR)/secrsif.irx
	bin2c $(IRX_DIR)/secrsif.irx SECRSIF_irx.c SECRSIF_irx

MCTOOLS_irx.c: $(IRX_DIR)/mctools.irx
	bin2c $(IRX_DIR)/mctools.irx MCTOOLS_irx.c MCTOOLS_irx

USBD_irx.c: $(PS2SDK)/iop/irx/usbd.irx
	bin2c $(PS2SDK)/iop/irx/usbd.irx USBD_irx.c USBD_irx

USBHDFSD_irx.c: $(PS2SDK)/iop/irx/usbhdfsd.irx
	bin2c $(PS2SDK)/iop/irx/usbhdfsd.irx USBHDFSD_irx.c USBHDFSD_irx

IOPRP_img.c: $(IRX_DIR)/IOPRP.img
	bin2c $(IRX_DIR)/IOPRP.img IOPRP_img.c IOPRP_img

ATAD_irx.c: $(PS2SDK)/iop/irx/ps2atad.irx
	bin2c $(PS2SDK)/iop/irx/ps2atad.irx ATAD_irx.c ATAD_irx

HDD_irx.c: $(PS2SDK)/iop/irx/ps2hdd-osd.irx
	bin2c $(PS2SDK)/iop/irx/ps2hdd-osd.irx HDD_irx.c HDD_irx

PFS_irx.c: $(PS2SDK)/iop/irx/ps2fs.irx
	bin2c $(PS2SDK)/iop/irx/ps2fs.irx PFS_irx.c PFS_irx

include $(PS2SDK)/Defs.make
