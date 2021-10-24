#include <kernel.h>
#include <libhdd.h>
#include <libmc.h>
#include <libpad.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <fileXio_rpc.h>
#include <osd_config.h>
#include <timer.h>
#include <limits.h>
#include <wchar.h>

#include <libgs.h>

#include "main.h"
#include "mctools_rpc.h"
#include "system.h"
#include "pad.h"
#include "graphics.h"
#include "font.h"
#include "UI.h"
#include "menu.h"

extern struct UIDrawGlobal UIDrawGlobal;
extern GS_IMAGE BackgroundTexture;
extern GS_IMAGE PadLayoutTexture;

extern int IsHDDUnitConnected;

enum MAIN_MENU_ID{
	MAIN_MENU_ID_BTN_EXIT	= 1,	//So that the cancel button case (1) will be aligned with this.
	MAIN_MENU_ID_BTN_INST,
	MAIN_MENU_ID_BTN_MI,
	MAIN_MENU_ID_BTN_UINST,
	MAIN_MENU_ID_BTN_DOWNGRADE_MI,
	MAIN_MENU_ID_BTN_FORMAT_MC,
	MAIN_MENU_ID_BTN_DUMP_MC,
	MAIN_MENU_ID_BTN_REST_MC,
	MAIN_MENU_ID_BTN_INST_CROSS_PSX,
	MAIN_MENU_ID_BTN_INST_FHDB,
	MAIN_MENU_ID_BTN_UINST_FHDB,
	MAIN_MENU_ID_BTN_FORMAT_HDD,
	MAIN_MENU_ID_BTN_ENABLE_MBR,
	MAIN_MENU_ID_DESCRIPTION,
	MAIN_MENU_ID_VERSION,
};

enum PRG_SCREEN_ID{
	PRG_SCREEN_ID_TITLE	= 1,
	PRG_SCREEN_ID_ETA_LBL,
	PRG_SCREEN_ID_ETA_HOURS,
	PRG_SCREEN_ID_ETA_MINS_SEP,
	PRG_SCREEN_ID_ETA_MINS,
	PRG_SCREEN_ID_ETA_SECS_SEP,
	PRG_SCREEN_ID_ETA_SECS,
	PRG_SCREEN_ID_RATE_LBL,
	PRG_SCREEN_ID_RATE,
	PRG_SCREEN_ID_RATE_UNIT,
	PRG_SCREEN_ID_PROGRESS
};

enum INSUFF_SPC_SCREEN_ID{
	INSUFF_SPC_SCREEN_ID_TITLE	= 1,
	INSUFF_SPC_SCREEN_ID_MESSAGE,
	INSUFF_SPC_SCREEN_ID_AVAIL_SPC,
	INSUFF_SPC_SCREEN_ID_REQD_SPC,
	INSUFF_SPC_SCREEN_ID_AVAIL_SPC_UNIT,
	INSUFF_SPC_SCREEN_ID_REQD_SPC_UNIT,
	INSUFF_SPC_SCREEN_ID_BTN_OK,
};

static struct UIMenuItem MainMenuItems[]={
	{MITEM_LABEL, 0, 0, 0, 0, 0, 0, SYS_UI_LBL_MENU_MAIN},
	{MITEM_SEPERATOR},
	{MITEM_BREAK},

	{MITEM_BUTTON, MAIN_MENU_ID_BTN_INST,		MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_INSTALL}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_MI,		MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_MI}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_UINST,		MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_UINSTALL}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_DOWNGRADE_MI,	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_UMI}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_EXIT,		MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_EXIT}, {MITEM_BREAK}, {MITEM_BREAK},

	{MITEM_STRING, MAIN_MENU_ID_DESCRIPTION,	MITEM_FLAG_POS_ABS|MITEM_FLAG_READONLY, 0, 0, 32, 370}, {MITEM_BREAK},
	{MITEM_STRING, MAIN_MENU_ID_VERSION,		MITEM_FLAG_POS_ABS|MITEM_FLAG_READONLY, 0, 0, 520, 420}, {MITEM_BREAK},

	{MITEM_TERMINATOR}
};

static struct UIMenuItem ExtraMenuItems[]={
	{MITEM_LABEL, 0, 0, 0, 0, 0, 0, SYS_UI_LBL_MENU_EXTRAS},
	{MITEM_SEPERATOR},
	{MITEM_BREAK},

	{MITEM_BUTTON, MAIN_MENU_ID_BTN_INST_CROSS_PSX,	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_INSTALL_CROSS_PSX}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_INST_FHDB,   	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_INSTALL_FHDB}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_UINST_FHDB,	    MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_UINSTALL_FHDB}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_FORMAT_HDD, 	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_FORMAT_HDD}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_ENABLE_MBR, 	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_ENABLE_MBR}, {MITEM_BREAK}, {MITEM_BREAK},

	{MITEM_STRING, MAIN_MENU_ID_DESCRIPTION,	MITEM_FLAG_POS_ABS|MITEM_FLAG_READONLY, 0, 0, 32, 370}, {MITEM_BREAK},
	{MITEM_STRING, MAIN_MENU_ID_VERSION,		MITEM_FLAG_POS_ABS|MITEM_FLAG_READONLY, 0, 0, 520, 420}, {MITEM_BREAK},

	{MITEM_TERMINATOR}
};

static struct UIMenuItem MCMenuItems[]={
	{MITEM_LABEL, 0, 0, 0, 0, 0, 0, SYS_UI_LBL_MENU_MC},
	{MITEM_SEPERATOR},
	{MITEM_BREAK},

