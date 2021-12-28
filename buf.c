#include <u.h>
#include <libc.h>
#include <draw.h>
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

int writefile(Buffer *buf, char *filename)
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
