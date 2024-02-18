/* BUFFER */
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

/* UNDO */
typedef struct Undo Undo;

enum
{
	Udelete,
	Uinsert,
	Uappend,
	Uset,
};

struct Undo
{
	int action;
	int index;
	uchar value;
	uchar newvalue;
	int modified;
};

int  canundo(void);
void undo(Undo*);
int  canredo(void);
void redo(Undo*);
void pushundo(int, int, uchar, uchar, int);
void patchundo(uchar);

/* DECODE */
void showdec(Buffer*, int, Mousectl*, Keyboardctl*);

/* ERROR */
void showerr(const char*, Mousectl*, Keyboardctl*);

/* COLORS */
enum
{
	BACK,
	ADDR,
	HEX,
	ASCII,
	HHEX,
	DHEX,
	HIGH,
	SCROLL,
	NCOLS,
};

Image* cols[NCOLS];
void initcols(int);