	{MITEM_BUTTON, MAIN_MENU_ID_BTN_FORMAT_MC,	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_FORMAT_MC}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_DUMP_MC,	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_DUMP_MC}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_BUTTON, MAIN_MENU_ID_BTN_REST_MC,	MITEM_FLAG_POS_MID, 0, 24, 0, 0, SYS_UI_LBL_REST_MC}, {MITEM_BREAK}, {MITEM_BREAK},

	{MITEM_STRING, MAIN_MENU_ID_DESCRIPTION,	MITEM_FLAG_POS_ABS|MITEM_FLAG_READONLY, 0, 0, 32, 370}, {MITEM_BREAK},
	{MITEM_STRING, MAIN_MENU_ID_VERSION,		MITEM_FLAG_POS_ABS|MITEM_FLAG_READONLY, 0, 0, 520, 420}, {MITEM_BREAK},

	{MITEM_TERMINATOR}
};

static struct UIMenuItem ProgressScreenItems[]={
	{MITEM_LABEL, PRG_SCREEN_ID_TITLE},
	{MITEM_SEPERATOR},
	{MITEM_BREAK},

	{MITEM_LABEL, PRG_SCREEN_ID_ETA_LBL, 0, 0, 0, 0, 0, SYS_UI_LBL_ETA},	{MITEM_TAB}, {MITEM_VALUE, PRG_SCREEN_ID_ETA_HOURS, MITEM_FLAG_READONLY, MITEM_FORMAT_UDEC, 2}, {MITEM_COLON, PRG_SCREEN_ID_ETA_MINS_SEP}, {MITEM_VALUE, PRG_SCREEN_ID_ETA_MINS, MITEM_FLAG_READONLY, MITEM_FORMAT_UDEC, 2}, {MITEM_COLON, PRG_SCREEN_ID_ETA_SECS_SEP}, {MITEM_VALUE, PRG_SCREEN_ID_ETA_SECS, MITEM_FLAG_READONLY, MITEM_FORMAT_UDEC, 2}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_LABEL, PRG_SCREEN_ID_RATE_LBL, 0, 0, 0, 0, 0, SYS_UI_LBL_RATE},	{MITEM_TAB}, {MITEM_TAB}, {MITEM_VALUE, PRG_SCREEN_ID_RATE, MITEM_FLAG_READONLY}, {MITEM_LABEL, PRG_SCREEN_ID_RATE_UNIT, 0, 0, 0, 0, 0, SYS_UI_LBL_KBPS}, {MITEM_BREAK}, {MITEM_BREAK},
	{MITEM_PROGRESS, PRG_SCREEN_ID_PROGRESS, MITEM_FLAG_POS_ABS, 0, 0, 0, 280}, {MITEM_BREAK},

	{MITEM_TERMINATOR}
};

static struct UIMenuItem InsuffSpaceScreenItems[]={
	{MITEM_LABEL, INSUFF_SPC_SCREEN_ID_TITLE, 0, 0, 0, 0, 0, SYS_UI_LBL_ERROR},
	{MITEM_SEPERATOR},
	{MITEM_BREAK},

	{MITEM_STRING, INSUFF_SPC_SCREEN_ID_MESSAGE, MITEM_FLAG_READONLY}, {MITEM_BREAK}, {MITEM_BREAK},

	{MITEM_LABEL, 0, 0, 0, 0, 0, 0, SYS_UI_LBL_AVAILABLE_SPC}, {MITEM_TAB}, {MITEM_VALUE, INSUFF_SPC_SCREEN_ID_AVAIL_SPC, MITEM_FLAG_READONLY, MITEM_FORMAT_UDEC}, {MITEM_LABEL, INSUFF_SPC_SCREEN_ID_AVAIL_SPC_UNIT}, {MITEM_BREAK},
	{MITEM_LABEL, 0, 0, 0, 0, 0, 0, SYS_UI_LBL_REQUIRED_SPC}, {MITEM_TAB}, {MITEM_TAB}, {MITEM_VALUE, INSUFF_SPC_SCREEN_ID_REQD_SPC, MITEM_FLAG_READONLY, MITEM_FORMAT_UDEC}, {MITEM_LABEL, INSUFF_SPC_SCREEN_ID_REQD_SPC_UNIT}, {MITEM_BREAK},

	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},
	{MITEM_BREAK},

	{MITEM_BUTTON, INSUFF_SPC_SCREEN_ID_BTN_OK, MITEM_FLAG_POS_MID, 0, 16},

	{MITEM_TERMINATOR}
};

//Forward declarations
static struct UIMenu InstallMainMenu;
static struct UIMenu ExtraMenu;

static struct UIMenu MCMenu = {NULL, &ExtraMenu, MCMenuItems, {{BUTTON_TYPE_SYS_SELECT, SYS_UI_LBL_OK}, {BUTTON_TYPE_SYS_CANCEL, SYS_UI_LBL_EXIT}}};
static struct UIMenu ExtraMenu = {&MCMenu, &InstallMainMenu, ExtraMenuItems, {{BUTTON_TYPE_SYS_SELECT, SYS_UI_LBL_OK}, {BUTTON_TYPE_SYS_CANCEL, SYS_UI_LBL_EXIT}}};
static struct UIMenu InstallMainMenu = {&ExtraMenu, NULL, MainMenuItems, {{BUTTON_TYPE_SYS_SELECT, SYS_UI_LBL_OK}, {BUTTON_TYPE_SYS_CANCEL, SYS_UI_LBL_EXIT}}};

