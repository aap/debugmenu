#include "debugmenu.h"
#include <inttypes.h>

#define strdup _strdup

#define MUHKEYS \
	X(leftjustdown, rsLEFT) \
	X(rightjustdown, rsRIGHT) \
	X(upjustdown, rsUP) \
	X(downjustdown, rsDOWN) \
	X(pgupjustdown, rsPGUP) \
	X(pgdnjustdown, rsPGDN)

#define REPEATDELAY 700
#define REPEATINTERVAL 50
#define X(var, keycode) int var;
MUHKEYS
#undef X
int downtime;
int repeattime;
int lastkeydown;
int *keyptr;

extern CControllerConfigManager *ctrldummy;
static int menuOn;
static int menuInitialized;
static int screenWidth, screenHeight;
static RwRaster *cursor;
static float mouseX, mouseY;

static int firstBorder = 10;
static int topBorder = 10;
static int leading = 4;
static int gap = 10;
static int minwidth = 100;

void drawMouse(void);

Menu toplevel;
Menu *activeMenu = &toplevel;

/*
 * MenuEntry
 */

MenuEntry::MenuEntry(const char *name)
{
	this->type = MENUEMPTY;
	this->name = strdup(name);
	this->next = nil;
}

MenuEntry_Sub::MenuEntry_Sub(const char *name, Menu *menu)
: MenuEntry(name)
{
	this->type = MENUSUB;
	this->submenu = menu;
}

MenuEntry_Var::MenuEntry_Var(const char *name, int vartype)
: MenuEntry(name)
{
	this->type = MENUVAR;
	this->vartype = vartype;
	this->maxvallen = 0;
}

/*
 * *****************************
 * MenuEntry_Int
 * *****************************
 */

MenuEntry_Int::MenuEntry_Int(const char *name)
: MenuEntry_Var(name, MENUVAR_INT)
{
	this->wrapAround = false;
}

#define X(NAME, TYPE, MAXLEN, FMT) \
int								     \
MenuEntry_##NAME::findStringLen(void){				     \
	TYPE i;							     \
	int len, maxlen = 0;					     \
	for(i = this->lowerBound; i <= this->upperBound; i++){	     \
		len = strlen(this->strings[i-this->lowerBound]);     \
		if(len > maxlen)				     \
			maxlen = len;				     \
	}							     \
	return maxlen;						     \
}								     \
void								     \
MenuEntry_##NAME::processInput(void)				     \
{								     \
	TYPE v, oldv;						     \
	int overflow = 0;					     \
	int underflow = 0;					     \
								     \
	v = *this->variable;					     \
	oldv = v;						     \
								     \
	if(leftjustdown){					     \
		v -= this->step;				     \
		if(v > oldv)					     \
			underflow = 1;				     \
	}							     \
	if(rightjustdown){					     \
		v += this->step;				     \
		if(v < oldv)					     \
			overflow = 1;				     \
	}							     \
	if(this->wrapAround){					     \
		if(v > this->upperBound || overflow) v = this->lowerBound;	     \
		if(v < this->lowerBound || underflow) v = this->upperBound;	     \
	}else{									     \
		if(v > this->upperBound || overflow) v = this->upperBound;	     \
		if(v < this->lowerBound || underflow) v = this->lowerBound;	     \
	}							     \
								     \
	*this->variable = v;					     \
	if(oldv != v && this->triggerFunc)			     \
		this->triggerFunc();				     \
}								     \
void											       \
MenuEntry_##NAME::getValStr(char *str, int len)						       \
{											       \
	static char tmp[20];								       \
	if(this->strings){								       \
		snprintf(tmp, 20, "%%%ds", this->maxvallen);				       \
		snprintf(str, len, tmp, this->strings[*this->variable-this->lowerBound]);      \
	}else										       \
		snprintf(str, len, this->fmt, *this->variable);				       \
}											       \
void																			   \
MenuEntry_##NAME::setStrings(const char **strings)													   \
{																			   \
	this->strings = strings;															   \
	if(this->strings)																   \
		this->maxvallen = findStringLen();													   \
}																			   \
MenuEntry_##NAME::MenuEntry_##NAME(const char *name, TYPE *ptr, TriggerFunc triggerFunc, TYPE step, TYPE lowerBound, TYPE upperBound, const char **strings)   \
: MenuEntry_Int(name)																	   \
{																			   \
	this->variable = ptr;																   \
	this->step = step;																   \
	this->lowerBound = lowerBound;															   \
	this->upperBound = upperBound;															   \
	this->triggerFunc = triggerFunc;														   \
	this->maxvallen = MAXLEN;															   \
	this->fmt = FMT;																   \
	this->setStrings(strings);																   \
}
MUHINTS
#undef X

