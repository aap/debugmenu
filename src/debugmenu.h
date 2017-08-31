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

#define EXPORT __declspec(dllexport)

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

void debughooks(void);

RwImage *readTGA(uint8 *p);
RwImage *readTGA(const char *afilename);
RwImage *readTGA(int res);

struct Pt
{
	int x, y;
};

// font
struct Font
{
	RwRaster *rasters[3];
	int glyphwidth, glyphheight;
	int numglyphs;
};
extern Font vga, bios, *curfont;
void createDebugFonts(void);
Pt fontPrint(const char *s, float x, float y, int style);
Pt fontGetStringSize(const char *s);
int fontGetLen(int len);

// debug menu
void processDebug(void);

#include "menu.h"



#define RwEngineInstance (*rwengine)
extern void **rwengine;

struct GlobalScene
{
	RpWorld *world;
	RwCamera *camera;
};
extern GlobalScene &Scene;

struct RsInputDevice
{
	int inputDeviceType;
	int used;
	void *inputEventHandler;
};

struct RsGlobalType
{
	const char *appName;
	int width;
	int height;
	int maximumWidth;
	int maximumHeight;
	int maxFPS;
	int quit;
	void *ps;
	RsInputDevice keyboard;
	RsInputDevice mouse;
	RsInputDevice pad;
};
extern RsGlobalType &RsGlobal;

class CVector
{
public:
	float x, y, z;
	CVector(void) {}
	CVector(float x, float y, float z) : x(x), y(y), z(z) {}
	CVector operator-(const CVector &rhs) const {
		return CVector(x-rhs.x, y-rhs.y, z-rhs.z);
	}
	CVector operator+(const CVector &rhs) const {
		return CVector(x+rhs.x, y+rhs.y, z+rhs.z);
	}
	CVector operator*(float f) const {
		return CVector(x*f, y*f, z*f);
	}
	void Normalize(void);
};

class CMatrix
{
public:
	RwMatrix matrix;
	RwMatrix *pMatrix;
	bool haveRwMatrix;
};

struct CPlaceable
{
	void **vtable;
	CMatrix matrix;
};
static_assert(sizeof(CPlaceable) == 0x4C, "CPlaceable: wrong size");

struct CEntity : CPlaceable
{
	RpClump *clump;
	char bfTypeStatus;
	char bfFlagsA;
	char bfFlagsB;
	char bfFlagsC;
	char bfFlagsD;
	char bfFlagsE;
	char __f0052[2];
	short unk1;
	short uiPathMedianRand;
	short nModelIndex;
	short level;
	int pFirstRef;

	void SetModelIndex(int id);
	void Render(void);
	void UpdateRpHAnim(void);
	void UpdateRwFrame(void);
};
static_assert(sizeof(CEntity) == 0x64, "CEntity: wrong size");

class CRect;

struct CEntityVMT
{
	void (__cdecl *dtor)(void *);
	int Add;
	int Remove;
	int SetModelIndex;
	void (__cdecl *SetModelIndexNoCreate)(uint);
	int CreateRwObject;
	int DeleteRwObject;
	CRect *(__thiscall *GetBoundRect)(CEntity *e, CRect *ret);
	int ProcessControl;
	int ProcessCollision;
	int ProcessShift;
	void (__thiscall *Teleport)(CEntity *e, float x, float y, float z);
	int PreRender;
	int Render;
	int SetupLighting;
	int RemoveLighting;
	int FlagToDestroyWhenNextProcessed;
};

struct CPhysical : CEntity
{
	int uAudioEntityId;
	int unk1;
	int field_6C;
	int field_70;
	int m_dwLastTimeCollided;
	CVector vecMoveSpeed;
	CVector vecTurnSpeed;
	CVector vecShiftVector1;
	CVector vecShiftVector2;
	CVector vecShiftVector3;
	CVector vecShiftVector4;
	float fMass;
	float fTurnMass;
	float fForceMultiplier;
	float fAirResistance;
	int fElasticity;
	float fPercentSubmerged;
	CVector vecCentreOfMass;
	void *pEntryInfoNode;
	int pMovingListNode;
	char field_EC;
	char field_ED;
	char uCollidingNum;
	char field_EF;
	CEntity *pCollisionRecords[6];
	float fTotSpeed;
	float fCollisionPower;
	float pPhysColliding;
	CVector vecCollisionPower;
	short wComponentCol;
	char byteMoveFlags;
	char byteCollFlags;
	char byteLastCollType;
	char byteZoneLevel;
	short __padding;

