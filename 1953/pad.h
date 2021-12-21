void PadInitPads(void);
void PadDeinitPads(void);

int ReadPadStatus_raw(int port, int slot);
int ReadCombinedPadStatus_raw(void);
int ReadPadStatus(int port, int slot);
int ReadCombinedPadStatus(void);