/*
 * *****************************
 * MenuEntry_Float
 * *****************************
 */

#define X(NAME, TYPE, MAXLEN, FMT) \
MenuEntry_##NAME::MenuEntry_##NAME(const char *name, TYPE *ptr, TriggerFunc triggerFunc, TYPE step, TYPE lowerBound, TYPE upperBound)	     \
: MenuEntry_Var(name, MENUVAR_FLOAT)															     \
{																	     \
	this->variable = ptr;														     \
	this->step = step;														     \
	this->lowerBound = lowerBound;													     \
	this->upperBound = upperBound;													     \
	this->triggerFunc = triggerFunc;												     \
	this->maxvallen = MAXLEN;													     \
	this->fmt = FMT;														     \
}																	     \
void																	     \
MenuEntry_##NAME::getValStr(char *str, int len)												     \
{																	     \
	snprintf(str, len, this->fmt, *this->variable);											     \
}																	     \
void																	     \
MenuEntry_##NAME::processInput(void)													     \
{																	     \
	float v, oldv;															     \
	int overflow = 0;					     \
	int underflow = 0;					     \
																	     \
	v = *this->variable;														     \
	oldv = v;															     \
																	     \
	if(leftjustdown){					     \
		v -= this->step;				     \
		if(v > oldv)					     \
			underflow = 1;				     \
	}							     \
	if(rightjustdown){					     \
		v += this->step;				     \
		if(v < oldv)					     \
			overflow = 1;				     \
	}							     \
	if(this->wrapAround){					     \
		if(v > this->upperBound || overflow) v = this->lowerBound;	     \
		if(v < this->lowerBound || underflow) v = this->upperBound;	     \
	}else{									     \
		if(v > this->upperBound || overflow) v = this->upperBound;	     \
		if(v < this->lowerBound || underflow) v = this->lowerBound;	     \
	}							     \
																	     \
	*this->variable = v;														     \
	if(oldv != v && this->triggerFunc)												     \
		this->triggerFunc();													     \
}

MUHFLOATS
#undef X

/*
 * *****************************
 * MenuEntry_Cmd
 * *****************************
 */

void
MenuEntry_Cmd::processInput(void)
{
	if((leftjustdown || rightjustdown) && this->triggerFunc)
		this->triggerFunc();
}

void
MenuEntry_Cmd::getValStr(char *str, int len)
{
	strncpy(str, "<", len);
}

MenuEntry_Cmd::MenuEntry_Cmd(const char *name, TriggerFunc triggerFunc)
: MenuEntry_Var(name, MENUVAR_CMD)
{
	this->maxvallen = 1;
	this->triggerFunc = triggerFunc;
}


/*
 * *****************************
 * Menu
 * *****************************
 */

void
Menu::scroll(int off) {
	if(isScrollingUp && off < 0)
		scrollStart += off;
	if(isScrollingDown && off > 0)
		scrollStart += off;
	if(scrollStart < 0) scrollStart = 0;
	if(scrollStart > numEntries-numVisible) scrollStart = numEntries-numVisible;
}

void
Menu::changeSelection(int newsel){
	selection = newsel;
	if(selection < 0) selection = 0;
	if(selection >= numEntries) selection = numEntries-1;
	if(selection < scrollStart) scrollStart = selection;
	if(selection >= scrollStart+numVisible) scrollStart = selection-numVisible+1;
}


MenuEntry*
Menu::findEntry(const char *entryname)
{
	MenuEntry *m;
	for(m = this->entries; m; m = m->next)
		if(strcmp(entryname, m->name) == 0)
			return m;
	return nil;
}

void
Menu::insertEntrySorted(MenuEntry *entry)
{
	MenuEntry **mp;
	int cmp;
	for(mp = &this->entries; *mp; mp = &(*mp)->next){
		cmp = strcmp(entry->name, (*mp)->name);
		if(cmp == 0)
			return;
		if(cmp < 0)
			break;
	}
	entry->next = *mp;
	*mp = entry;
	this->numEntries++;
}

void
Menu::appendEntry(MenuEntry *entry)
{
	MenuEntry **mp;
	for(mp = &this->entries; *mp; mp = &(*mp)->next);
	entry->next = *mp;
	*mp = entry;
	this->numEntries++;
}

