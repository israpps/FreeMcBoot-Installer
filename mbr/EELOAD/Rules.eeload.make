# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

EE_CC_VERSION := $(shell $(EE_CC) --version 2>&1 | sed -n 's/^.*(GCC) //p')

CRT0_OBJ = crt0.o
MAPFILE = memory.map

EE_INCS := $(EE_INCS) -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I.
EE_LDFLAGS := $(EE_LDFLAGS) -L$(PS2SDK)/ee/lib -Wl,-Map,$(MAPFILE) -s
EE_LIBS := $(EE_LIBS) -lc -lkernel-nopatch

# C compiler flags
EE_CFLAGS := -D_EE -G0 -Os -Wall $(EE_INCS) $(EE_CFLAGS)

# C++ compiler flags
EE_CXXFLAGS := -D_EE -G0 -Os -Wall $(EE_INCS) $(EE_CXXFLAGS)

# Assembler flags
EE_ASFLAGS := $(EE_ASFLAGS)

# Externally defined variables: EE_BIN, EE_OBJS, EE_LIB, LINKFILE, CRT0_OBJ_S

# These macros can be used to simplify certain build rules.
EE_C_COMPILE = $(EE_CC) $(EE_CFLAGS)
EE_CXX_COMPILE = $(EE_CXX) $(EE_CXXFLAGS)

# Extra macro for disabling the automatic inclusion of the built-in CRT object(s)
ifeq ($(EE_CC_VERSION),3.2.2)
	EE_NO_CRT = -mno-crt0
else ifeq ($(EE_CC_VERSION),3.2.3)
	EE_NO_CRT = -mno-crt0
else
	EE_NO_CRT =
endif

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

%.o : %.s
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

%.o : %.S
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_BIN) : $(EE_OBJS) $(CRT0_OBJ)
	$(EE_CC) $(EE_NO_CRT) -nostartfiles -T$(LINKFILE) $(EE_CFLAGS) \
		-o $(EE_BIN) $(CRT0_OBJ) $(EE_OBJS) $(EE_LDFLAGS) $(EE_LIBS)
#	$(EE_STRIP) -s -d -R .mdebug.eabi64 -R .reginfo -R .comment $(EE_BIN)
	$(EE_OBJCOPY) -O binary $(EE_BIN) EELOAD

$(CRT0_OBJ) : $(CRT0_OBJ_S)
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

clean::
	rm -f $(EE_BIN) $(EE_OBJS) $(CRT0_OBJ) $(MAPFILE) EELOAD
