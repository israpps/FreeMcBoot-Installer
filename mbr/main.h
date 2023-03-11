//#define DEBUG_TTY_FEEDBACK /* Comment out to disable debugging messages */

#ifdef DEBUG_TTY_FEEDBACK
	#define DEBUG_PRINTF(args...) printf(args)
#else
	#define DEBUG_PRINTF(args...)
#endif
