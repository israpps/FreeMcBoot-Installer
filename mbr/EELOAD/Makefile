EE_BIN = EELOAD.elf
EE_OBJS = main.o

CRT0_OBJ_S = crt0-eeload.s
LINKFILE = linkfile
EE_CFLAGS = -Os -mgpopt -G8192

EE_LIBS = -lfileXio -liopreboot -lpatches

include $(PS2SDK)/Defs.make
include Rules.eeload.make
