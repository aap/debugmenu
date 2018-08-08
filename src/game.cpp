#include "debugmenu.h"

// GTA

void **&RwEngineInst = *AddressByVersion<void***>(0x661228, 0x661228, 0x671248, 0x7870C0, 0x7870C8, 0x7860C8, 0xC97B24);

int &CTimer::m_snTimeInMilliseconds = *AddressByVersion<int*>(0x885B48, 0x885AF8, 0x895C38, 0x974B2C, 0x974B34, 0x973B34, 0xB7CB84);

CMouseControllerState &CPad::NewMouseControllerState = *(CMouseControllerState*)AddressByVersion<uint32_t>(0x8809F0, 0x8809A0, 0x890AE0, 0x94D788, 0x94D790, 0x94C790, 0xB73418);
CMouseControllerState &CPad::OldMouseControllerState = *(CMouseControllerState*)AddressByVersion<uint32_t>(0x8472A0, 0x8472A0, 0x8573E0, 0x936908, 0x936910, 0x935910, 0xB7342C);

static uint32_t CPad__GetPad_A = AddressByVersion<uint32_t>(0x492F60, 0x493020, 0x492FB0, 0x4AB060, 0x4AB080, 0x4AAF30, 0x53FB70);
WRAPPER CPad *CPad::GetPad(int id) { VARJMP(CPad__GetPad_A); }

static uint32_t CControllerConfigManager__GetIsKeyboardKeyDown_A = AddressByVersion<uint32_t>(0x58D2A0, 0x58D590, 0x58D480, 0x617E10, 0x617DF0, 0x617A30, 0x52DDB0);
WRAPPER bool CControllerConfigManager::GetIsKeyboardKeyDown(RsKeyCodes key) { VARJMP(CControllerConfigManager__GetIsKeyboardKeyDown_A); }
static uint32_t CControllerConfigManager__GetIsKeyboardKeyJustDown_A = AddressByVersion<uint32_t>(0x58D8A0, 0x58DB90, 0x58DA80, 0x617430, 0x617410, 0x617050, 0x52E450);
WRAPPER bool CControllerConfigManager::GetIsKeyboardKeyJustDown(RsKeyCodes key) { VARJMP(CControllerConfigManager__GetIsKeyboardKeyJustDown_A); }

CMousePointerStateHelper &MousePointerStateHelper = *AddressByVersion<CMousePointerStateHelper*>(0x95CC8C, 0x95CE44, 0x96CF84, 0xA10A4C, 0xA10A54, 0xA0FA54, 0xBA6744);

//
// from SkyGfx:
//

static uint32_t RwRasterCreate_A = AddressByVersion<uint32_t>(0x5AD930, 0x5ADBF0, 0x5B0580, 0x655490, 0x6554E0, 0x654440, 0x7FB230);
WRAPPER RwRaster *RwRasterCreate(RwInt32, RwInt32, RwInt32, RwInt32) { VARJMP(RwRasterCreate_A); }
static uint32_t RwRasterSetFromImage_A = AddressByVersion<uint32_t>(0x5BBF50, 0x5BC210, 0x5C0BF0, 0x6602B0, 0x660300, 0x65F260, 0x804290);
WRAPPER RwRaster *RwRasterSetFromImage(RwRaster*, RwImage*) { VARJMP(RwRasterSetFromImage_A); }

static uint32_t RwImageFindRasterFormat_A = AddressByVersion<uint32_t>(0x5BBF80, 0x5BC240, 0x5C0C40, 0x6602E0, 0x660330, 0x65F290, 0x8042C0);
WRAPPER RwImage *RwImageFindRasterFormat(RwImage*, RwInt32, RwInt32*, RwInt32*, RwInt32*, RwInt32*) { VARJMP(RwImageFindRasterFormat_A); }
static uint32_t RwImageDestroy_A = AddressByVersion<uint32_t>(0x5A9180, 0x5A9440, 0x5AB6A0, 0x6512B0, 0x651300, 0x650260, 0x802740);
WRAPPER RwBool RwImageDestroy(RwImage*) { VARJMP(RwImageDestroy_A); }

// Macros in newer RW
#ifndef GTASA
static uint32_t RwIm2DGetNearScreenZ_A = AddressByVersion<uint32_t>(0x5A43A0, 0x5A4660, 0x5A5340, 0x649B80, 0x649BD0, 0x648B30, 0);
WRAPPER RwReal RwIm2DGetNearScreenZ(void) { VARJMP(RwIm2DGetNearScreenZ_A); }
static uint32_t RwIm2DRenderIndexedPrimitive_A = AddressByVersion<uint32_t>(0x5A4440, 0x5A4700, 0x5A5440, 0x649C20, 0x649C70, 0x648BD0, 0);
WRAPPER RwBool RwIm2DRenderIndexedPrimitive(RwPrimitiveType, RwIm2DVertex*, RwInt32, RwImVertexIndex*, RwInt32) { VARJMP(RwIm2DRenderIndexedPrimitive_A); }

static uint32_t RwRenderStateGet_A = AddressByVersion<uint32_t>(0x5A4410, 0x5A46D0, 0x5A53B0, 0x649BF0, 0x649C40, 0x648BA0, 0);
WRAPPER RwBool RwRenderStateGet(RwRenderState, void*) { VARJMP(RwRenderStateGet_A); }
static uint32_t RwRenderStateSet_A = AddressByVersion<uint32_t>(0x5A43C0, 0x5A4680, 0x5A5360, 0x649BA0, 0x649BF0, 0x648B50, 0);
WRAPPER RwBool RwRenderStateSet(RwRenderState, void*) { VARJMP(RwRenderStateSet_A); }
#endif

// ADDRESS III and VC 1.0
static uint32_t RwImageCreate_A = AddressByVersion<uint32_t>(0x5A9120, 0x5A93E0, 0x5AB620, 0x651250, 0x6512A0, 0x650200, 0x8026E0);
WRAPPER RwImage *RwImageCreate(RwInt32, RwInt32, RwInt32) { VARJMP(RwImageCreate_A); }
static uint32_t RwImageAllocatePixels_A = AddressByVersion<uint32_t>(0x5A91E0, 0x5A94A0, 0x5AB710, 0x651310, 0x651360, 0x6502C0, 0x8027A0);
WRAPPER RwImage *RwImageAllocatePixels(RwImage *) { VARJMP(RwImageAllocatePixels_A); }
