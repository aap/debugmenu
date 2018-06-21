#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include "windows.h"
#include <stdint.h>
#include <assert.h>
#include <rwcore.h>
#include <rpworld.h>
typedef uintptr_t uintptr;
#include "MemoryMgr.h"
#include "resources.h"

#define EXPORT extern "C" __declspec(dllexport)

typedef uint8_t uint8, uchar;
typedef uint16_t uint16, ushort;
typedef uint32_t uint32, uint;
typedef uint64_t uint64;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

#include "debugmenu_api.h"

#define RAD2DEG(x) (float)(180.0f*(x)/M_PI)
#define DEG2RAD(x) (float)((x)/180.0f*M_PI)

#define nil NULL

RwImage *readTGA(uint8 *p);
RwImage *readTGA(const char *afilename);
RwImage *readTGA(int res);

struct Pt
{
	int x, y;
};

enum FontStyle
{
	FONT_NORMAL,
	FONT_SEL_ACTIVE,
	FONT_SEL_INACTIVE,
	FONT_MOUSE
};

// font
struct Font
{
	RwRaster *rasters[4];
	int glyphwidth, glyphheight;
	int numglyphs;
};
extern Font vga, bios, *curfont;
extern int fontscale;
void createDebugFonts(void);
Pt fontPrint(const char *s, float x, float y, int style);
Pt fontGetStringSize(const char *s);
int fontGetLen(int len);

EXPORT void DebugMenuProcess(void);
EXPORT void DebugMenuRender(void);

#include "menu.h"


#define RwEngineInstance RwEngineInst
extern void **&RwEngineInst;

class CTimer
{
public:
	static int &m_snTimeInMilliseconds;
};

struct CControllerState
{
	short LEFTSTICKX;
	short LEFTSTICKY;
	short RIGHTSTICKX;
	short RIGHTSTICKY;
	short LEFTSHOULDER1;
	short LEFTSHOULDER2;
	short RIGHTSHOULDER1;
	short RIGHTSHOULDER2;
	short DPADUP;
	short DPADDOWN;
	short DPADLEFT;
	short DPADRIGHT;
	short START;
	short SELECT;
	short SQUARE;
	short TRIANGLE;
	short CROSS;
	short CIRCLE;
	short LEFTSHOCK;
	short RIGHTSHOCK;
	short NETWORK_TALK;
#ifdef GTASA
	short m_bPedWalk;
	short m_bVehicleMouseLook;
	short m_bRadioTrackSkip;
#endif
};
#if defined (GTA3) || defined (GTAVC)
static_assert(sizeof(CControllerState) == 0x2A, "CControllerState: wrong size");
#else
static_assert(sizeof(CControllerState) == 0x30, "CControllerState: wrong size");
#endif

struct CMouseControllerState
{
	uchar lmb;
	uchar rmb;
	uchar mmb;
	uchar wheelUp;
	uchar wheelDown;
	uchar bmx1;
	uchar bmx2;
#ifdef GTASA
	float Z;
#endif
	float X;
	float Y;
};

enum RsKeyCodes
{
	rsESC = 0x3E8,
	rsF1 = 0x3E9,
	rsF2 = 0x3EA,
	rsF3 = 0x3EB,
	rsF4 = 0x3EC,
	rsF5 = 0x3ED,
	rsF6 = 0x3EE,
	rsF7 = 0x3EF,
	rsF8 = 0x3F0,
	rsF9 = 0x3F1,
	rsF10 = 0x3F2,
	rsF11 = 0x3F3,
	rsF12 = 0x3F4,
	rsINSERT = 0x3F5,
	rsDEL = 0x3F6,
	rsHOME = 0x3F7,
	rsEND = 0x3F8,
	rsPGUP = 0x3F9,
	rsPGDN = 0x3FA,
	rsUP = 0x3FB,
	rsDOWN = 0x3FC,
	rsLEFT = 0x3FD,
	rsRIGHT = 0x3FE,
	rsDIV = 0x3FF,
	rsMUL = 0x400,
	rsADD = 0x401,
	rsSUB = 0x402,
	rsEXTDEL = 0x403,
	rsEXT_END = 0x404,
	rsEXTDOWN = 0x405,
	rsEXTPGDN = 0x406,
	rsEXTLEFT = 0x407,
	rsNUM5 = 0x408,
	rsNUMLOCK = 0x409,
	rsEXTRIGHT = 0x40A,
	rsEXTHOME = 0x40B,
	rsEXTUP = 0x40C,
	rsEXTPGUP = 0x40D,
	rsEXTINSERT = 0x40E,
	rsENTER = 0x40F,
	rsSCROLL = 0x410,
	rsPAUSE = 0x411,
	rsBACK = 0x412,
	rsTAB = 0x413,
	rsCAPSLOCK = 0x414,
	rsEXTENTER = 0x415,
	rsLSHIFT = 0x416,
	rsRSHIFT = 0x417,
	rsSHIFT = 0x418,
	rsLCTRL = 0x419,
	rsRCTRL = 0x41A,
	rsLMENU = 0x41B,
	rsRMENU = 0x41C,
	rsLWIN = 0x41D,
	rsRWIN = 0x41E,
	rsAPPS = 0x41F,
	rsNULL = 0x420,
};