static struct UIMenu ProgressScreen = {NULL, NULL, ProgressScreenItems, {{BUTTON_TYPE_SYS_CANCEL, SYS_UI_LBL_CANCEL}, {-1, -1}}};
static struct UIMenu InsuffSpaceScreen = {NULL, NULL, InsuffSpaceScreenItems, {{BUTTON_TYPE_SYS_SELECT, SYS_UI_LBL_OK}, {-1, -1}}};

static unsigned char ProcessSpaceValue(unsigned long int space, unsigned int *ProcessedSpace)
{
	unsigned long int temp;
	unsigned char unit;

	unit=0;
	temp=space;
	while(temp >= 1024 && unit < 5)
	{
		unit++;
		temp /= 1024;
	}

	*ProcessedSpace = temp;
	return(SYS_UI_LBL_B + unit);
}

static void DrawMenuEntranceSlideInMenuAnimation(int SelectedOption)
{
	int i;
	GS_RGBAQ rgbaq;

	rgbaq.r = 0;
	rgbaq.g = 0;
	rgbaq.b = 0;
	rgbaq.q = 0;
	for(i=30; i>0; i--)
	{
		rgbaq.a = 0x80-(i*4);
		DrawSprite(&UIDrawGlobal,	0, 0,
						UIDrawGlobal.width, UIDrawGlobal.height,
						0, rgbaq);
		UIDrawMenu(&InstallMainMenu, i, UI_OFFSET_X + i * 6, UI_OFFSET_Y, SelectedOption);
		SyncFlipFB(&UIDrawGlobal);
	}
}

static void DrawMenuExitAnimation(void)
{
	int i;
	GS_RGBAQ rgbaq;

	rgbaq.r = 0;
	rgbaq.g = 0;
	rgbaq.b = 0;
	rgbaq.q = 0;
	for(i=30; i>0; i--)
	{
		rgbaq.a = 0x80-(i*4);
		DrawSprite(&UIDrawGlobal,	0, 0,
						UIDrawGlobal.width, UIDrawGlobal.height,
						0, rgbaq);
		SyncFlipFB(&UIDrawGlobal);
	}
}

static int CheckFormat(void)
{
	int status;

	status = HDDCheckStatus();
	switch(status)
	{
		case 1:		//Not formatted
			if(DisplayPromptMessage(SYS_UI_MSG_FORMAT_HDD, SYS_UI_LBL_CANCEL, SYS_UI_LBL_OK) == 2)
			{
				status = 0;

				if(hddFormat() != 0)
					DisplayErrorMessage(SYS_UI_MSG_FORMAT_HDD_FAILED);
			}
			break;
		case 0:		//Formatted
			break;
		case 2:		//Not a usable HDD
		case 3:		//No HDD connected
		default:	//Unknown errors
			//DisplayErrorMessage(SYS_UI_MSG_NO_HDD);
			break;
	}

	return status;
}

static int MainMenuUpdateCallback(struct UIMenu *menu, unsigned short int frame, int selection, u32 padstatus)
{
	if((padstatus != 0) || (frame == 0))
	{
		if(selection >= 0)
		{
			switch(menu->items[selection].id)
			{
				case MAIN_MENU_ID_BTN_INST:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_INST_FMCB));
					break;
				case MAIN_MENU_ID_BTN_MI:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_MI_FMCB));
					break;
				case MAIN_MENU_ID_BTN_UINST:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_UINST_FMCB));
					break;
				case MAIN_MENU_ID_BTN_DOWNGRADE_MI:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_DOWNGRADE_MI));
					break;
				case MAIN_MENU_ID_BTN_FORMAT_MC:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_FORMAT_MC));
					break;
				case MAIN_MENU_ID_BTN_DUMP_MC:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_DUMP_MC));
					break;
				case MAIN_MENU_ID_BTN_REST_MC:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_REST_MC));
					break;
				case MAIN_MENU_ID_BTN_INST_CROSS_PSX:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_INST_CROSS_PSX));
					break;
				case MAIN_MENU_ID_BTN_INST_FHDB:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_INST_FHDB));
					break;
				case MAIN_MENU_ID_BTN_UINST_FHDB:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_UINST_FHDB));
					break;
				case MAIN_MENU_ID_BTN_FORMAT_HDD:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_LBL_FORMAT_HDD));
					break;
				case MAIN_MENU_ID_BTN_ENABLE_MBR:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_LBL_ENABLE_MBR));
					break;
				case MAIN_MENU_ID_BTN_EXIT:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, GetUIString(SYS_UI_MSG_DSC_QUIT));
					break;
				default:
					UISetString(menu, MAIN_MENU_ID_DESCRIPTION, NULL);
			}
		} else
			UISetString(menu, MAIN_MENU_ID_DESCRIPTION, NULL);
	}

	return 0;
}