	void ProcessControl(void);
};
static_assert(sizeof(CPhysical) == 0x128, "CPhysical: wrong size");

struct CPed : CEntity
{
	int pad;
};

struct CVehicle : CEntity
{
	int pad;
};

struct CCam {
	void WorkOutCamHeight(float *vec, float a, float b);
	void WorkOutCamHeight_hook(float *vec, float a, float b);
	void Process_FollowPed(float *vec, float a, float b, float c);
	void Process_FollowPed_hook(float *vec, float a, float b, float c);
	void Process_Editor(float *vec, float a, float b, float c);
	void Process_Editor_hook(float *vec, float a, float b, float c);
	void Process_Debug(float *vec, float a, float b, float c);
	void Process_Debug_hook(float *vec, float a, float b, float c);
	void GetVectorsReadyForRW(void);

	char bBelowMinDist;
	char bBehindPlayerDesired;
	char bCamLookingAtVector;
	char bCollisionChecksOn;
	char bFixingBeta;
	char bTheHeightFixerVehicleIsATrain;
	char bLookBehindCamWasInFront;
	char bLookingBehind;
	char bLookingLeft;
	char bLookingRight;
	char bRotating;
	char field_B;
	short Mode;
	char gap_E[2];

	int dwFinishTime;
	int nDoCollisionChecksOnFrameNum;
	int nDoCollisionCheckEveryNumOfFrames;
	int nFrameNumWereAt;
	int nRunningVectorArrayPos;
	int nRunningVectorCounter;
	float nDirectionWasLooking;
	float fMaxRollAngle;
	float fRoll;
	float fRollSpeed;
	float fSyphonModeTargetZOffSet;
	// not in VC apparently, but it is really this field that's gone?
	float var11;
	float fAmountFractionObscured;
	float fAlphaSpeedOverOneFrame;
	float fBetaSpeedOverOneFrame;
	float fBufferedTargetBeta;
	float fBufferedTargetOrientation;
	float fBufferedTargetOrientationSpeed;
	float fCamBufferedHeight;
	float fCamBufferedHeightSpeed;
	float fCloseInPedHeightOffset;
	float fCloseInPedHeightOffsetSpeed;
	float fCloseInCarHeightOffset;
	float fCloseInCarHeightOffsetSpeed;
	float fDimensionOfHighestNearCar;
	float fDistanceBeforeChanges;
	float fFovSpeedOverOneFrame;
	float fMinDistAwayFromCamWhenInterPolating;
	float fPedBetweenCameraHeightOffset;
	float fPlayerInFrontSyphonAngleOffset;
	float fRadiusForDead;
	float fRealGroundDist;
	float fTargetBeta;
/*	// which is which?
	float fTimeElapsedFloat;
	float fTilt;
	float fTiltSpeed;
	float fTransitionBeta;
*/
	float var25, var26;
	float fTrueBeta;
	float fTrueAlpha;
	float fInitialPlayerOrientation;
	float fAlpha;
	float fAlphaSpeed;
	float fFOV;
	float fFOVSpeed;
	float fBeta;
	float fBetaSpeed;
	float fDistance;
	float fDistanceSpeed;
	float fCaMinDistance;
	float fCaMaxDistance;
	float fSpeedVar;
	float fTargetZoomGroundOne;
	float fTargetZoomGroundTwo;
	float fTargetZoomGroundThree;
	float fTargetZoomOneZExtra;
	float fTargetZoomTwoZExtra;
	float fTargetZoomThreeZExtra;
	float fTargetZoomZCloseIn;
	float fMinRealGroundDist;
	float fTargetCloseInDist;

