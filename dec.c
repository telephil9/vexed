#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include "a.h"

enum { Padding = 12, };

u8int
u8(Buffer *buf, int index)
{
	return (u8int)buf->data[index];
}

s8int
s8(Buffer *buf, int index)
{
	return (s8int)buf->data[index];
}

u16int
u16(Buffer *buf, int index)
{
	int i;
	u16int r;

	r = 0;
	for(i = 0; i < 2 && index + i < buf->count; i++)
		r += ((u16int)buf->data[index+i]) << 8*i;
	return r;
}

s16int
s16(Buffer *buf, int index)
{
	int i;
	s16int r;

	r = 0;
	for(i = 0; i < 2 && index + i < buf->count; i++)
		r += ((s16int)buf->data[index+i]) << 8*i;
	return r;
}

u32int
u32(Buffer *buf, int index)
{
	int i;
	u32int r;

	r = 0;
	for(i = 0; i < 4 && index + i < buf->count; i++)
		r += ((u32int)buf->data[index+i]) << 8*i;
	return r;
}

s32int
s32(Buffer *buf, int index)
{
	int i;
	s32int r;

	r = 0;
	for(i = 0; i < 4 && index + i < buf->count; i++)
		r += ((s32int)buf->data[index+i]) << 8*i;
	return r;
}

u64int
u64(Buffer *buf, int index)
{
	int i;
	u64int r;

	r = 0;
	for(i = 0; i < 8 && index + i < buf->count; i++)
		r += ((u64int)buf->data[index+i]) << 8*i;
	return r;
}

s64int
s64(Buffer *buf, int index)
{
	int i;
	s64int r;

	r = 0;
	for(i = 0; i < 8 && index + i < buf->count; i++)
		r += ((s64int)buf->data[index+i]) << 8*i;
	return r;
}

float
f32(Buffer *buf, int index)
{
	union { uchar b[4]; float f; } v;
	int i;

	for(i = 0; i < 4 && index + i < buf->count; i++)
		v.b[i] = buf->data[index + i];
	return v.f;
}

double
f64(Buffer *buf, int index)
{
	union { uchar b[8]; double d; } v;
	int i;

	for(i = 0; i < 8 && index + i < buf->count; i++)
		v.b[i] = buf->data[index + i];
	return v.d;
}

void
dec(Buffer *buf, int index, Image *b, Point o, Point p, Image *fg)
{
	char tmp[64] = {0};
	int n;

	p = string(b, p, fg, ZP, font, "  in: ");
	for(n = 0; n < 8 && index + n < buf->count; n++){
		snprint(tmp, sizeof tmp, "%02X ", buf->data[index + n]);
		p = string(b, p, fg, ZP, font, tmp);
	}
	p = addpt(o, Pt(Padding, 2*Padding + font->height));
	snprint(tmp, sizeof tmp, "%5s %-20ud %5s %-20d", "u8:", u8(buf, index), "s8:", s8(buf, index));
	string(b, p, fg, ZP, font, tmp);
	p.y += font->height;
	snprint(tmp, sizeof tmp, "%5s %-20ud %5s %-20d", "u16:", u16(buf, index), "s16:", s16(buf, index));
	string(b, p, fg, ZP, font, tmp);
	p.y += font->height;
	snprint(tmp, sizeof tmp, "%5s %-20ud %5s %-20d", "u32:", u32(buf, index), "s32:", s32(buf, index));
	string(b, p, fg, ZP, font, tmp);
	p.y += font->height;
	snprint(tmp, sizeof tmp, "%5s %-20llud %5s %-20lld", "u64:", u64(buf, index), "s64:", s64(buf, index));
	string(b, p, fg, ZP, font, tmp);
	p.y += font->height;
	snprint(tmp, sizeof tmp, "%5s %-20e %5s %-20e", "f32:", f32(buf, index), "f64:", f64(buf, index));
	string(b, p, fg, ZP, font, tmp);
}

void
showdec(Buffer *buf, int index, Mousectl *mctl, Keyboardctl *kctl)
{
	Alt alts[3];
	Rectangle r, sc;
	Point o, p;
	Image *b, *save, *bg, *fg, *bord;
	int done, h, w, sw;
	Mouse m;
	Rune k;

	alts[0].op = CHANRCV;
	alts[0].c  = mctl->c;
	alts[0].v  = &m;
	alts[1].op = CHANRCV;
	alts[1].c  = kctl->c;
	alts[1].v  = &k;
	alts[2].op = CHANEND;
	alts[2].c  = nil;
	alts[2].v  = nil;
	while(nbrecv(kctl->c, nil)==1)
		;
	bg = allocimagemix(display, DPaleyellow, DWhite);
	bord = allocimage(display, Rect(0,0,1,1), screen->chan, 1, DYellowgreen);
	fg = allocimage(display, Rect(0,0,1,1), screen->chan, 1, 0x000000ff);
	done = 0;
	save = nil;
	sw = stringwidth(font, " ");
	h = Padding + 6*font->height + Padding + Padding;
	w = Padding + 5*sw + 20*sw + 2*sw + 5*sw + 20*sw + Padding;
	b = screen;
	sc = b->clipr;
	replclipr(b, 0, b->r);
	while(!done){
		o = addpt(screen->r.min, Pt((Dx(screen->r)-w)/2, (Dy(screen->r)-h)/2));
		r = Rect(o.x, o.y, o.x+w, o.y+h);
		if(save==nil){
			save = allocimage(display, r, b->chan, 0, DNofill);
			if(save==nil)
				break;
			draw(save, r, b, nil, r.min);
		}
		draw(b, r, bg, nil, ZP);
		border(b, r, 2, bord, ZP);
		p = addpt(o, Pt(Padding, Padding));
		dec(buf, index, b, o, p, fg);
		flushimage(display, 1);
		if(b!=screen || !eqrect(screen->clipr, sc)){
			freeimage(save);
			save = nil;
		}
		b = screen;
		sc = b->clipr;
		replclipr(b, 0, b->r);
		switch(alt(alts)){
		default:
			continue;
			break;
		case 1:
			done = (k=='\n' || k==Kesc);
			break;
		case 0:
			done = m.buttons&1 && ptinrect(m.xy, r);
			break;
		}
		if(save){
			draw(b, save->r, save, nil, save->r.min);
			freeimage(save);
			save = nil;
		}
			
	}
	replclipr(b, 0, sc);
	flushimage(display, 1);
	freeimage(bg);
	freeimage(fg);
}
