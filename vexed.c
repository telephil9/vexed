#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include <ctype.h>
#include "a.h"

enum
{
	Emouse,
	Eresize,
	Ekeyboard,
};

enum
{
	Padding = 4,
	Spacing = 8,
	Scrollwidth = 12,
};

const char	*filename;
int modified;
Buffer buf;
int blines;
Mousectl	*mctl;
Keyboardctl	*kctl;
int sw;
int scrollsize;
Rectangle scrollr;
Rectangle viewr;
Rectangle statusr;
int nlines;
int offset;
int sel = 0;

int
selvisible(void)
{
	int l;

	l = sel/16;
	return offset <= l && l < offset+nlines;
}

int
ensureselvisible(void)
{
	if(selvisible())
		return 0;
	offset = sel/16;
	return 1;
}

void
drawline(int y, int base)
{
	int index, i, n;
	char b[8] = {0}, *s;
	Point p;
	Point p2;
	
	index = base + offset*16;
	if(index >= buf.count)
		return;
	p = Pt(viewr.min.x, y);
	if(index/16 == sel/16){
		n = snprint(b, sizeof b, "%06X", sel);
		p = stringnbg(screen, p, cols[BACK], ZP, font, b, n, cols[ADDR], ZP);
	}else{
		n = snprint(b, sizeof b, "%06X", index);
		p = stringn(screen, p, cols[ADDR], ZP, font, b, n);
	}
	p.x += 2*Spacing;
	p2 = addpt(p, Pt(16*3*sw - sw + 2*Spacing, 0));
	for(i = 0; i < 16; i++) {
		if(index + i >= buf.count)
			break;
		n = snprint(b, sizeof b, "%02X", buf.data[index + i]);
		s = isprint(buf.data[index + i]) ? (char*)&buf.data[index + i] : ".";
		if(index + i == sel){
			p = stringnbg(screen, p, cols[BACK], ZP, font, b, n, cols[HEX], ZP);
			p2 = stringnbg(screen, p2, cols[BACK], ZP, font, s, 1, cols[ASCII], ZP);
		}else{
			p = stringn(screen, p, cols[HEX], ZP, font, b, n);
			p2 = stringn(screen, p2, cols[ASCII], ZP, font, s, 1);
		}
		p = stringn(screen, p, cols[BACK], ZP, font, " ", 1);
	}
}

void
redraw(void)
{
	int i, h, y, ye;
	Rectangle scrposr;
	Point p;
	char b[16] = {0};

	draw(screen, screen->r, cols[BACK], nil, ZP);
	draw(screen, scrollr, cols[SCROLL], nil, ZP);
	border(screen, scrollr, 0, cols[HEX], ZP);
	if(blines > 0){
		h = ((double)nlines / blines) * Dy(scrollr);
		y = ((double)offset / blines) * Dy(scrollr);
		ye = scrollr.min.y + y + h - 1;
		if(ye >= scrollr.max.y)
			ye = scrollr.max.y - 1;
		scrposr = Rect(scrollr.min.x + 1, scrollr.min.y + y + 1, scrollr.max.x - 1, ye);
	}else
		scrposr = insetrect(scrollr, -1);
	draw(screen, scrposr, cols[BACK], nil, ZP);
	for(i = 0; i < nlines; i++)
		drawline(viewr.min.y + i * font->height, i*16);
	p = string(screen, Pt(statusr.min.x + Padding, statusr.min.y), cols[HEX], ZP, font, filename);
	if(modified)
		string(screen, p, cols[HEX], ZP, font, " (modified)");
	snprint(b, sizeof b, "%d%%", (int)((100.0 * sel) / buf.count + 0.5));
	y = statusr.max.x - stringwidth(font, b) - Padding;
	string(screen, Pt(y, statusr.min.y), cols[HEX], ZP, font, b);
	flushimage(display, 1);
}

int
scroll(int lines)
{
	if(blines <= nlines)
		return 0;
	if(lines < 0 && offset == 0)
		return 0;
	if(lines > 0 && offset + nlines >= blines){
		return 0;
	}
	offset += lines;
	if(offset < 0)
		offset = 0;
	if(offset + blines%nlines >= blines)
		offset = blines - blines%nlines;
	sel += lines * 16;
	if(sel < 0)
		sel = 0;
	else if(sel >= buf.count)
		sel = buf.count - 1;
	redraw();
	return 1;
}

int
indexat(Point p)
{
	int row, col, index;

	row = (p.y - viewr.min.y) / font->height;
	col = ((p.x - viewr.min.x) / sw - 8);
	if(col < 0 || col > 16*3*sw)
		return -1;
	index = col/3 + row*16 + offset*16;
	if(index >= buf.count)
		return -1;
	return index;
}