	CVector vecTargetCoorsForFudgeInter;
	CVector vecCamFixedModeVector;
	CVector vecCamFixedModeSource;
	CVector vecCamFixedModeUpOffSet;
	CVector vecLastAboveWaterCamPosition;
	CVector vecBufferedPlayerBodyOffset;
	CVector vecFront;
	CVector vecPos;
	CVector vecSourceBeforeLookBehind;
	CVector vecLookUp;
	CVector vecArrPrevVectors1;
	CVector vecArrPrevVectors2;
	CEntity *pTargetEntity;
	float fCameraDistance;
	float fIdealAlpha;
	float fPlayerVelocity;
	CVehicle *pLastCarEntered;
	CPed *pLastPedLookedAt;
	char bFirstPersonRunAboutActive;
};
static_assert(sizeof(CCam) == 0x1A4, "CCam: wrong size");

class CCamera
{
public:
	void *vmt;
	CMatrix m_matrix;
	__int16 field_4C;
	char anonymous_10;
	char anonymous_11;
	uchar gap50[1];
	char anonymous_12;
	uchar gap52[3];
	char anonymous_13;
	char anonymous_14;
	uchar gap57[2];
	char anonymous_15;
	char anonymous_16;
	char anonymous_17;
	char anonymous_18;
	char anonymous_19;
	char anonymous_20;
	char anonymous_21;
	uchar gap60[6];
	char anonymous_22;
	uchar gap67[2];
	char anonymous_23;
	char anonymous_24;
	char anonymous_25[5];
	char widescreen;
	char anonymous_27;
	char anonymous_28;
	char anonymous_29;
	char anonymous_30;
	char debugMode;
	char ActiveCam;
	uchar gap77[18];
	char anonymous_32;
	char anonymous_33[6];
	int anonymous_34;
	int field_94;
	int field_98;
	int motionBlurBlue;
	int motionBlurGreen;
	int motionBlurRed;
	int motionBlurType;
	int field_AC;
	int field_B0;
	int field_B4;
	int field_B8;
	int motionBlurAlpha;
	int field_C0;
	int field_C4;
	int field_C8;
	int anonymous_36;
	float anonymous_37;
	float anonymous_38;
	float distSwitch_Car;
	float someDistance_Car;
	float anonymous_40;
	float anonymous_41;
	uchar gapE8[4];
	float LODmult;
	float anonymous_42;
	uchar gapF4[40];
	float anonymous_43;
	float anonymous_44;
	char anonymous_45[4];
	int anonymous_46;
	char anonymous_47[4];
	float fov;
	uchar gap134[20];
	float anonymous_49;
	float anonymous_50;
	float anonymous_51;
	char anonymous_52[4];
	float anonymous_53;
	float anonymous_54;
	float distSwitch_Ped;
	float anonymous_56;
	float anonymous_57;
	uchar gap16C[40];
	float anonymous_58;
	float anonymous_59;
	float anonymous_60;
	float anonymous_61;
	CCam Cams[3];
	int anonymous_87;
	int anonymous_88;
	uchar gap698[12];
	__int16 anonymous_89;
	char field_6A6[10];
	CVector field_6B0;
	CVector pos;
	CVector field_6C8;
	CVector field_6D4;
	CVector field_6E0;
	CVector field_6EC;
	CVector field_6F8;
	CVector field_704;
	CVector field_710;
	CVector field_71C;
	CVector field_728;
	CVector field_734;
	CVector field_740;
	CVector field_74C;
	CVector field_758;
	CVector field_764;
	CVector field_770;
	CVector field_77C;
	CVector field_788;
	CVector field_794;
	RwCamera *rwCamera;
	int targetEntity;
	char pathSplines[12800];
	char hugeArray2[44800];
	CMatrix field_E8A8;
	char field_E8F0[8];
	CMatrix viewMatrix;
	CVector field_E940;
	CVector field_E94C;
	CVector field_E958;
	CVector field_E964;
	CVector field_E970;
	CVector field_E97C;
	CVector field_E988;
	char field_E994[64];
	float anonymous_147;