class CPadIII
{
public:
	CControllerState	NewState;
	CControllerState	OldState;
	CControllerState	PCTempKeyState;
	CControllerState	PCTempJoyState;
	CControllerState	PCTempMouseState;
	BYTE			Phase;
	WORD			Mode;
	short			ShakeDur;
	BYTE			ShakeFreq;
	bool			bHornHistory[5];
	BYTE			iCurrHornHistory;
	bool			DisablePlayerControls;
	bool			JustOutOfFrontEnd;
	bool			bApplyBrakes;
	BYTE			_pad1[12];
	int			LastTimeTouched;
	DWORD			AverageWeapon;
	DWORD			AverageEntries;
};

class CPadVC
{
public:
	CControllerState	NewState;
	CControllerState	OldState;
	short			SteeringLeftRightBuffer[10];
	int				DrunkDrivingBufferUsed;
	CControllerState	PCTempKeyState;
	CControllerState	PCTempJoyState;
	CControllerState	PCTempMouseState;
	BYTE			Phase;
	WORD			Mode;
	short			ShakeDur;
	WORD			DisablePlayerControls;
	BYTE			ShakeFreq;
	bool			bHornHistory[5];
	BYTE			iCurrHornHistory;
	bool			JustOutOfFrontEnd;
	bool			bApplyBrakes;
	BYTE			_pad1[12];
	int			LastTimeTouched;
	DWORD			AverageWeapon;
	DWORD			AverageEntries;
};

class CPadSA
{
public:
	CControllerState	NewState;
	CControllerState	OldState;
	WORD			SteeringLeftRightBuffer[10];
	DWORD			DrunkDrivingBufferUsed;
	CControllerState	PCTempKeyState;
	CControllerState	PCTempJoyState;
	CControllerState	PCTempMouseState;
	BYTE			Phase;
	WORD			Mode;
	short			ShakeDur;
	WORD			DisablePlayerControls;
	BYTE			ShakeFreq;
	BYTE			bHornHistory[5];
	BYTE			iCurrHornHistory;
	bool			JustOutOfFrontEnd;
	bool			bApplyBrakes;
	bool			bDisablePlayerEnterCar;
	bool			bDisablePlayerDuck;
	bool			bDisablePlayerFireWeapon;
	bool			bDisablePlayerFireWeaponWithL1;
	bool			bDisablePlayerCycleWeapon;
	bool			bDisablePlayerJump;
	bool			bDisablePlayerDisplayVitalStats;
	int			LastTimeTouched;
	DWORD			AverageWeapon;
	DWORD			AverageEntries;
	DWORD			NoShakeBeforeThis;
	BYTE			NoShakeFreq;
};

#if defined (GTA3)
class CPad : public CPadIII
#elif defined (GTAVC)
class CPad : public CPadVC
#elif defined (GTASA)
class CPad : public CPadSA
#endif
{
public:
	static CMouseControllerState &NewMouseControllerState;
	static CMouseControllerState &OldMouseControllerState;

	static CPad *GetPad(int);
};
#if defined (GTA3)
static_assert(sizeof(CPad) == 0xFC, "CPad: wrong size");
#elif defined (GTAVC)
static_assert(sizeof(CPad) == 0x114, "CPad: wrong size");
#elif defined (GTASA)
static_assert(sizeof(CPad) == 0x134, "CPad: wrong size");
#endif

class CControllerConfigManager
{
public:
	bool GetIsKeyboardKeyDown(RsKeyCodes key);
	bool GetIsKeyboardKeyJustDown(RsKeyCodes key);
};

#define JUSTDOWN(b) (pad->NewState.b && !pad->OldState.b)
#define KEYJUSTDOWN(k) ctrldummy->GetIsKeyboardKeyJustDown((RsKeyCodes)k)
#define KEYDOWN(k) ctrldummy->GetIsKeyboardKeyDown((RsKeyCodes)k)
#define CTRLJUSTDOWN(key) \
	       ((KEYDOWN(rsLCTRL) || KEYDOWN(rsRCTRL)) && KEYJUSTDOWN((RsKeyCodes)key) || \
	        (KEYJUSTDOWN(rsLCTRL) || KEYJUSTDOWN(rsRCTRL)) && KEYDOWN((RsKeyCodes)key))
#define CTRLDOWN(key) ((KEYDOWN(rsLCTRL) || KEYDOWN(rsRCTRL)) && KEYDOWN((RsKeyCodes)key))

extern bool &MouseInvertX, &MouseInvertY;
