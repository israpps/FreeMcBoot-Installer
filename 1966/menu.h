void MainMenu(void);
void DrawFileCopyProgressScreen(float PercentageComplete);
void DrawMemoryCardDumpingProgressScreen(float PercentageComplete, unsigned int rate, unsigned int SecondsRemaining);
void DrawMemoryCardRestoreProgressScreen(float PercentageComplete, unsigned int rate, unsigned int SecondsRemaining);
void DisplayOutOfSpaceMessage(unsigned int AvailableSpace, unsigned int RequiredSpace);
void DisplayOutOfSpaceMessageHDD_APPS(unsigned int AvailableSpace, unsigned int RequiredSpace);
void DisplayOutOfSpaceMessageHDD(unsigned int AvailableSpace, unsigned int RequiredSpace);
void RedrawLoadingScreen(unsigned int frame);
