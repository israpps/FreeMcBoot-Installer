#define FNT_ATLAS_WIDTH  128
#define FNT_ATLAS_HEIGHT 128
#define FNT_MAX_ATLASES  4
#define FNT_CHAR_WIDTH   16
#define FNT_CHAR_HEIGHT  16
#define FNT_TAB_STOPS    4

int FontInit(struct UIDrawGlobal *gsGlobal, const char *FontFile);
int FontInitWithBuffer(struct UIDrawGlobal *gsGlobal, void *buffer, unsigned int size);
int AddSubFont(struct UIDrawGlobal *gsGlobal, const char *FontFile);
int AddSubFontWithBuffer(struct UIDrawGlobal *gsGlobal, void *buffer, unsigned int size);
void FontDeinit(void);
int FontReset(struct UIDrawGlobal *gsGlobal); // Performs a partial re-initialization of the Font library and re-allocates VRAM. Used when VRAM has been cleared.
void FontPrintfWithFeedback(struct UIDrawGlobal *gsGlobal, short int x, short int y, short int z, float scale, GS_RGBAQ colour, const char *string, short int *xRel, short int *yRel);
void FontPrintf(struct UIDrawGlobal *gsGlobal, short int x, short int y, short int z, float scale, GS_RGBAQ colour, const char *string);
int FontGetGlyphWidth(struct UIDrawGlobal *gsGlobal, wint_t character);
