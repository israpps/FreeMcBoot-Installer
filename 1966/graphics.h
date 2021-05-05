#define GS_SETREG_RGBAQ(r, g, b, a, q) (GS_RGBAQ){(r), (g), (b), (a), (q)}

#define GS_WHITE GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00)
#define GS_BLACK GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00)
#define GS_GREY GS_SETREG_RGBAQ(0x30,0x30,0x30,0x80,0x00)
#define GS_LGREY GS_SETREG_RGBAQ(0x50,0x50,0x50,0x80,0x00)
#define GS_DBLUE GS_SETREG_RGBAQ(0x00,0x00,0x50,0x80,0x00)
#define GS_BLUE GS_SETREG_RGBAQ(0x00,0x00,0x80,0x80,0x00)
#define GS_RED GS_SETREG_RGBAQ(0x80,0x00,0x00,0x80,0x00)
#define GS_GREEN GS_SETREG_RGBAQ(0x00,0x80,0x00,0x80,0x00)
#define GS_LBLUE_TRANS GS_SETREG_RGBAQ(0x00,0x00,0x80,0x40,0x00)
#define GS_YELLOW GS_SETREG_RGBAQ(0x80,0x80,0x30,0x80,0x00)

#define GS_WHITE_FONT GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00)
#define GS_GREY_FONT GS_SETREG_RGBAQ(0x30,0x30,0x30,0x80,0x00)
#define GS_YELLOW_FONT GS_SETREG_RGBAQ(0x80,0x80,0x30,0x80,0x00)
#define GS_BLUE_FONT GS_SETREG_RGBAQ(0x30,0x30,0x80,0x80,0x00)

/* Button types, for use with DrawButtonLegend() */
enum BUTTON_TYPE{
	BUTTON_TYPE_CIRCLE=0,
	BUTTON_TYPE_CROSS,
	BUTTON_TYPE_SQUARE,
	BUTTON_TYPE_TRIANGLE,
	BUTTON_TYPE_L1,
	BUTTON_TYPE_R1,
	BUTTON_TYPE_L2,
	BUTTON_TYPE_R2,
	BUTTON_TYPE_L3,
	BUTTON_TYPE_R3,
	BUTTON_TYPE_START,
	BUTTON_TYPE_SELECT,
	BUTTON_TYPE_RSTICK,
	BUTTON_TYPE_UP_RSTICK,
	BUTTON_TYPE_DOWN_RSTICK,
	BUTTON_TYPE_LEFT_RSTICK,
	BUTTON_TYPE_RIGHT_RSTICK,
	BUTTON_TYPE_LSTICK,
	BUTTON_TYPE_UP_LSTICK,
	BUTTON_TYPE_DOWN_LSTICK,
	BUTTON_TYPE_LEFT_LSTICK,
	BUTTON_TYPE_RIGHT_LSTICK,
	BUTTON_TYPE_DPAD,
	BUTTON_TYPE_LR_DPAD,
	BUTTON_TYPE_UD_DPAD,
	BUTTON_TYPE_UP_DPAD,
	BUTTON_TYPE_DOWN_DPAD,
	BUTTON_TYPE_LEFT_DPAD,
	BUTTON_TYPE_RIGHT_DPAD,

	BUTTON_TYPE_COUNT
};

//Special button types
#define BUTTON_TYPE_SYS_SELECT	0x40
#define	BUTTON_TYPE_SYS_CANCEL	0x41

#define GIF_PACKET_MAX	1

struct UIDrawGlobal{
	unsigned char vmode, interlaced, ffmd, psm;
	unsigned short int width, height;
	GS_DRAWENV	draw_env;
	GS_DISPENV	disp_env;
	GS_GIF_PACKET	packets[GIF_PACKET_MAX];
	GS_PACKET_TABLE	giftable;
};

struct ClutImage{
	GS_IMAGE texture;
	GS_IMAGE clut;
};

int LoadBackground(struct UIDrawGlobal *gsGlobal, GS_IMAGE* Texture);
int LoadPadGraphics(struct UIDrawGlobal *gsGlobal, struct ClutImage* PadGraphics);
void DrawSetFilterMode(struct UIDrawGlobal *gsGlobal, int mode);
void DrawLine(struct UIDrawGlobal *gsGlobal, short int x1, short int y1, short int x2, short int y2, short int z, GS_RGBAQ rgbaq);
void DrawSprite(struct UIDrawGlobal *gsGlobal, short int x1, short int y1, short int x2, short int y2, short int z, GS_RGBAQ rgbaq);
void DrawSpriteTextured(struct UIDrawGlobal *gsGlobal, GS_IMAGE *texture, short int x1, short int y1, short int u1, short int v1, short int x2, short int y2, short int u2, short int v2, short int z, GS_RGBAQ rgbaq);
void DrawSpriteTexturedClut(struct UIDrawGlobal *gsGlobal, GS_IMAGE *texture, GS_IMAGE *clut, short int x1, short int y1, short int u1, short int v1, short int x2, short int y2, short int u2, short int v2, short int z, GS_RGBAQ rgbaq);
void UploadClut(struct UIDrawGlobal *gsGlobal, GS_IMAGE *clut, const void *buffer);
void DrawBackground(struct UIDrawGlobal *gsGlobal, GS_IMAGE *background);
void DrawButtonLegendWithFeedback(struct UIDrawGlobal *gsGlobal, struct ClutImage* PadGraphicsTexture, unsigned char ButtonType, short int x, short int y, short int z, short int *xRel);
void DrawButtonLegend(struct UIDrawGlobal *gsGlobal, struct ClutImage* PadGraphicsTexture, unsigned char ButtonType, short int x, short int y, short int z);
void DrawProgressBar(struct UIDrawGlobal *gsGlobal, float percentage, short int x, short int y, short int z, short int len, GS_RGBAQ colour);
void SyncFlipFB(struct UIDrawGlobal *gsGlobal);
void ExecSyncClear(struct UIDrawGlobal *gsGlobal);
