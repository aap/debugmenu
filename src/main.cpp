#include "debugmenu.h"

HMODULE dllModule;
int gtaversion = -1;

CControllerConfigManager *ctrldummy = nil;

/*
 * TGA reader
 */

#pragma pack(push, 1)
struct TGAHeader
{
	RwInt8  IDlen;
	RwInt8  colorMapType;
	RwInt8  imageType;
	RwInt16 colorMapOrigin;
	RwInt16 colorMapLength;
	RwInt8  colorMapDepth;
	RwInt16 xOrigin, yOrigin;
	RwInt16 width, height;
	RwUInt8 depth;
	RwUInt8 descriptor;
};
#pragma pack(pop)

RwImage*
readTGA(uint8 *p)
{
	RwImage *image;

	TGAHeader header;
	int depth = 0, palDepth = 0;
	memcpy(&header, p, sizeof(header));
	p += sizeof(header);
	
	assert(header.imageType == 1 || header.imageType == 2);
	p += header.IDlen;
	if(header.colorMapType){
		assert(header.colorMapOrigin == 0);
		depth = (header.colorMapLength <= 16) ? 4 : 8;
		palDepth = header.colorMapDepth;
		assert(palDepth == 24 || palDepth == 32);
	}else{
		depth = header.depth;
		assert(depth == 24 || depth == 32);
	}

	image = RwImageCreate(header.width, header.height, depth == 24 ? 32 : depth);
	RwImageAllocatePixels(image);
	RwRGBA *palette = header.colorMapType ? image->palette : NULL;
	if(palette){
		int maxlen = depth == 4 ? 16 : 256;
		int i;
		for(i = 0; i < header.colorMapLength; i++){
			palette[i].blue = *p++;
			palette[i].green = *p++;
			palette[i].red = *p++;
			palette[i].alpha = 0xFF;
			if(palDepth == 32)
				palette[i].alpha = *p++;
		}
		for(; i < maxlen; i++){
			palette[i].red = palette[i].green = palette[i].blue = 0;
			palette[i].alpha = 0xFF;
		}
	}

	RwUInt8 *pixels = image->cpPixels;
	if(!(header.descriptor & 0x20))
		pixels += (image->height-1)*image->stride;
	for(int y = 0; y < image->height; y++){
		RwUInt8 *line = pixels;
		for(int x = 0; x < image->width; x++){
			if(palette)
				*line++ = *p++;
			else{
				line[2] = *p++;
				line[1] = *p++;
				line[0] = *p++;
				line += 3;
				if(depth == 32)
					*line++ = *p++;
				if(depth == 24)
					*line++ = 0xFF;
			}
		}
		pixels += (header.descriptor&0x20) ?
		              image->stride : -image->stride;
	}
	return image;
}

RwImage*
readTGA(const char *filename)
{
	RwImage *img;
	FILE *file = fopen(filename, "rb");
	fseek(file, 0, 2);
	int len = ftell(file);
	uint8 *data = (uint8*)malloc(len);
	fseek(file, 0, 0);
	fread(data, 1, len, file);
	fclose(file);
	img = readTGA(data);
	free(data);
	return img;
}

RwImage*
readTGA(int res)
{
	RwImage *img;
	HRSRC resource = FindResource(dllModule, MAKEINTRESOURCE(res), RT_RCDATA);
	uint8 *data = (uint8*)LoadResource(dllModule, resource);
	img = readTGA(data);
	FreeResource(data);
	return img;
}


/*
 * Debug menu
 */

void (*Render2dStuff_orig)(void);
void
Render2dStuff(void)
{
	Render2dStuff_orig();
	DebugMenuRender();
}

void (*CCutsceneMgr__Update_orig)(void);
void
CCutsceneMgr__Update(void)
{
	DebugMenuProcess();
	CCutsceneMgr__Update_orig();
}

void
patchSA10(void)
{
	InterceptCall(&Render2dStuff_orig, Render2dStuff, 0x53EB12);
	InterceptCall(&CCutsceneMgr__Update_orig, CCutsceneMgr__Update, 0x53BF28);
}

void
patchVC10(void)
{
	InterceptCall(&Render2dStuff_orig, Render2dStuff, 0x4A608E);
	InterceptCall(&CCutsceneMgr__Update_orig, CCutsceneMgr__Update, 0x4A4417);
}

void
patchIII10(void)
{
	InterceptCall(&Render2dStuff_orig, Render2dStuff, 0x48E642);
	InterceptCall(&CCutsceneMgr__Update_orig, CCutsceneMgr__Update, 0x48C888);
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if(reason == DLL_PROCESS_ATTACH){
		dllModule = hInst;

		AddressByVersion<uint32_t>(0, 0, 0, 0, 0, 0, 0);
#if defined (GTA3)
		if(gtaversion == III_10)
			patchIII10();
#elif defined (GTAVC)
		if(gtaversion == VC_10)
			patchVC10();
#elif defined (GTASA)
		if(gtaversion == SA_10)
			patchSA10();
#endif
	}

	return TRUE;
}
