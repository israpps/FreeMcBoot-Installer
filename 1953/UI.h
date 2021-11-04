#include "lang.h"

#define UI_OFFSET_X	8
#define UI_OFFSET_Y	8
#define UI_FONT_WIDTH	FNT_CHAR_WIDTH
#define UI_FONT_HEIGHT	FNT_CHAR_HEIGHT
#define UI_TAB_STOPS	FNT_TAB_STOPS

//In VSYNC ticks
#define UI_PAD_REPEAT_START_DELAY	20
#define UI_PAD_REPEAT_DELAY		5

enum MENU_ITEM{
	MITEM_SEPERATOR,
	MITEM_TAB,
	MITEM_SPACE,
	MITEM_BREAK,
	MITEM_DASH,
	MITEM_COLON,
	MITEM_DOT,
	MITEM_SLASH,
	MITEM_PROGRESS,
	MITEM_LABEL,	//Last non-selectable item.

	MITEM_STRING,
	MITEM_VALUE,
	MITEM_BUTTON,
	MITEM_TOGGLE,
	MITEM_ENUM,

	MITEM_TERMINATOR,

	MITEM_COUNT
};

enum MENU_ITEM_FORMAT{
	MITEM_FORMAT_DEC	= 0,
	MITEM_FORMAT_UDEC,
	MITEM_FORMAT_HEX,
	MITEM_FORMAT_POINTER,
	MITEM_FORMAT_FLOAT,

	MITEM_FORMAT_COUNT
};

#define MITEM_FLAG_HIDDEN		0x01
#define MITEM_FLAG_READONLY		0x02
#define MITEM_FLAG_DISABLED		0x04
#define MITEM_FLAG_UNIT_PREFIX		0x08	//For formats with a unit prefix, show it.
#define MITEM_FLAG_POS_ABS		0x10	//Coordinates are absolute.
#define MITEM_FLAG_POS_MID		0x20	//Aligned to the middle. Only for buttons.

struct UIMenuItem{
	unsigned char type;
	unsigned char id;	//ID 0 is reserved for items that do not need to be tracked (i.e. labels).
	unsigned char flags;
	unsigned char format;
	unsigned char width;
	short int x;
	short int y;
	union{	//Limits do not have to be specified, if the values are not going to be changeable by the user.
		struct{
			int value;
			int min, max;
		}value;
		struct{
			const char *buffer;
			int maxlen;
		}string;
		struct{
			const int *labels;
			int count;
			int selectedIndex;
		}enumeration;
		struct{
			int id;
			int TextWidth;
		}label;
	};
};

struct UIBtnHint{
	short int button;
	short int label;
};

struct UIMenu{
	struct UIMenu *next, *prev;
	struct UIMenuItem *items;
	struct UIBtnHint hints[2];
};

enum UI_MENU_TRANSITION{
	UIMT_LEFT_OUT,
	UIMT_RIGHT_OUT,
	UIMT_LEFT_IN,
	UIMT_RIGHT_IN,
	UIMT_FADE_IN,
	UIMT_FADE_OUT,
};

const char *GetUIString(unsigned int id);
const char *GetUILabel(unsigned int id);
int InitializeUI(int BufferFont);
int ReinitializeUI(void);
void DeinitializeUI(void);
int ShowMessageBox(int Option1Label, int Option2Label, int Option3Label, int Option4Label, const char *message, int title);
void DisplayWarningMessage(unsigned int message);
void DisplayErrorMessage(unsigned int message);
void DisplayInfoMessage(unsigned int message);
int DisplayPromptMessage(unsigned int message, unsigned char Button1Label, unsigned char Button2Label);
void DisplayFlashStatusUpdate(unsigned int message);

struct UIMenuItem *UIGetItem(struct UIMenu *menu, unsigned char id);
void UISetVisible(struct UIMenu *menu, unsigned char id, int visible);
void UISetReadonly(struct UIMenu *menu, unsigned char id, int readonly);
void UISetEnabled(struct UIMenu *menu, unsigned char id, int enabled);
void UISetValue(struct UIMenu *menu, unsigned char id, int value);
int UIGetValue(struct UIMenu *menu, unsigned char id);
void UISetLabel(struct UIMenu *menu, unsigned char id, int label);
void UISetString(struct UIMenu *menu, unsigned char id, const char *string);
void UISetType(struct UIMenu *menu, unsigned char id, unsigned char type);
void UISetFormat(struct UIMenu *menu, unsigned char id, unsigned char format, unsigned char width);
void UISetEnum(struct UIMenu *menu, unsigned char id, const int *labels, int count);
void UISetEnumSelectedIndex(struct UIMenu *menu, unsigned char id, int selectedIndex);
int UIGetEnumSelectedIndex(struct UIMenu *menu, unsigned char id);
void UIDrawMenu(struct UIMenu *menu, unsigned short int frame, short int StartX, short int StartY, short int selection);
void UITransition(struct UIMenu *menu, int type, int SelectedOption);
int UIExecMenu(struct UIMenu *FirstMenu, short int SelectedItem, struct UIMenu **CurrentMenu, int (*callback)(struct UIMenu *menu, unsigned short int frame, int selection, u32 padstatus));