void MainMenu(void)
{
	int result;
	unsigned char done, event, McPort;
	short int option;
	struct McData McData[2];
	u32 PadStatus;
	unsigned int flags;
	struct UIMenu *CurrentMenu;

	done=0;
	memset(McData, 0, sizeof(McData));

	if(IsUnsupportedModel())
		DisplayWarningMessage(SYS_UI_MSG_ROM_UNSUPPORTED);

	UISetString(&InstallMainMenu, MAIN_MENU_ID_VERSION, "v"FMCB_INSTALLER_VERSION);
	UISetString(&ExtraMenu, MAIN_MENU_ID_VERSION, "v"FMCB_INSTALLER_VERSION);
	UISetString(&MCMenu, MAIN_MENU_ID_VERSION, "v"FMCB_INSTALLER_VERSION);
	UISetEnabled(&InstallMainMenu, MAIN_MENU_ID_BTN_MI, GetPs2Type() == PS2_SYSTEM_TYPE_PS2);
	UISetEnabled(&ExtraMenu, MAIN_MENU_ID_BTN_INST_CROSS_PSX, GetPs2Type() == PS2_SYSTEM_TYPE_PS2);
	UISetEnabled(&ExtraMenu, MAIN_MENU_ID_BTN_INST_FHDB, IsHDDUnitConnected);
	UISetEnabled(&ExtraMenu, MAIN_MENU_ID_BTN_UINST_FHDB, IsHDDUnitConnected);
	UISetEnabled(&ExtraMenu, MAIN_MENU_ID_BTN_FORMAT_HDD, IsHDDUnitConnected);
	UISetEnabled(&ExtraMenu, MAIN_MENU_ID_BTN_ENABLE_MBR, IsHDDUnitConnected);
	CurrentMenu = &InstallMainMenu;
	option = 0;

	DrawMenuEntranceSlideInMenuAnimation(0);

	/* Main loop */
	while(!done)
	{
		option = UIExecMenu(CurrentMenu, option, &CurrentMenu, &MainMenuUpdateCallback);

		McPort = GetNumMemcardsInserted(McData);

		switch(option)
		{
			case MAIN_MENU_ID_BTN_INST:
				event = EVENT_INSTALL;
				break;
			case MAIN_MENU_ID_BTN_MI:
				event = EVENT_MULTI_INSTALL;
				break;
			case MAIN_MENU_ID_BTN_UINST:
				event = EVENT_CLEANUP;
				break;
			case MAIN_MENU_ID_BTN_DOWNGRADE_MI:
				event = EVENT_CLEANUP_MULTI;
				break;
			case MAIN_MENU_ID_BTN_FORMAT_MC:
				event = EVENT_FORMAT;
				break;
			case MAIN_MENU_ID_BTN_DUMP_MC:
				event = EVENT_DUMP_MC;
				break;
			case MAIN_MENU_ID_BTN_REST_MC:
				event = EVENT_RESTORE_MC;
				break;
			case MAIN_MENU_ID_BTN_INST_FHDB:
				event = EVENT_INSTALL_FHDB;
				break;
			case MAIN_MENU_ID_BTN_UINST_FHDB:
				event = EVENT_CLEANUP_FHDB;
				break;
			case MAIN_MENU_ID_BTN_FORMAT_HDD:
				event = EVENT_FORMAT_HDD;
				break;
			case MAIN_MENU_ID_BTN_ENABLE_MBR:
				event = EVENT_ENABLE_HDD_BOOT;
				break;
			case MAIN_MENU_ID_BTN_INST_CROSS_PSX:
				event = EVENT_INSTALL_CROSS_PSX;
				break;
			default:	//MAIN_MENU_ID_BTN_EXIT
				event = EVENT_EXIT;
		}

		UITransition(CurrentMenu, UIMT_LEFT_OUT, option);

		switch(event)
		{
			case EVENT_MULTI_INSTALL:
				if(ShowMessageBox(SYS_UI_LBL_CANCEL, SYS_UI_LBL_OK, -1, -1, GetUIString(SYS_UI_MSG_MULTI_WARN), SYS_UI_LBL_WARNING) <= 1)
					break;
			case EVENT_INSTALL_CROSS_PSX:
			case EVENT_INSTALL:
				/* Install */
				if(McPort>1)
				{
					McPort = DisplayPromptMessage(SYS_UI_MSG_MULTIPLE_CARDS, SYS_UI_LBL_SLOT1, SYS_UI_LBL_SLOT2);
					if (McPort == 0)
						break;
					McPort--;
				} else if(McPort==0) {
					DisplayErrorMessage(SYS_UI_MSG_NO_CARDS);
					break;
				} else	/* Only one MC inserted. */
					McPort=(McData[0].Type==MC_TYPE_PS2)?0:1;

				if(DisplayPromptMessage(McPort==0?SYS_UI_MSG_INST_CFM_SLOT1:SYS_UI_MSG_INST_CFM_SLOT2, SYS_UI_LBL_NO, SYS_UI_LBL_YES) <= 1)
					break;

				flags=0;
				if(event==EVENT_INSTALL && GetPs2Type()==PS2_SYSTEM_TYPE_PS2)
				{
					result = ShowMessageBox(SYS_UI_LBL_INST_TYPE_NORMAL, SYS_UI_LBL_INST_TYPE_CRS_MDL, SYS_UI_LBL_INST_TYPE_CRS_REG, -1, GetUIString(SYS_UI_MSG_INST_PROMPT_INST_TYPE), SYS_UI_LBL_CONFIRM);
					if(result == 0)
						break;

					switch(result)
					{
						case 2:
							flags|=INSTALL_MODE_FLAG_CROSS_MODEL;
							break;
						case 3:
							flags|=INSTALL_MODE_FLAG_CROSS_REG;
							break;
					}
				}

				if(CheckPrerequisites(&McData[McPort], event)<0) break;

				if(HasOldFMCBConfigFile(McPort, 0))
				{
					result = DisplayPromptMessage(SYS_UI_MSG_CNF_FOUND, SYS_UI_LBL_YES, SYS_UI_LBL_NO);

					if(result == 1)		//"YES" selected.
						flags|=INSTALL_MODE_FLAG_SKIP_CNF;
					else if (result == 0)	//User pressed back.
						break;
				}

				if(event==EVENT_MULTI_INSTALL)
					flags|=INSTALL_MODE_FLAG_MULTI_INST;

				if(event == EVENT_INSTALL_CROSS_PSX)
					flags|=(INSTALL_MODE_FLAG_CROSS_PSX|INSTALL_MODE_FLAG_SKIP_CLEANUP);

				if((result=PerformInstallation(McPort, 0, flags))>=0)
				{
					if(event==EVENT_MULTI_INSTALL)
						McData[McPort].flags|=MC_FLAG_CARD_HAS_MULTI_INST;
				}

				if(result<0)
				{
					switch(-result)
					{
						case ENOENT:
							DisplayErrorMessage(SYS_UI_MSG_NO_ENT_ERROR);
							break;
						case (EIO|ERROR_SIDE_SRC):
							DisplayErrorMessage(SYS_UI_MSG_READ_INST_ERROR);
							break;
						case (EIO|ERROR_SIDE_DST):
							DisplayErrorMessage(SYS_UI_MSG_WRITE_INST_ERROR);
							break;
						case ENOMEM:
							DisplayErrorMessage(SYS_UI_MSG_NO_MEM_ERROR);
							break;
						case EEXTCACHEINITERR:
							DisplayErrorMessage(SYS_UI_MSG_CACHE_INIT_ERROR);
							break;
						case EEXTCRSLNKFAIL:
							DisplayErrorMessage(SYS_UI_MSG_CROSSLINK_FAIL);
							break;
						case EEXTMGSIGNERR:
							DisplayErrorMessage(SYS_UI_MSG_MG_BIND_FAIL);
							break;
					}

					DisplayErrorMessage(SYS_UI_MSG_INSTALL_FAILED);
				} else
					DisplayInfoMessage(SYS_UI_MSG_INSTALL_COMPLETE);

				break;
			case EVENT_CLEANUP:
			case EVENT_CLEANUP_MULTI:
				if(McPort>1)
				{
					McPort = DisplayPromptMessage(SYS_UI_MSG_MULTIPLE_CARDS, SYS_UI_LBL_SLOT1, SYS_UI_LBL_SLOT2);
					if (McPort == 0)
						break;
					McPort--;
				} else if(McPort==0) {
					DisplayErrorMessage(SYS_UI_MSG_NO_CARDS);
					break;
				} else	/* Only one MC inserted. */
					McPort=(McData[0].Type==MC_TYPE_PS2)?0:1;

				if(DisplayPromptMessage(McPort==0?SYS_UI_MSG_CLNUP_CFM_SLOT1:SYS_UI_MSG_CLNUP_CFM_SLOT2, SYS_UI_LBL_NO, SYS_UI_LBL_YES) <= 1)
					break;

				if(CheckPrerequisites(&McData[McPort], event)<0) break;

				DisplayFlashStatusUpdate(SYS_UI_MSG_UNINSTALLING_FMCB);

				if(event!=EVENT_CLEANUP_MULTI)
					result=CleanupTarget(McPort, 0);
				else {
					result=CleanupMultiInstallation(McPort, 0);
					if(result>=0) McData[McPort].flags&=~MC_FLAG_CARD_HAS_MULTI_INST;
				}

				if(result<0)
				{
					switch(-result)
					{
						case ENOMEM:
							DisplayErrorMessage(SYS_UI_MSG_NO_MEM_ERROR);
							break;
						case EEXTCACHEINITERR:
							DisplayErrorMessage(SYS_UI_MSG_CACHE_INIT_ERROR);
							break;
						case EEXTUNSUPPUNINST:
							DisplayErrorMessage(SYS_UI_MSG_UNSUPPORTED_UINST_FILE_ERROR);
							break;
					}

					DisplayErrorMessage(SYS_UI_MSG_CLEANUP_FAILED);
				} else
					DisplayInfoMessage(SYS_UI_MSG_CLEANUP_COMPLETE);
				break;
			case EVENT_FORMAT:
				/* Format Memory Card */
				if(McPort>1)
				{
					McPort = DisplayPromptMessage(SYS_UI_MSG_MULTIPLE_CARDS, SYS_UI_LBL_SLOT1, SYS_UI_LBL_SLOT2);
					if (McPort == 0)
						break;
					McPort--;
				} else if(McPort==0) {
					DisplayErrorMessage(SYS_UI_MSG_NO_CARDS);
					break;
				} else	/* Only one MC inserted. */
					McPort=(McData[0].Type==MC_TYPE_PS2)?0:1;

				if(DisplayPromptMessage(McPort==0?SYS_UI_MSG_FORMAT_CFM_SLOT1:SYS_UI_MSG_FORMAT_CFM_SLOT2, SYS_UI_LBL_NO, SYS_UI_LBL_YES) <= 1)
					break;

				DisplayFlashStatusUpdate(SYS_UI_MSG_FORMATTING_MC);

				if(CheckPrerequisites(&McData[McPort], event)<0) break;

				if((result=mcFormat(McPort, 0))>=0)
					mcSync(0, NULL, &result);

				if(result<0)
					DisplayErrorMessage(SYS_UI_MSG_FORMAT_ERROR);
				else {
					McData[McPort].flags=0;	/* Clear all flags once formatting has been completed. */
					DisplayInfoMessage(SYS_UI_MSG_FORMAT_COMPLETE);
				}
				break;
			case EVENT_DUMP_MC:
				if(McPort>1)
				{
					McPort = DisplayPromptMessage(SYS_UI_MSG_MULTIPLE_CARDS, SYS_UI_LBL_SLOT1, SYS_UI_LBL_SLOT2);
					if (McPort == 0)
						break;
					McPort--;
				} else if(McPort==0) {
					DisplayErrorMessage(SYS_UI_MSG_NO_CARDS);
					break;
				} else	/* Only one MC inserted. */
					McPort=(McData[0].Type==MC_TYPE_PS2)?0:1;

				if(DisplayPromptMessage(McPort==0?SYS_UI_MSG_DUMP_CFM_SLOT1:SYS_UI_MSG_DUMP_CFM_SLOT2, SYS_UI_LBL_NO, SYS_UI_LBL_YES) <= 1)
					break;

				if(CheckPrerequisites(&McData[McPort], event)<0) break;

				result=PerformMemoryCardDump(McPort, 0);

				if(result<0)
				{
					switch(-result)
					{
						case (EIO|ERROR_SIDE_SRC):
							DisplayErrorMessage(SYS_UI_MSG_READ_MC_ERROR);
							break;
						case (EIO|ERROR_SIDE_DST):
							DisplayErrorMessage(SYS_UI_MSG_WRITE_INST_ERROR);
							break;
						case ENOMEM:
							DisplayErrorMessage(SYS_UI_MSG_NO_MEM_ERROR);
							break;
					}

					DisplayErrorMessage(SYS_UI_MSG_DUMP_ERROR);
				}
				else if(result==0)
					DisplayInfoMessage(SYS_UI_MSG_DUMP_COMPLETE);
				break;
			case EVENT_RESTORE_MC:
				if(McPort>1)
				{
					McPort = DisplayPromptMessage(SYS_UI_MSG_MULTIPLE_CARDS, SYS_UI_LBL_SLOT1, SYS_UI_LBL_SLOT2);
					if (McPort == 0)
						break;
					McPort--;
				} else if(McPort==0) {
					DisplayErrorMessage(SYS_UI_MSG_NO_CARDS);
					break;
				} else	/* Only one MC inserted. */
					McPort=(McData[0].Type==MC_TYPE_PS2)?0:1;

				if(DisplayPromptMessage(McPort==0?SYS_UI_MSG_RESTORE_CFM_SLOT1:SYS_UI_MSG_RESTORE_CFM_SLOT2, SYS_UI_LBL_NO, SYS_UI_LBL_YES) <= 1)
					break;

				if(CheckPrerequisites(&McData[McPort], event)<0) break;

				result=PerformMemoryCardRestore(McPort, 0);

				if(result<0)
				{
					switch(-result)
					{
						case ENOENT:
							DisplayErrorMessage(SYS_UI_MSG_NO_ENT_ERROR);
							break;
						case (EIO|ERROR_SIDE_SRC):
							DisplayErrorMessage(SYS_UI_MSG_READ_INST_ERROR);
							break;
						case (EIO|ERROR_SIDE_DST):
							DisplayErrorMessage(SYS_UI_MSG_WRITE_MC_ERROR);
							break;
						case ENOMEM:
							DisplayErrorMessage(SYS_UI_MSG_NO_MEM_ERROR);
							break;
					}

					DisplayErrorMessage(SYS_UI_MSG_RESTORE_ERROR);
				}
				else if(result==0)
					DisplayInfoMessage(SYS_UI_MSG_RESTORE_COMPLETE);
				break;
			case EVENT_INSTALL_FHDB:
				if(HDDCheckSMARTStatus())
				{
					DisplayErrorMessage(SYS_UI_MSG_HDD_SMART_FAILED);
					break;
				}

				if(CheckFormat())
					break;

				if(HDDCheckSectorErrorStatus() || HDDCheckPartErrorStatus())
				{
					DisplayErrorMessage(SYS_UI_MSG_HDD_CORRUPTED);
					break;
				}

				if(DisplayPromptMessage(SYS_UI_MSG_INST_CFM_HDD, SYS_UI_LBL_NO, SYS_UI_LBL_YES) <= 1)
					break;
				flags=0;

				if(HasOldFMCBConfigFileOnHDD())
				{
					result = DisplayPromptMessage(SYS_UI_MSG_CNF_HDD_FOUND, SYS_UI_LBL_YES, SYS_UI_LBL_NO);

					if(result == 1)		//"YES" selected.
						flags|=INSTALL_MODE_FLAG_SKIP_CNF;
					else if (result == 0)	//User pressed back.
						break;
				}

				result=PerformHDDInstallation(flags);

				if(result<0)
				{
					switch(-result)
					{
						case ENOENT:
							DisplayErrorMessage(SYS_UI_MSG_NO_ENT_ERROR);
							break;
						case (EIO|ERROR_SIDE_SRC):
							DisplayErrorMessage(SYS_UI_MSG_READ_INST_ERROR);
							break;
						case (EIO|ERROR_SIDE_DST):
							DisplayErrorMessage(SYS_UI_MSG_WRITE_HDD_ERROR);
							break;
						case ENOMEM:
							DisplayErrorMessage(SYS_UI_MSG_NO_MEM_ERROR);
							break;
					}

					DisplayErrorMessage(SYS_UI_MSG_INSTALL_FAILED);
				} else
					DisplayInfoMessage(SYS_UI_MSG_INSTALL_COMPLETE);
				break;
			case EVENT_CLEANUP_FHDB:
				if(DisplayPromptMessage(SYS_UI_MSG_CLNUP_CFM_HDD, SYS_UI_LBL_NO, SYS_UI_LBL_YES) <= 1)
					break;

				DisplayFlashStatusUpdate(SYS_UI_MSG_UNINSTALLING_FHDB);

				result=CleanupHDDTarget();

				if(result<0)
				{
					switch(-result)
					{
						case ENOMEM:
							DisplayErrorMessage(SYS_UI_MSG_NO_MEM_ERROR);
							break;
					}

					DisplayErrorMessage(SYS_UI_MSG_CLEANUP_FAILED);
				}
				else{
					DisplayInfoMessage(SYS_UI_MSG_CLEANUP_COMPLETE);
				}
				break;
			case EVENT_FORMAT_HDD:
					if(DisplayPromptMessage(SYS_UI_MSG_FORMAT_HDD, SYS_UI_LBL_CANCEL, SYS_UI_LBL_OK) == 2)
					{
					if(hddFormat() != 0)
					DisplayErrorMessage(SYS_UI_MSG_FORMAT_HDD_FAILED);
					}
				break;
			case EVENT_ENABLE_HDD_BOOT:
				if((result=EnableHDDBooting())<0){
					DEBUG_PRINTF("EnableHDDBooting() failed: %d\n", result);
					//result|=ERROR_SIDE_DST;
					DisplayErrorMessage(SYS_UI_MSG_HDD_ENABLE_FAIL)
				}
				break;
			case EVENT_EXIT:
				if(DisplayPromptMessage(SYS_UI_MSG_QUIT, SYS_UI_LBL_CANCEL, SYS_UI_LBL_OK) == 2)
					done=1;
				break;
		}

		if(!done) UITransition(CurrentMenu, UIMT_LEFT_IN, option);
	}

	DrawMenuExitAnimation();
}

