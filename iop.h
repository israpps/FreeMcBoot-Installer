#define IOP_MOD_HDD	0x01
#define IOP_MOD_SECRSIF	0x02
#define IOP_MOD_MCTOOLS	0x04
#define IOP_LIBSECR_IMG	0x40
#define IOP_REBOOT	0x80

#define IOP_MOD_SET_MAIN (IOP_MOD_HDD|IOP_MOD_SECRSIF|IOP_LIBSECR_IMG)				//Main module set

int IopInitStart(unsigned int flags);
void IopDeinit(void);

