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
	processDebug();
}

void
dothing(void)
{
	((void (*)(void))0x491040)();
}

void
patchIII10(void)
{
	InterceptCall(&Render2dStuff_orig, Render2dStuff, 0x48E642);
/*
	Menu *m1 = new Menu();
	m1->parent = &toplevel;
	MenuEntry *me = new MenuEntry_Sub("submenu", m1);
	m1->appendEntry(new MenuEntry("foo"));
	m1->appendEntry(new MenuEntry("bar"));
	static int32 testint;
	m1->appendEntry(new MenuEntry_Int32("someint", &testint, nil, 2, -4, 10, nil));
	static const char *str[3] = { "x", "reallylongline woooh", "Val 3" };
	static int32 enumval = 4;
	m1->appendEntry(new MenuEntry_Int32("enumval", &enumval, nil, 1, 4, 6, str));
	m1->appendEntry(new MenuEntry_Cmd("do a thing", dothing));

	m1->appendEntry(new MenuEntry_UInt8("hour", (uint8*)0x95CDA6, nil, 1, 0, 23, nil));
	m1->appendEntry(new MenuEntry_Int16("old weather", (int16*)0x95CCEC, nil, 1, 0, 3, nil));
	m1->appendEntry(new MenuEntry_Int16("new weather", (int16*)0x95CC70, nil, 1, 0, 3, nil));
	m1->appendEntry(new MenuEntry_Float32("time scale", (float*)0x8F2C20, nil, 0.1f, 0.0f, 1.0f));
//	m1->insertEntry(new MenuEntry_Int16("someshort", &testshort, 2, -4, 10, nil));

	static int8 testint8 = 123;
	DebugMenuAddInt8("abc|qwer|foo", "testchar", &testint8, nil, 1, 0, 127, nil);

	toplevel.insertEntrySorted(me);
	toplevel.insertEntrySorted(new MenuEntry("test"));
	toplevel.insertEntrySorted(new MenuEntry("zxy"));
	toplevel.insertEntrySorted(new MenuEntry("text"));
	toplevel.insertEntrySorted(new MenuEntry("abc"));
	toplevel.insertEntrySorted(new MenuEntry("dddd"));
	static char xxx[100];
	for(int i = 0; i < 100; i++){
		sprintf(xxx, "zzz%d", i);
		toplevel.insertEntrySorted(new MenuEntry(xxx));
	}
*/
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if(reason == DLL_PROCESS_ATTACH){
		dllModule = hInst;

		AddressByVersion<uint32_t>(0, 0, 0, 0, 0, 0);
		if (gtaversion == III_10)
			patchIII10();
	}

	return TRUE;
}
