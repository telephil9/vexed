#include <u.h>
#include <libc.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "a.h"

int
readfile(Buffer *buf, char *filename)
{
	int fd;
	long r;

	buf->count = 0;
	buf->size  = 8192;
	buf->data  = malloc(buf->size);
	if(buf->data == nil)
		return -1;
	fd = open(filename, OREAD);
	if(fd < 0)
		return -1;
	for(;;){
		r = read(fd, buf->data + buf->count, buf->size - buf->count);
		if(r < 0)
			return -1;
		if(r == 0)
			break;
		buf->count += r;
		if(buf->count == buf->size){
			buf->size *= 1.5;
			buf->data = realloc(buf->data, buf->size);
			if(buf->data == nil)
				return -1;
		}
	}
	buf->data[buf->count] = 0;
	close(fd);
	return 0;
}

int
writefile(Buffer *buf, char *filename)
{
	int fd, n;

	fd = open(filename, OWRITE|OTRUNC);
	if(fd < 0)
		return -1;
	n = write(fd, buf->data, buf->count);
	if(n < 0 || n != buf->count)
		return -1;
	close(fd);
	return 0;
}

int
delete(Buffer *buf, int index)
{
	if(index != buf->count - 1)
		memmove(&buf->data[index], &buf->data[index + 1], buf->count - index);
	buf->count -= 1;
	/* TODO: is that really what we want ? */
	if(buf->count == 0){
		buf->count = 1;
		buf->data[0] = 0;
	}
	return 0;
}

int
insert(Buffer *buf, int index)
{
	if(buf->count == buf->size){
		buf->size *= 1.5;
		buf->data = realloc(buf->data, buf->size);
		if(buf->data == nil)
			return -1;
	}
	buf->count += 1;
	memmove(&buf->data[index + 1], &buf->data[index], buf->count - index);
	buf->data[index] = 0;
	buf->data[buf->count] = 0;
	return 1;
}

int
append(Buffer *buf, int index)
{
	return insert(buf, index + 1);
}