void
Menu::update(void)
{
	int i;
	int x, y;
	Pt sz;
	MenuEntry *e;
	int onscreen;
	x = this->r.x;
	y = this->r.y + 10;
	int end = this->r.y+this->r.h - 10;
	this->numVisible = 0;

	onscreen = 1;
	i = 0;
	this->maxNameWidth = 0;
	this->maxValWidth = 0;
	this->isScrollingUp = this->scrollStart > 0;
	this->isScrollingDown = false;
	this->selectedEntry = nil;
	for(e = this->entries; e; e = e->next){
		sz = fontGetStringSize(e->name);
		e->r.x = x;
		e->r.y = y;
		e->r.w = sz.x;
		e->r.h = sz.y;

		if(i == this->selection)
			this->selectedEntry = e;

		if(i >= this->scrollStart)
			y += sz.y + leading;
		if(y >= end){
			this->isScrollingDown = true;
			onscreen = 0;
		}
		if(i >= this->scrollStart && onscreen)
			this->numVisible++;

		if(e->type == MENUVAR){
			int valwidth = fontGetLen(((MenuEntry_Var*)e)->getValWidth());
			if(valwidth > maxValWidth)
				maxValWidth = valwidth;
		}
		if(e->r.w > maxNameWidth)
			maxNameWidth = e->r.w;
		i++;
	}
	if(this->r.w < maxNameWidth + maxValWidth + gap)
		this->r.w = maxNameWidth + maxValWidth + gap;

	// Update active submenu
	if(this->selectedEntry && this->selectedEntry->type == MENUSUB){
		Menu *submenu = ((MenuEntry_Sub*)this->selectedEntry)->submenu;
		submenu->r.x = this->r.x+this->r.w + 10;
		submenu->r.y = this->r.y;
		submenu->r.w = minwidth;	// update menu will expand
		submenu->r.h = this->r.h;
		submenu->update();
	}
}

void
Menu::draw(void)
{
	static char val[100];
	int i;
	MenuEntry *e;
	i = 0;
	for(e = this->entries; e; e = e->next){
		if(i >= this->scrollStart+this->numVisible)
			break;
		if(i >= this->scrollStart){
			int style = i == this->selection;
			if(style && this == activeMenu)
				style = 2;
			fontPrint(e->name, e->r.x, e->r.y, style);
			if(e->type == MENUVAR){
				int valw = fontGetLen(((MenuEntry_Var*)e)->getValWidth());
				((MenuEntry_Var*)e)->getValStr(val, 100);
				fontPrint(val, e->r.x+this->r.w-valw, e->r.y, style);
			}
		}
		i++;
	}

	if(this->selectedEntry && this->selectedEntry->type == MENUSUB)
		((MenuEntry_Sub*)this->selectedEntry)->submenu->draw();
}

Menu*
findMenu(const char *name)
{
	Menu *m;
	MenuEntry *e;
	char *tmppath = strdup(name);
	char *next, *curname;

	curname = tmppath;
	next = curname;

	m = &toplevel;
	while(*next){
		curname = next;
		while(*next){
			if(*next == '|'){
				*next++ = '\0';
				break;
			}
			next++;
		}
		e = m->findEntry(curname);
		if(e){
			// return an error if the entry exists but isn't a menu
			if(e->type != MENUSUB){
				free(tmppath);
				return nil;
			}
			m = ((MenuEntry_Sub*)e)->submenu;
		}else{
			// Create submenus that don't exist yet
			Menu *submenu = new Menu();
			submenu->parent = m;
			MenuEntry *me = new MenuEntry_Sub(curname, submenu);
			m->insertEntrySorted(me);
			m = submenu;
		}
	}

	free(tmppath);
	return m;
}

/*
 * ****************
 * debug menu
 * ****************
 */

void
initDebug(void)
{
	createDebugFonts();

	RwInt32 w, h, d, flags;
	RwImage *img = readTGA(IDR_CURSOR);
	RwImageFindRasterFormat(img, rwRASTERTYPETEXTURE, &w, &h, &d, &flags);
	cursor = RwRasterCreate(w, h, d, flags);
	cursor = RwRasterSetFromImage(cursor, img);
	assert(cursor);
	RwImageDestroy(img);

}

void
processInput(void)
{
	int shift = KEYDOWN(rsRSHIFT) || KEYDOWN(rsLSHIFT);
#define X(var, keycode) var = KEYJUSTDOWN(keycode);
	MUHKEYS
#undef X

	// Implement auto-repeat
#define X(var, keycode) \
	if(var){ \
		repeattime = downtime = CTimer::m_snTimeInMilliseconds; \
		lastkeydown = keycode; \
		keyptr = &var; \
	}
	MUHKEYS
#undef X
	if(lastkeydown){
		if(KEYDOWN(lastkeydown)){
			int curtime = CTimer::m_snTimeInMilliseconds;
			if(curtime - downtime > REPEATDELAY){
				if(curtime - repeattime > REPEATINTERVAL){
					repeattime = curtime;
					*keyptr = 1;
				}
			}
		}else{
			lastkeydown = 0;
		}
	}

	if(pgupjustdown)
		activeMenu->scroll(shift ? -5 : -1);
	if(pgdnjustdown)
		activeMenu->scroll(shift ? 5 : 1);
	if(downjustdown)
		activeMenu->changeSelection(activeMenu->selection + (shift ? 5 : 1));
	if(upjustdown)
		activeMenu->changeSelection(activeMenu->selection - (shift ? 5 : 1));
	if(KEYJUSTDOWN(rsEXTENTER)){
		if(activeMenu->selectedEntry && activeMenu->selectedEntry->type == MENUSUB)
			activeMenu = ((MenuEntry_Sub*)activeMenu->selectedEntry)->submenu;
	}else if(KEYJUSTDOWN(rsBACK)){
		if(activeMenu->parent)
			activeMenu = activeMenu->parent;
	}else if(activeMenu->selectedEntry && activeMenu->selectedEntry->type == MENUVAR)
		((MenuEntry_Var*)activeMenu->selectedEntry)->processInput();
}

