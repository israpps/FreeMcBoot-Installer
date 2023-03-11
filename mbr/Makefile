EE_BIN = MBR.elf
EE_BIN_RAW = MBR.bin
EE_BIN_KELF = MBR.KELF

#IOP modules
EE_IOP_OBJS = DEV9_irx.o IOMANX_irx.o FILEXIO_irx.o ATAD_irx.o HDD_irx.o PFS_irx.o

EE_OBJS = main.o EmbeddedEELOAD_bin.o $(EE_IOP_OBJS)

EE_INCS := -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I.
EE_LDFLAGS := -L$(PS2SDK)/ee/lib -s
EE_LIBS := -lpatches -lfileXio -lc -lkernel
EE_CFLAGS += -mgpopt -G8192
EE_TEMP_FILES = IOMANX_irx.c FILEXIO_irx.c DEV9_irx.c ATAD_irx.c HDD_irx.c PFS_irx.c \
		EmbeddedEELOAD_bin.c EELOAD.bin

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

%.o : %.s
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

%.o : %.S
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_BIN) : $(EE_OBJS) $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -mno-crt0 -nostartfiles -Tlinkfile $(EE_LDFLAGS) \
		-o $(EE_BIN) $(PS2SDK)/ee/startup/crt0.o $(EE_OBJS) $(EE_LIBS)

raw: $(EE_BIN_RAW)
kelf: $(EE_BIN_KELF)

$(EE_BIN_KELF): $(EE_BIN_RAW)
	kelftool_dnasload.exe encrypt mbr $< $@ 

$(EE_BIN_RAW): $(EE_BIN)
	$(EE_OBJCOPY)  -O binary -v $< $@ 

clean:
	$(MAKE) -C EELOAD clean
	rm -f $(EE_BIN) $(EE_OBJS) $(EE_TEMP_FILES) $(EE_BIN_RAW) $(EE_BIN_KELF)

IOMANX_irx.c:
	bin2c $(PS2SDK)/iop/irx/iomanX.irx IOMANX_irx.c IOMANX_irx

FILEXIO_irx.c:
	bin2c $(PS2SDK)/iop/irx/fileXio.irx FILEXIO_irx.c FILEXIO_irx

DEV9_irx.c:
	bin2c $(PS2SDK)/iop/irx/ps2dev9.irx DEV9_irx.c DEV9_irx

ATAD_irx.c:
	bin2c $(PS2SDK)/iop/irx/ps2atad.irx ATAD_irx.c ATAD_irx

HDD_irx.c:
	bin2c $(PS2SDK)/iop/irx/ps2hdd-osd.irx HDD_irx.c HDD_irx

PFS_irx.c:
	bin2c $(PS2SDK)/iop/irx/ps2fs.irx PFS_irx.c PFS_irx

EmbeddedEELOAD_bin.c:
	$(MAKE) -C EELOAD
	$(EE_OBJCOPY) -Obinary EELOAD/EELOAD.elf EELOAD.bin
	bin2c EELOAD.bin EmbeddedEELOAD_bin.c EmbeddedEELOAD_bin

include $(PS2SDK)/Defs.make
