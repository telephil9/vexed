typedef struct Buffer Buffer;

struct Buffer
{
	uchar *data;
	usize count;
	usize size;
};

int readfile(Buffer*, char*);
int writefile(Buffer*, char*);
int delete(Buffer*, int);
int insert(Buffer*, int);
int append(Buffer*, int);

/* COLORS */
enum
{
	BACK,
	ADDR,
	HEX,
	ASCII,
	SCROLL,
	NCOLS,
};

Image* cols[NCOLS];
void initcols(int);
