//#define DEBUG_TTY_FEEDBACK /* Comment out to disable debugging messages */

#ifdef DEBUG_TTY_FEEDBACK
	#define DEBUG_PRINTF(args...) printf(args)
#elif defined(DEBUG_EE_SIO)
	void sio_printf(const char *fmt, ...)
	{
	    va_list args;
	    va_start(args, fmt);

	    char str[256];
	    vsnprintf(str, sizeof(str), fmt, args);
	    sio_puts(str);
	}
	#define DEBUG_PRINTF(args...) sio_printf(args)
#else
	#define DEBUG_PRINTF(args...)
#endif

#ifndef FMCB_INSTALLER_VERSION
#define FMCB_INSTALLER_VERSION "1.001-MOD"
#endif//FMCB_INSTALLER_VERSION

/* The number of files and folders to crosslink (For multi-regional and cross-model installations). */
#define NUM_CROSSLINKED_FILES	8

/* Event numbers */
enum MainMenuEvents{
	EVENT_INSTALL=0,
	EVENT_MULTI_INSTALL,
	EVENT_CLEANUP,
	EVENT_CLEANUP_MULTI,
	EVENT_FORMAT,
	EVENT_DUMP_MC,
	EVENT_RESTORE_MC,
	EVENT_INSTALL_FHDB,
	EVENT_CLEANUP_FHDB,
	EVENT_INSTALL_CROSS_PSX,
	EVENT_FORMAT_HDD,
	EVENT_EXIT,
	EVENT_OPTION_COUNT	/* The number of valid events. */
};

/* Memory card state flags */
#define MC_FLAG_CARD_HAS_MULTI_INST	0x01

struct McData{
	int SpaceFree;
	int Type;
	int Format;
	unsigned char flags;
};

/* Operation mode prerequisite check parameters */
#define CHECK_MULTI_INSTALL		0x01
#define CHECK_MUST_HAVE_MULTI_INSTALL	0x02	/* The opposite of CHECK_MULTI_INSTALL */
#define CHECK_FREE_SPACE		0x04
