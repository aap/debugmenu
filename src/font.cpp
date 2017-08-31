#include "debugmenu.h"

Font vga = { { nil, nil, nil }, 8, 16, 256 };
Font bios = { { nil, nil, nil }, 8, 8, 256 };
Font *curfont = &vga;

#define NUMCHARS 100
RwImVertexIndex indices[NUMCHARS*6];
RwIm2DVertex vertices[NUMCHARS*4];
int curVert;
int curIndex;

void
changeColors(RwImage *img, RwRGBA fg, RwRGBA bg)
{
	RwInt32 x, y;
	RwInt32 w = RwImageGetWidth(img);
	RwInt32 h = RwImageGetHeight(img);
	RwInt32 stride = RwImageGetStride(img);
	RwUInt8 *pixels = RwImageGetPixels(img);
	RwUInt8 *line, *p;
	assert(RwImageGetDepth(img) == 32);

	line = pixels;
	for(y = 0; y < h; y++){
		p = line;
		for(x = 0; x < w; x++){
			if(p[3]){
				p[0] = fg.red;
				p[1] = fg.green;
				p[2] = fg.blue;
				p[3] = fg.alpha;
			}else{
				p[0] = bg.red;
				p[1] = bg.green;
				p[2] = bg.blue;
				p[3] = bg.alpha;
			}
			p += 4;
		}
		line += stride;
	}
}

void
createDebugFont(int res, Font *font)
{
	RwInt32 w, h, d, flags;

	RwImage *img = readTGA(res);
	RwImageFindRasterFormat(img, rwRASTERTYPETEXTURE, &w, &h, &d, &flags);

	RwRGBA fg_inactive = { 200, 200, 200, 255 };
	RwRGBA bg_inactive = { 0, 0, 0, 0 };	// alpha must be 0
	changeColors(img, fg_inactive, bg_inactive);

	font->rasters[0] = RwRasterCreate(w, h, d, flags);
	font->rasters[0] = RwRasterSetFromImage(font->rasters[0], img);
	assert(font->rasters[0]);

	RwRGBA fg_inactiveSelect = { 255, 255, 255, 255 };
	RwRGBA bg_inactiveSelect = { 0, 0, 0, 0 };	// alpha must be 0
	changeColors(img, fg_inactiveSelect, bg_inactiveSelect);

	font->rasters[1] = RwRasterCreate(w, h, d, flags);
	font->rasters[1] = RwRasterSetFromImage(font->rasters[1], img);
	assert(font->rasters[1]);

	RwRGBA fg_activeSelect = { 200, 200, 200, 255 };
	RwRGBA bg_activeSelect = { 132, 132, 132, 255 };
	changeColors(img, fg_activeSelect, bg_activeSelect);

	font->rasters[2] = RwRasterCreate(w, h, d, flags);
	font->rasters[2] = RwRasterSetFromImage(font->rasters[2], img);
	assert(font->rasters[2]);

	RwImageDestroy(img);
}

void
createDebugFonts(void)
{
	createDebugFont(IDR_FONT_BIOS, &bios);
	createDebugFont(IDR_FONT_VGA, &vga);
}

Pt
fontPrint(const char *s, float xstart, float ystart, int style)
{
	char c;
	RwCamera *cam;
	RwRaster *raster;
	RwIm2DVertex *vert;
	RwImVertexIndex *ix;
	float u, v, du, dv;
	float x, y;
	Pt sz;
	int szx;

	sz.y = curfont->glyphheight;
	sz.x = 0;
	szx = 0;

	x = xstart;
	y = ystart;
	raster = curfont->rasters[style];
	curVert = 0;
	curIndex = 0;
	cam = (RwCamera*)RWSRCGLOBAL(curCamera);
	vert = &vertices[curVert];
	ix = &indices[curIndex];
	du = curfont->glyphwidth/(float)raster->width;
	dv = curfont->glyphheight/(float)raster->height;
	float recipz = 1.0f/RwCameraGetNearClipPlane(cam);
	while(c = *s++){
		if(c == '\n'){
			x = xstart;
			y += curfont->glyphheight;
			sz.y = curfont->glyphheight;
			if(szx > sz.x)
				sz.x = szx;
			szx = 0;
			continue;
		}

		if(c >= curfont->numglyphs)
			c = 0;
		u = (c % 16)*curfont->glyphwidth / (float)raster->width;
		v = (c / 16)*curfont->glyphheight / (float)raster->height;

		RwIm2DVertexSetScreenX(vert, x);
		RwIm2DVertexSetScreenY(vert, y);
		RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
		RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
		RwIm2DVertexSetRecipCameraZ(vert, recipz);
		RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
		RwIm2DVertexSetU(vert, u, recipz);
		RwIm2DVertexSetV(vert, v, recipz);
		vert++;

		RwIm2DVertexSetScreenX(vert, x+curfont->glyphwidth);
		RwIm2DVertexSetScreenY(vert, y);
		RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
		RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
		RwIm2DVertexSetRecipCameraZ(vert, recipz);
		RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
		RwIm2DVertexSetU(vert, u+du, recipz);
		RwIm2DVertexSetV(vert, v, recipz);
		vert++;

		RwIm2DVertexSetScreenX(vert, x);
		RwIm2DVertexSetScreenY(vert, y+curfont->glyphheight);
		RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
		RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
		RwIm2DVertexSetRecipCameraZ(vert, recipz);
		RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
		RwIm2DVertexSetU(vert, u, recipz);
		RwIm2DVertexSetV(vert, v+dv, recipz);
		vert++;

		RwIm2DVertexSetScreenX(vert, x+curfont->glyphwidth);
		RwIm2DVertexSetScreenY(vert, y+curfont->glyphheight);
		RwIm2DVertexSetScreenZ(vert, RwIm2DGetNearScreenZ());
		RwIm2DVertexSetCameraZ(vert, RwCameraGetNearClipPlane(cam));
		RwIm2DVertexSetRecipCameraZ(vert, recipz);
		RwIm2DVertexSetIntRGBA(vert, 255, 255, 255, 255);
		RwIm2DVertexSetU(vert, u+du, recipz);
		RwIm2DVertexSetV(vert, v+dv, recipz);
		vert++;
		*ix++ = curVert;
		*ix++ = curVert+1;
		*ix++ = curVert+2;
		*ix++ = curVert+2;
		*ix++ = curVert+1;
		*ix++ = curVert+3;

		curVert += 4;
		curIndex += 6;
		x += curfont->glyphwidth;
		szx += curfont->glyphwidth;
	}
	if(szx > sz.x)
		sz.x = szx;
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, vertices, curVert, indices, curIndex);
	return sz;
}

Pt
fontGetStringSize(const char *s)
{
	Pt sz = { 0, 0 };
	int x;
	char c;
	sz.y = curfont->glyphheight;	// always assume one line;
	x = 0;
	while(c = *s++){
		if(c == '\n'){
			sz.y += curfont->glyphheight;
			if(x > sz.x)
				sz.x = x;
			x = 0;
		}else
			x += curfont->glyphwidth;
	}
	if(x > sz.x)
		sz.x = x;
	return sz;
}

int
fontGetLen(int len)
{
	return len*curfont->glyphwidth;
}