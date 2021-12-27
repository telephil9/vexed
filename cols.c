#include <u.h>
#include <libc.h>
#include <draw.h>
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
/*
	bg  = allocimage(display, Rect(0,0,1,1), screen->chan, 1, 0x282828ff);
	fg  = allocimage(display, Rect(0,0,1,1), screen->chan, 1, 0xebdbb2ff);
	ofg = allocimage(display, Rect(0,0,1,1), screen->chan, 1, 0x83a598ff);
	scrlbg = allocimage(display, Rect(0,0,1,1), screen->chan, 1, 0x504945ff); 
*/
}