static unsigned char ETADisplayed = 1;

void InitProgressScreen(int label)
{
	int ETAIndicator, RateIndicator;

	UISetLabel(&ProgressScreen, PRG_SCREEN_ID_TITLE, label);
	ETADisplayed = 1;

	switch(label)
	{
		case SYS_UI_LBL_DUMPING_MC:
		case SYS_UI_LBL_RESTORING_MC:
			ETAIndicator = 1;
			RateIndicator = 1;
			ProgressScreen.hints[0].button = BUTTON_TYPE_SYS_CANCEL;
			break;
		default:
			ETAIndicator = 0;
			RateIndicator = 0;
			ProgressScreen.hints[0].button = -1;
	}

	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_ETA_LBL, ETAIndicator);
	UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, MITEM_VALUE);
	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, ETAIndicator);
	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS_SEP, ETAIndicator);
	UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, MITEM_VALUE);
	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, ETAIndicator);
	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS_SEP, ETAIndicator);
	UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, MITEM_VALUE);
	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, ETAIndicator);

	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_RATE_LBL, RateIndicator);
	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_RATE, RateIndicator);
	UISetVisible(&ProgressScreen, PRG_SCREEN_ID_RATE_UNIT, RateIndicator);
}

void DrawFileCopyProgressScreen(float PercentageComplete)
{
	UISetValue(&ProgressScreen, PRG_SCREEN_ID_PROGRESS, (int)(PercentageComplete * 100));
	UIDrawMenu(&ProgressScreen, 0, 0, 0, -1);

	SyncFlipFB(&UIDrawGlobal);
}

