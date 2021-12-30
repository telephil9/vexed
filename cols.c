#include <u.h>
#include <libc.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "a.h"

void
initcols(int reverse)
{
	if(reverse){
		cols[BACK]   = display->black;
		cols[ADDR]   = allocimage(display, Rect(0,0,1,1), screen->chan, 1, DPurpleblue);
		cols[HEX]    = display->white;
		cols[ASCII]  = allocimage(display, Rect(0,0,1,1), screen->chan, 1, DPurpleblue);
		cols[SCROLL] = allocimage(display, Rect(0,0,1,1), screen->chan, 1, 0x999999FF^reverse);
	}else{
		cols[BACK]   = display->white;
		cols[ADDR]   = allocimage(display, Rect(0,0,1,1), screen->chan, 1, DGreygreen);
		cols[HEX]    = display->black;
		cols[ASCII]  = allocimage(display, Rect(0,0,1,1), screen->chan, 1, DGreygreen);
		cols[SCROLL] = allocimage(display, Rect(0,0,1,1), screen->chan, 1, 0x999999FF);
	}
}

