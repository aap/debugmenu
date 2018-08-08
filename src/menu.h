enum EntryType
{
	MENUEMPTY = 0,
	MENUSUB,
	MENUVAR,

	MENUVAR_INT,
	MENUVAR_FLOAT,
	MENUVAR_CMD,

	MENUSCROLL	// dummy
};

struct Menu;

struct MenuEntry
{
	int type;
	const char *name;
	MenuEntry *next;
	RwRect r;
	Menu *menu;

	MenuEntry(const char *name);
};

struct Menu
{
	Menu *parent;
	RwRect r;
	MenuEntry *entries;
	int numEntries;
	int maxNameWidth, maxValWidth;

	MenuEntry *findEntry(const char *entryname);
	void insertEntrySorted(MenuEntry *entry);
	void appendEntry(MenuEntry *entry);

	bool8 isScrollingUp, isScrollingDown;
	int scrollStart;
	int numVisible;
	RwRect scrollUpR, scrollDownR;
	void scroll(int off);

	int selection;
	MenuEntry *selectedEntry;	// updated by update
	void changeSelection(int newsel);
	void changeSelection(MenuEntry *e);

	void update(void);
	void draw(void);
	Menu(void){ memset(this, 0, sizeof(Menu)); }
};
extern Menu toplevel;

struct MenuEntry_Sub : MenuEntry
{
	Menu *submenu;

	MenuEntry_Sub(const char *name, Menu *menu);
};

struct MenuEntry_Var : MenuEntry
{
	int maxvallen;
	int vartype;
	bool8 wrapAround;

	virtual void processInput(bool mouseOver, bool selected) = 0;
	int getValWidth(void) { return maxvallen; }
	virtual void getValStr(char *str, int len) = 0;
	MenuEntry_Var(const char *name, int type);
};

struct MenuEntry_Int : MenuEntry_Var
{
	virtual void setStrings(const char **strings) = 0;
	virtual int findStringLen(void) = 0;
	MenuEntry_Int(const char *name);
};

#define MUHINTS \
	X(Int8, int8, 4, "%4" PRId8) \
	X(Int16, int16, 6, "%6" PRId16) \
	X(Int32, int32, 11, "%11" PRId32) \
	X(Int64, int64, 21, "%21" PRId64) \
	X(UInt8, uint8, 4, "%4" PRIu8) \
	X(UInt16, uint16, 6, "%6" PRIu16) \
	X(UInt32, uint32, 11, "%11" PRIu32) \
	X(UInt64, uint64, 21, "%21" PRIu64)
#define MUHFLOATS \
	X(Float32, float, 11, "%11.3f") \
	X(Float64, double, 11, "%11.3lf")

#define X(NAME, TYPE, MAXLEN, FMT) \
struct MenuEntry_##NAME : MenuEntry_Int														       \
{																		       \
	TYPE *variable;																       \
	TYPE lowerBound, upperBound;														       \
	TYPE step;																       \
	TriggerFunc triggerFunc;														       \
	const char *fmt;															       \
	const char **strings;															       \
																		       \
	void processInput(bool mouseOver, bool selected);														       \
	void getValStr(char *str, int len);													       \
																		       \
	void setStrings(const char **strings);													       \
	int findStringLen(void);														       \
	MenuEntry_##NAME(const char *name, TYPE *ptr, TriggerFunc triggerFunc, TYPE step, TYPE lowerBound, TYPE upperBound, const char **strings);     \
};
MUHINTS
#undef X

#define X(NAME, TYPE, MAXLEN, FMT) \
struct MenuEntry_##NAME : MenuEntry_Var												 \
{																 \
	TYPE *variable;														 \
	TYPE lowerBound, upperBound;												 \
	TYPE step;														 \
	TriggerFunc triggerFunc;												 \
	const char *fmt;													 \
																 \
	void processInput(bool mouseOver, bool selected);												 \
	void getValStr(char *str, int len);											 \
																 \
	MenuEntry_##NAME(const char *name, TYPE *ptr, TriggerFunc triggerFunc, TYPE step, TYPE lowerBound, TYPE upperBound);	 \
};
MUHFLOATS
#undef X

struct MenuEntry_Cmd : MenuEntry_Var
{
	TriggerFunc triggerFunc;

	void processInput(bool mouseOver, bool selected);
	void getValStr(char *str, int len);

	MenuEntry_Cmd(const char *name, TriggerFunc triggerFunc);
};


Menu *findMenu(const char *name);