void DrawMemoryCardDumpingProgressScreen(float PercentageComplete, unsigned int rate, unsigned int SecondsRemaining)
{
	unsigned int HoursRemaining;
	unsigned char MinutesRemaining;

	UISetValue(&ProgressScreen, PRG_SCREEN_ID_PROGRESS, (int)(PercentageComplete * 100));
	UISetValue(&ProgressScreen, PRG_SCREEN_ID_RATE, rate);
	if(SecondsRemaining < UINT_MAX)
	{
		if(!ETADisplayed)
		{
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, MITEM_VALUE);
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, MITEM_VALUE);
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, MITEM_VALUE);
			ETADisplayed = 1;
		}

		HoursRemaining=SecondsRemaining/3600;
		MinutesRemaining=(SecondsRemaining-HoursRemaining*3600)/60;
		UISetValue(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, HoursRemaining);
		UISetValue(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, MinutesRemaining);
		UISetValue(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, SecondsRemaining-HoursRemaining*3600-MinutesRemaining*60);
	}
	else{
		if(ETADisplayed)
		{
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, MITEM_STRING);
			UISetString(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, "--");
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, MITEM_STRING);
			UISetString(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, "--");
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, MITEM_STRING);
			UISetString(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, "--");
			ETADisplayed = 0;
		}
	}

	UIDrawMenu(&ProgressScreen, 0, 0, 0, -1);

	SyncFlipFB(&UIDrawGlobal);
}