void
emouse(Mouse *m)
{
	int n;

	if(ptinrect(m->xy, scrollr)){
		if(m->buttons == 1){
			n = (m->xy.y - scrollr.min.y) / font->height;
			scroll(-n);
		}else if(m->buttons == 2){
			n = (m->xy.y - scrollr.min.y) * blines / Dy(scrollr);
			offset = n;
			sel = offset*16;
			redraw();
		}else if(m->buttons == 4){
			n = (m->xy.y - scrollr.min.y) / font->height;
			scroll(n);
		}
	} else if(ptinrect(m->xy, viewr)){
		if(m->buttons == 1){
			n = indexat(m->xy);
			if(n >= 0){
				sel = n;
				redraw();
			}
		}else if(m->buttons == 8){
			scroll(-scrollsize);
		}else if(m->buttons == 16){
			scroll(scrollsize);
		}
	}
}

void
eresize(void)
{
	int w, x;

	statusr = Rect(screen->r.min.x + Padding, screen->r.max.y - Padding - font->height, screen->r.max.x - Padding, screen->r.max.y - Padding);
	scrollr = Rect(screen->r.min.x + Padding, screen->r.min.y + Padding, screen->r.min.x + Padding + Scrollwidth, statusr.min.y - Padding);
	sw = stringwidth(font, " ");
	w = Padding + 6*sw + 2*Spacing + 16*3*sw-sw + 2*Spacing + 16*sw + Padding;
	x = scrollr.max.x + Padding;
	viewr = Rect(x, screen->r.min.y + Padding, x + w, statusr.min.y - Padding);
	nlines = Dy(viewr) / font->height;
	scrollsize = mousescrollsize(nlines);
	redraw();
	flushimage(display, 1);
}

int
hexval(Rune k)
{
	int v;

	if(isdigit(k))
		v = k - '0';
	else
		v = 10 + tolower(k) - 'a';
	return v;
}

void
ekeyboard(Rune k)
{
	static long lastk = 0;
	static int lastv;
	long e;

	e = time(nil);
	switch(k){
	case 'q':
	case Kdel:
		threadexitsall(nil);
		break;
	case Kpgup:
		scroll(-nlines);
		break;
	case Kpgdown:
		scroll(nlines);
		break;
	case Kleft:
		if(sel > 0){
			sel--;
			ensureselvisible();
			redraw();
		}
		break;
	case Kright:
		if(sel < (buf.count - 1)){
			sel++;
			ensureselvisible();
			redraw();
		}
		break;
	case Kup:
		if(sel >= 16){
			sel -= 16;
			ensureselvisible();
			redraw();
		}
		break;
	case Kdown:
		if(sel < (buf.count - 16)){
			sel += 16;
			ensureselvisible();
			redraw();
		}
		break;
	case Khome:
		if(sel != 0){
			sel = 0;
			ensureselvisible();
			redraw();
		}
		break;
	case Kend:
		if(sel != buf.count - 1){
			sel = buf.count - 1;
			if(!selvisible())
				offset = blines - blines%nlines;
			redraw();
		}
		break;
	default:
		if(isxdigit(k)){
			if(e - lastk < 2 && lastv > 0) {
				buf.data[sel] = lastv*16 + hexval(k);
				lastv = -1;
			}else{
				lastv = hexval(k);
				buf.data[sel] = lastv;
			}
			modified = 1;
			redraw();
		}else{
			lastv = -1;
		}
	}
	lastk = e;
}

void
usage(void)
{
	fprint(2, "usage: %s [-b] <filename>\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char *argv[])
{
	Mouse m;
	Rune k;
	Alt alts[] = {
		{ nil, &m,  CHANRCV },
		{ nil, nil, CHANRCV },
		{ nil, &k,  CHANRCV },
		{ nil, nil, CHANEND },
	};
	int reverse;

	reverse = 0;
	ARGBEGIN{
	case 'b':
		reverse = 1;
		break;
	default:
		usage();
	}ARGEND;
	if(*argv == nil)
		usage();
	filename = *argv;
	modified = 0;
	if(readfile(&buf, filename) < 0)
		sysfatal("readfile: %r");
	blines = buf.count/16;
	if(initdraw(nil, nil, argv0) < 0)
		sysfatal("initdraw: %r");
	display->locking = 0;
	if((mctl = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
	if((kctl = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");
	initcols(reverse);
	alts[Emouse].c = mctl->c;
	alts[Eresize].c = mctl->resizec;
	alts[Ekeyboard].c = kctl->c;
	eresize();
	for(;;){
		switch(alt(alts)){
		case Emouse:
			emouse(&m);
			break;
		case Eresize:
			if(getwindow(display, Refnone) < 0)
				sysfatal("getwindow: %r");
			eresize();
			break;
		case Ekeyboard:
			ekeyboard(k);
			break;
		}
	}
}

