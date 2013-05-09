// File: ConsoleDefintions.h
// Title: Definitions for dimensions, menu results, and key presses
// Author: Jeff Benson
// Date: 8/2/11

#ifndef _PRAGMA_ONCE_CONSOLEDEFS_H_
#define _PRAGMA_ONCE_CONSOLEDEFS_H_
#define MAXSCREENX			(80)						// Standard sized window dimensions
#define MAXSCREENY			(25)
#define SCREEN_BUFFER_SIZE	(MAXSCREENX * MAXSCREENY)
#define BADMENU				(60000)
#define USERESC				(60001)
#define UP_KEY				(72)
#define DOWN_KEY			(80)
#define RETURN				(13)
#define ESCAPE				(27)
#define KB_EXTENDED			(224)						// Returned from kbhit if an extended key is pressed
#endif