void DrawMemoryCardRestoreProgressScreen(float PercentageComplete, unsigned int rate, unsigned int SecondsRemaining)
{
	unsigned int HoursRemaining;
	unsigned char MinutesRemaining;

	UISetValue(&ProgressScreen, PRG_SCREEN_ID_PROGRESS, (int)(PercentageComplete * 100));
	UISetValue(&ProgressScreen, PRG_SCREEN_ID_RATE, rate);
	if(SecondsRemaining < UINT_MAX)
	{
		if(!ETADisplayed)
		{
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, MITEM_VALUE);
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, MITEM_VALUE);
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, MITEM_VALUE);
			ETADisplayed = 1;
		}

		HoursRemaining=SecondsRemaining/3600;
		MinutesRemaining=(SecondsRemaining-HoursRemaining*3600)/60;
		UISetValue(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, HoursRemaining);
		UISetValue(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, MinutesRemaining);
		UISetValue(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, SecondsRemaining-HoursRemaining*3600-MinutesRemaining*60);
	}
	else{
		if(ETADisplayed)
		{
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, MITEM_STRING);
			UISetString(&ProgressScreen, PRG_SCREEN_ID_ETA_HOURS, "--");
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, MITEM_STRING);
			UISetString(&ProgressScreen, PRG_SCREEN_ID_ETA_MINS, "--");
			UISetType(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, MITEM_STRING);
			UISetString(&ProgressScreen, PRG_SCREEN_ID_ETA_SECS, "--");
			ETADisplayed = 0;
		}
	}

	UIDrawMenu(&ProgressScreen, 0, 0, 0, -1);

	SyncFlipFB(&UIDrawGlobal);
}

