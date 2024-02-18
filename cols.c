#include <u.h>
#include <libc.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "a.h"

Image*
ecolor(ulong n)
{
	Image *i;

	i = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, n);
	if(i == nil)
		sysfatal("allocimage: %r");
	return i;
}

void
initcols(int reverse)
{
	if(reverse){
		cols[BACK]   = display->black;
		cols[ADDR]   = ecolor(DPurpleblue);
		cols[HEX]    = display->white;
		cols[ASCII]  = ecolor(DPurpleblue);
		cols[HHEX]	 = display->black;
		cols[DHEX]	 = ecolor(0xAAAAAAFF^reverse);
		cols[HIGH]	 = ecolor(DPurpleblue);
		cols[SCROLL] = ecolor(0x999999FF^reverse);
	}else{
		cols[BACK]   = display->white;
		cols[ADDR]   = ecolor(DGreygreen);
		cols[HEX]    = display->black;
		cols[ASCII]  = ecolor(DGreygreen);
		cols[HHEX]	 = display->black;
		cols[DHEX]	 = ecolor(0xAAAAAAFF);
		cols[HIGH]   = ecolor(0xCCCCCCFF);
		cols[SCROLL] = ecolor(0x999999FF);
	}
}