	void InitialiseCameraForDebugMode(void);
};
static_assert(sizeof(CCamera) == 0xE9D8, "CCamera: wrong size");
extern CCamera &TheCamera;

class CFileMgr
{
public:
	static void SetDir(const char *path);
};

class CGame
{
public:
	static void ReloadIPLs(void);
	static void Process(void);
};

class CTimer
{
public:
	static uchar &m_UserPause;
	static uchar &m_CodePause;
	static int &m_snTimeInMilliseconds;
	static void Stop(void);
	static void Update(void);
};

class CClock
{
public:
	static char  &ms_nGameClockHours;
	static char  &ms_nGameClockMinutes;
	static short &ms_nGameClockSeconds;
};

class CWeather
{
public:
	static float &InterpolationValue;
	static short &OldWeatherType;
	static short &NewWeatherType;
};

class CCoronas
{
public:
	static void DoSunAndMoon(void);
	static void Update(void);
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
};

struct CMouseControllerState
{
	uchar lmb;
	uchar rmb;
	uchar mmb;
	uchar wheelUp;
	uchar wheelDown;
	uchar bmx1;
	uchar bmx2;
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

class CPad
{
public:
	CControllerState NewState;
	CControllerState OldState;
	CControllerState PCTempKeyState;
	CControllerState PCTempJoyState;
	CControllerState PCTempMouseState;
	char Phase;
	char gap_d3[1];
	short Mode;
	short ShakeDur;
	char ShakeFreq;
	char bHornHistory[5];
	char iCurrHornHistory;
	char DisablePlayerControls;
	char JustOutOfFrontEnd;
	char bApplyBrakes;
	char field_e2[12];
	char gap_ee[2];
	int LastTimeTouched;
	int AverageWeapon;
	int AverageEntries;

	static short *NewKeyState;	// 312
	static short *OldKeyState;	// 312
	static CMouseControllerState &NewMouseControllerState;
	static CMouseControllerState &OldMouseControllerState;
	static bool &m_bMapPadOneToPadTwo;

	static CPad *GetPad(int);
};
static_assert(sizeof(CPad) == 0xFC, "CPad: wrong size");

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

class CRGBA
{
public:
	uchar r, g, b, a;
	CRGBA(uchar r, uchar g, uchar b, uchar a) : r(r), g(g), b(b), a(a) {}
};

class CRect
{
public:
	float left, top, right, bottom;
	CRect(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {}
};

class CSprite2d
{
public:
	static void Draw(CRect const &,CRGBA const &);
	static void DrawRect(CRect const &,CRGBA const &);
};

class CFont
{
public:
	enum {
		REGULAR,
		PAGER,
		PRICEDOWN
	};
	// everything that is in my IDB
	static void Initialise(void);
	static void Shutdown(void);
	static void InitPerFrame(void);
	static void PrintChar(float,float,short);
	static void PrintString(float,float,ushort *);
	static void PrintString(float,float,ushort *,ushort *,float);
	static void ParseToken(ushort *,ushort *);
	static void DrawFonts(void);
	static void SetScale(float,float);
	static void SetColor(CRGBA*);
	static void SetJustifyOff(void);
	static void SetCentreOn(void);
	static void SetCentreOff(void);
	static void SetCentreSize(float);
	static void SetBackgroundOff(void);
	static void SetBackGroundOnlyTextOff(void);
	static void SetRightJustifyOn(void);
	static void SetPropOff(void);
	static void SetPropOn(void);
	static void SetFontStyle(short);
	static void SetRightJustifyWrap(float);
	static void SetDropShadowPosition(short);
	static double GetStringWidth(ushort *str, bool spc);
};

void AsciiToUnicode(char const *,ushort *);
void debug(const char *fmt, ...);

class CHud
{
public:
	static void Draw(void);
};

CEntity *FindPlayerPed(void);
CEntity *FindPlayerVehicle(void);