void
processMouse(void)
{
	mouseX += CPad::GetPad(0)->NewMouseControllerState.X;
	mouseY -= CPad::GetPad(0)->NewMouseControllerState.Y;
	if(mouseX < 0.0f) mouseX = 0.0f;
	if(mouseY < 0.0f) mouseY = 0.0f;
	if(mouseX >= screenWidth) mouseX = screenWidth;
	if(mouseY >= screenHeight) mouseY = screenHeight;
}

void
processDebug(void)
{
	CPad *pad = CPad::GetPad(0);
	if(CTRLJUSTDOWN('M'))
		menuOn = !menuOn;
	if(!menuOn)
		return;

	RwCamera *cam = (RwCamera*)RWSRCGLOBAL(curCamera);
	screenWidth = RwRasterGetWidth(RwCameraGetRaster(cam));
	screenHeight = RwRasterGetHeight(RwCameraGetRaster(cam));

	CPad::GetPad(0)->DisablePlayerControls = 1;
	if(!menuInitialized){
		initDebug();
		menuInitialized = 1;
	}
	processMouse();

	// !!! Important to set up the correct font for update and draw!
	curfont = &vga;
	Pt sz;
	sz = fontPrint("Debug Menu", firstBorder, topBorder, 0);

	toplevel.r.x = firstBorder;
	toplevel.r.y = topBorder + sz.y + 10;
	toplevel.r.w = minwidth;	// update menu will expand
	toplevel.r.h = screenHeight - 10 - toplevel.r.y;
	toplevel.update();
	toplevel.draw();
	processInput();

	{
		static char xxx[100];
//		sprintf(xxx, "scrolling up:%d down:%d %d %d", toplevel.isScrollingUp, toplevel.isScrollingDown, toplevel.scrollStart, toplevel.scrollStart+toplevel.numVisible);
//		fontPrint(xxx, toplevel.r.x+toplevel.r.w, toplevel.r.y, 0);
	}


	drawMouse();
}



void
drawMouse(void)
{
	static RwImVertexIndex indices[] = { 0, 1, 2, 2, 1, 3 };
	static RwIm2DVertex vertices[4];
	RwIm2DVertex *vert;
	RwCamera *cam;
	cam = (RwCamera*)RWSRCGLOBAL(curCamera);
	float x = mouseX;
	float y = mouseY;
	float w = RwRasterGetWidth(cursor);
	float h = RwRasterGetHeight(cursor);
	float recipz = 1.0f/RwCameraGetNearClipPlane(cam);

	vert = vertices;
	RwIm2DVertexSetScreenX(vert, x);
	RwIm2DVertexSetScreenY(vert, y);
	RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(vert, recipz);
	RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
	RwIm2DVertexSetU(vert, 0.0f, recipz);
	RwIm2DVertexSetV(vert, 0.0f, recipz);
	vert++;

	RwIm2DVertexSetScreenX(vert, x+w);
	RwIm2DVertexSetScreenY(vert, y);
	RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(vert, recipz);
	RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
	RwIm2DVertexSetU(vert, 1.0f, recipz);
	RwIm2DVertexSetV(vert, 0.0f, recipz);
	vert++;

	RwIm2DVertexSetScreenX(vert, x);
	RwIm2DVertexSetScreenY(vert, y+h);
	RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(vert, recipz);
	RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
	RwIm2DVertexSetU(vert, 0.0f, recipz);
	RwIm2DVertexSetV(vert, 1.0f, recipz);
	vert++;

	RwIm2DVertexSetScreenX(vert, x+w);
	RwIm2DVertexSetScreenY(vert, y+h);
	RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(vert, recipz);
	RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
	RwIm2DVertexSetU(vert, 1.0f, recipz);
	RwIm2DVertexSetV(vert, 1.0f, recipz);
	vert++;

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, cursor);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, vertices, 4, indices, 6);
}
