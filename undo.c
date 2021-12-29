#include <u.h>
#include <libc.h>
#include <draw.h>
#include "a.h"

enum { Stacksize = 1024 };
Undo ustack[Stacksize];
int ucount = 0;
int ucur = -1;

int
canundo(void)
{
	return ucur >= 0;
}

void
undo(Undo *undo)
{
	undo->action = ustack[ucur].action;
	undo->index = ustack[ucur].index;
	undo->value = ustack[ucur].value;
	undo->newvalue = ustack[ucur].newvalue;
	undo->modified = ustack[ucur].modified;
	ucur -= 1;
}

int
canredo(void)
{
	return ucur < ucount - 1;
}

void
redo(Undo *undo)
{
	ucur += 1;
	undo->action = ustack[ucur].action;
	undo->index = ustack[ucur].index;
	undo->value = ustack[ucur].value;
	undo->newvalue = ustack[ucur].newvalue;
	undo->modified = ustack[ucur].modified;
}

void
pushundo(int action, int index, uchar value, uchar newvalue, int modified)
{
	if(ucur == Stacksize - 1)
		return;
	ucur += 1;
	ucount = ucur + 1;
	ustack[ucur].action = action;
	ustack[ucur].index = index;
	ustack[ucur].value = value;
	ustack[ucur].newvalue = newvalue;
	ustack[ucur].modified = modified;
}

void
patchundo(uchar value)
{
	ustack[ucur].newvalue = value;
}