void DisplayOutOfSpaceMessage(unsigned int AvailableSpace, unsigned int RequiredSpace)
{
	unsigned int RequiredSpaceProcessed, AvailableSpaceProcessed;
	unsigned char RequiredSpaceDisplayUnit, AvailableSpaceDisplayUnit;

	RequiredSpaceDisplayUnit=ProcessSpaceValue(RequiredSpace, &RequiredSpaceProcessed);
	AvailableSpaceDisplayUnit=ProcessSpaceValue(AvailableSpace, &AvailableSpaceProcessed);

	UISetString(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_MESSAGE, GetUIString(SYS_UI_MSG_INSUF_FREE_SPC));
	UISetLabel(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_AVAIL_SPC_UNIT, AvailableSpaceDisplayUnit);
	UISetLabel(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_REQD_SPC_UNIT, RequiredSpaceDisplayUnit);
	UISetValue(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_AVAIL_SPC, AvailableSpaceProcessed);
	UISetValue(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_REQD_SPC, RequiredSpaceProcessed);

	UIExecMenu(&InsuffSpaceScreen, 0, NULL, NULL);
}

void DisplayOutOfSpaceMessageHDD_APPS(unsigned int AvailableSpace, unsigned int RequiredSpace)
{
	unsigned int RequiredSpaceProcessed, AvailableSpaceProcessed;
	unsigned char RequiredSpaceDisplayUnit, AvailableSpaceDisplayUnit;

	RequiredSpaceDisplayUnit=ProcessSpaceValue(RequiredSpace, &RequiredSpaceProcessed);
	AvailableSpaceDisplayUnit=ProcessSpaceValue(AvailableSpace, &AvailableSpaceProcessed);
	UISetString(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_MESSAGE, GetUIString(SYS_UI_MSG_INSUF_FREE_SPC_HDD_APPS));
	UISetLabel(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_AVAIL_SPC_UNIT, AvailableSpaceDisplayUnit);
	UISetLabel(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_REQD_SPC_UNIT, RequiredSpaceDisplayUnit);
	UISetValue(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_AVAIL_SPC, AvailableSpaceProcessed);
	UISetValue(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_REQD_SPC, RequiredSpaceProcessed);

	UIExecMenu(&InsuffSpaceScreen, 0, NULL, NULL);
}

void DisplayOutOfSpaceMessageHDD(unsigned int AvailableSpace, unsigned int RequiredSpace)
{
	unsigned int RequiredSpaceProcessed, AvailableSpaceProcessed;
	unsigned char RequiredSpaceDisplayUnit, AvailableSpaceDisplayUnit;

	RequiredSpaceDisplayUnit=ProcessSpaceValue(RequiredSpace, &RequiredSpaceProcessed);
	AvailableSpaceDisplayUnit=ProcessSpaceValue(AvailableSpace, &AvailableSpaceProcessed);

	//The base unit is MB.
	RequiredSpaceDisplayUnit += 2;
	AvailableSpaceDisplayUnit += 2;

	UISetString(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_MESSAGE, GetUIString(SYS_UI_MSG_INSUF_FREE_SPC_HDD));
	UISetLabel(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_AVAIL_SPC_UNIT, AvailableSpaceDisplayUnit);
	UISetLabel(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_REQD_SPC_UNIT, RequiredSpaceDisplayUnit);
	UISetValue(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_AVAIL_SPC, AvailableSpaceProcessed);
	UISetValue(&InsuffSpaceScreen, INSUFF_SPC_SCREEN_ID_REQD_SPC, RequiredSpaceProcessed);

	UIExecMenu(&InsuffSpaceScreen, 0, NULL, NULL);
}

void RedrawLoadingScreen(unsigned int frame)
{
	short int xRel, x, y;
	int NumDots;
	GS_RGBAQ rgbaq;

	SyncFlipFB(&UIDrawGlobal);

	NumDots=frame%240/60;

	DrawBackground(&UIDrawGlobal, &BackgroundTexture);
	FontPrintf(&UIDrawGlobal, 10, 10, 0, 2.5f, GS_WHITE_FONT, "FMCBInstaller v"FMCB_INSTALLER_VERSION);

	x = 420;
	y = 380;
	FontPrintfWithFeedback(&UIDrawGlobal, x, y, 0, 1.8f, GS_WHITE_FONT, "Loading ", &xRel, NULL);
	x += xRel;
	switch(NumDots)
	{
		case 1:
			FontPrintf(&UIDrawGlobal, x, y, 0, 1.8f, GS_WHITE_FONT, ".");
			break;
		case 2:
			FontPrintf(&UIDrawGlobal, x, y, 0, 1.8f, GS_WHITE_FONT, ". .");
			break;
		case 3:
			FontPrintf(&UIDrawGlobal, x, y, 0, 1.8f, GS_WHITE_FONT, ". . .");
			break;
	}

	if(frame < 60)
	{	//Fade in
		rgbaq.r = 0;
		rgbaq.g = 0;
		rgbaq.b = 0;
		rgbaq.q = 0;
		rgbaq.a = 0x80-(frame*2);
		DrawSprite(&UIDrawGlobal,	0, 0,
						UIDrawGlobal.width, UIDrawGlobal.height,
						0, rgbaq);
	}
}
