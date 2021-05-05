/*	Font-drawing engine
	Version:	1.23
	Last updated:	2019/01/11	*/

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <kernel.h>
#include <wchar.h>

#include <libgs.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ADVANCES_H

#include "graphics.h"
#include "font.h"

/*	Draw the glyphs as close as possible to each other, to save VRAM.

	C: character
	X: Horizontal frontier
	Y: Vertical frontier
	I: Initial state, no frontier.

	Initial state:
		IIII
		IIII
		IIII
		IIII

	After one character is added to an empty atlas, initialize the X and Y frontier.
		CXXX
		YXXX
		YXXX
		YXXX

	After one more character is added:
		CCXX
		YYXX
		YYXX
		YYXX

	After one more character is added:
		CCCX
		YYYX
		YYYX
		YYYX

	After one more character is added:
		CCCC
		YYYY
		YYYY
		YYYY

	After one more character is added:
		CCCC
		CXXX
		YXXX
		YXXX	*/

struct FontGlyphSlotInfo {
	struct FontGlyphSlot *slot;
	u32 vram;	//Address of the atlas buffer in VRAM
};

struct FontGlyphSlot{
	wint_t character;
	unsigned short int VramPageX, VramPageY;
	short int top, left;	//Offsets to place the glyph at. Refer to the FreeType documentation.
	short int advance_x, advance_y;
	unsigned short int width, height;
};

struct FontFrontier{
	short int x, y;
	short int width, height;
};

struct FontAtlas{
	u32 vram;	//Address of the buffer in VRAM
	void *buffer;	//Address of the buffer in local memory

	unsigned int NumGlyphs;
	struct FontGlyphSlot *GlyphSlot;
	struct FontFrontier frontier[2];
};

typedef struct Font{
	FT_Face FTFace;
	GS_IMAGE Texture;
	GS_IMAGE Clut;
	void *ClutMem;
	unsigned short int IsLoaded;
	unsigned short int IsInit;
	struct FontAtlas atlas[FNT_MAX_ATLASES];
} Font_t;

static FT_Library FTLibrary;
static Font_t GS_FTFont;
static Font_t GS_sub_FTFont;

static int ResetThisFont(struct UIDrawGlobal *gsGlobal, Font_t *font)
{
	struct FontAtlas *atlas;
	unsigned short int i;
	int result, vram;

	result=0;

	font->Clut.x = 0;
	font->Clut.y = 0;
	font->Clut.width = 16;
	font->Clut.height = 16;
	font->Clut.vram_width = 1;	//width (16) / 64 = 1

	vram = GsVramAllocTextureBuffer(font->Clut.width, font->Clut.height, font->Clut.psm);

	if (vram >= 0)
	{
		font->Clut.vram_addr = (u16)vram;

		UploadClut(gsGlobal, &font->Clut, font->ClutMem);

		font->Texture.x = 0;
		font->Texture.y = 0;
		for(i=0,atlas=font->atlas; i<FNT_MAX_ATLASES; i++,atlas++)
		{
			atlas->frontier[0].width = 0;
			atlas->frontier[0].height = 0;
			atlas->frontier[1].width = 0;
			atlas->frontier[1].height = 0;
			atlas->vram = -1;
			if(atlas->GlyphSlot != NULL)
			{
				free(atlas->GlyphSlot);
				atlas->GlyphSlot = NULL;
			}
			if(atlas->buffer != NULL)
			{
				free(atlas->buffer);
				atlas->buffer = NULL;
			}
			atlas->NumGlyphs = 0;
		}
	} else {
		printf("Font: error - unable to allocate VRAM for CLUT.\n");
		result = -1;
	}

	return result;
}

int FontReset(struct UIDrawGlobal *gsGlobal)
{
	int result;

	if((result = ResetThisFont(gsGlobal, &GS_FTFont)) == 0)
		result = ResetThisFont(gsGlobal, &GS_sub_FTFont);

	return 0;
}

static int InitFontSupportCommon(struct UIDrawGlobal *gsGlobal, Font_t *font)
{
	int result;
	unsigned short int i;

	if((result=FT_Set_Pixel_Sizes(font->FTFace, FNT_CHAR_WIDTH, FNT_CHAR_HEIGHT))==0)
	{
		font->Texture.width = FNT_ATLAS_WIDTH;
		font->Texture.height = FNT_ATLAS_HEIGHT;
		font->Texture.psm = GS_TEX_8;
		font->Clut.psm = GS_CLUT_32;

		// generate the clut table
		u32 *clut = memalign(64, 256 * 4);
		font->ClutMem=clut;
		for (i = 0; i < 256; ++i) clut[i] = (i > 0x80 ? 0x80 : i) << 24 | i << 16 | i << 8 | i;
		SyncDCache(clut, (void*)((unsigned int)clut+256*4));

		if(!font->IsInit)
			ResetThisFont(gsGlobal, font);
		font->IsLoaded=1;
		font->IsInit=1;
	}

	return result;
}

int FontInit(struct UIDrawGlobal *gsGlobal, const char *FontFile)
{
	int result;

	if((result=FT_Init_FreeType(&FTLibrary))==0)
	{
		if((result=FT_New_Face(FTLibrary, FontFile, 0, &GS_FTFont.FTFace))==0)
			result=InitFontSupportCommon(gsGlobal, &GS_FTFont);

		if(result!=0)
			FontDeinit();
	}

	return result;
}

int FontInitWithBuffer(struct UIDrawGlobal *gsGlobal, void *buffer, unsigned int size)
{
	int result;

	if((result=FT_Init_FreeType(&FTLibrary))==0)
	{
		if((result=FT_New_Memory_Face(FTLibrary, buffer, size, 0, &GS_FTFont.FTFace))==0)
			result=InitFontSupportCommon(gsGlobal, &GS_FTFont);

		if(result!=0)
			FontDeinit();
	}

	return result;
}

static void UnloadFont(Font_t *font)
{
	struct FontAtlas *atlas;
	unsigned int i;

	if(font->IsLoaded)
		FT_Done_Face(font->FTFace);

	for(i=0,atlas=font->atlas; i<FNT_MAX_ATLASES; i++,atlas++)
	{
		if(atlas->buffer!=NULL)
		{
			free(atlas->buffer);
			atlas->buffer = NULL;
		}
		if(atlas->GlyphSlot!=NULL)
		{
			free(atlas->GlyphSlot);
			atlas->GlyphSlot = NULL;
		}
	}
	font->atlas->NumGlyphs = 0;

	if(font->ClutMem != NULL)
	{
		free(font->ClutMem);
		font->ClutMem = NULL;
	}

	font->IsLoaded = 0;
}

int AddSubFont(struct UIDrawGlobal *gsGlobal, const char *FontFile)
{
	int result;

	if((result = FT_New_Face(FTLibrary, FontFile, 0, &GS_sub_FTFont.FTFace)) == 0)
		result = InitFontSupportCommon(gsGlobal, &GS_sub_FTFont);

	if(result != 0)
		UnloadFont(&GS_sub_FTFont);

	return result;
}

int AddSubFontWithBuffer(struct UIDrawGlobal *gsGlobal, void *buffer, unsigned int size)
{
	int result;

	if((result = FT_New_Memory_Face(FTLibrary, buffer, size, 0, &GS_sub_FTFont.FTFace)) == 0)
		result = InitFontSupportCommon(gsGlobal, &GS_sub_FTFont);

	if(result != 0)
		UnloadFont(&GS_sub_FTFont);

	return result;
}

//VRAM stays allocated.
void FontDeinit(void)
{
	UnloadFont(&GS_sub_FTFont);
}

static int AtlasInit(Font_t *font, struct FontAtlas *atlas)
{
	unsigned int TextureSizeEE, i;
	short int width_aligned, height_aligned;
	int result;

	result = 0;
	width_aligned = (font->Texture.width+127)&~127;
	height_aligned = (font->Texture.height+127)&~127;
	font->Texture.vram_width = width_aligned / 64;
	if((atlas->vram = GsVramAllocTextureBuffer(width_aligned, height_aligned, font->Texture.psm)) >= 0)
	{
		TextureSizeEE = width_aligned * height_aligned;
		if((atlas->buffer = memalign(64, TextureSizeEE)) == NULL)
		{
			printf("Font: error - unable to allocate memory for atlas.\n");
			result = -ENOMEM;
		}
		memset(atlas->buffer, 0, TextureSizeEE);
		SyncDCache(atlas->buffer, atlas->buffer+TextureSizeEE);
	}
	else
	{
		printf("Font: error - unable to allocate VRAM for atlas.\n");
		result = -ENOMEM;
	}

	return result;
}

static struct FontGlyphSlot *AtlasAlloc(Font_t *font, struct FontAtlas *atlas, short int width, short int height)
{
	struct FontGlyphSlot *GlyphSlot;

	GlyphSlot = NULL;
	if(atlas->buffer == NULL)
	{	//No frontier (empty atlas)
		if(AtlasInit(font, atlas) != 0)
			return NULL;

		//Give the glyph 1px more, for texel rendering.
		atlas->frontier[0].width = FNT_ATLAS_WIDTH - (width + 1);
		atlas->frontier[0].height = FNT_ATLAS_HEIGHT;
		atlas->frontier[0].x = width + 1;
		atlas->frontier[0].y = 0;
		atlas->frontier[1].width = FNT_ATLAS_WIDTH;
		atlas->frontier[1].height = FNT_ATLAS_HEIGHT - (height + 1);
		atlas->frontier[1].x = 0;
		atlas->frontier[1].y = height + 1;

		atlas->NumGlyphs++;

		if((atlas->GlyphSlot = realloc(atlas->GlyphSlot, atlas->NumGlyphs * sizeof(struct FontGlyphSlot))) != NULL)
		{
			GlyphSlot = &atlas->GlyphSlot[atlas->NumGlyphs - 1];

			GlyphSlot->VramPageX = 0;
			GlyphSlot->VramPageY = 0;
		} else {
			printf("Font: error - unable to allocate a new glyph slot.\n");
			atlas->NumGlyphs = 0;
		}

		return GlyphSlot;
	} else {	//We have the frontiers
		//Try to allocate from the horizontal frontier first.
		if((atlas->frontier[0].width >= width + 1)
		   && (atlas->frontier[0].height >= height + 1))
		{
			atlas->NumGlyphs++;

			if((atlas->GlyphSlot = realloc(atlas->GlyphSlot, atlas->NumGlyphs * sizeof(struct FontGlyphSlot))) != NULL)
			{
				GlyphSlot = &atlas->GlyphSlot[atlas->NumGlyphs - 1];

				GlyphSlot->VramPageX = atlas->frontier[0].x;
				GlyphSlot->VramPageY = atlas->frontier[0].y;

				//Give the glyph 1px more, for texel rendering.
				//Update frontier.
				atlas->frontier[0].width -= width + 1;
				atlas->frontier[0].x += width + 1;

				//If the new glyph is a little taller than the glyphs under the horizontal frontier, move the vertical frontier.
				if(atlas->frontier[0].y + height + 1 > atlas->frontier[1].y)
				{
					atlas->frontier[1].y = atlas->frontier[0].y + height + 1;
					atlas->frontier[1].height = FNT_ATLAS_HEIGHT - atlas->frontier[1].y;
				}
			} else {
				printf("Font: error - unable to allocate a new glyph slot.\n");
				atlas->NumGlyphs = 0;
			}
		//Now try the vertical frontier.
		} else if((atlas->frontier[1].width >= width + 1)
			  && (atlas->frontier[1].height >= height + 1))
		{
			atlas->NumGlyphs++;

			if((atlas->GlyphSlot = realloc(atlas->GlyphSlot, atlas->NumGlyphs * sizeof(struct FontGlyphSlot))) != NULL)
			{
				GlyphSlot = &atlas->GlyphSlot[atlas->NumGlyphs - 1];

				GlyphSlot->VramPageX = atlas->frontier[1].x;
				GlyphSlot->VramPageY = atlas->frontier[1].y;

				//Give the glyph 1px more, for texel rendering.
				/*	Update frontier.
					If we got here, it means that the horizontal frontier is very close the edge of VRAM.
					Give a large portion of the space recorded under this frontier to the horizontal frontier.

					Before:		After one more character is added:
						CCCC		CCCC
						YYYY		CXXX
						YYYY		YXXX
						YYYY		YXXX	*/
				atlas->frontier[0].x = width + 1;
				atlas->frontier[0].y = atlas->frontier[1].y;
				atlas->frontier[0].width = FNT_ATLAS_WIDTH - (width + 1);
				atlas->frontier[0].height = atlas->frontier[1].height;
				atlas->frontier[1].height -= height + 1;
				atlas->frontier[1].y += height + 1;
			} else {
				printf("Font: error - unable to allocate a new glyph slot.\n");
				atlas->NumGlyphs = 0;
			}
		}
	}

	return GlyphSlot;
}

static void AtlasCopyFT(Font_t *font, struct FontAtlas *atlas, struct FontGlyphSlot *GlyphSlot, FT_GlyphSlot FT_GlyphSlot)
{
	short int yOffset;
	unsigned char *FTCharRow;

	for(yOffset=0; yOffset<FT_GlyphSlot->bitmap.rows; yOffset++)
	{
		FTCharRow=(void*)UNCACHED_SEG(((unsigned int)atlas->buffer+GlyphSlot->VramPageX+(GlyphSlot->VramPageY+yOffset)*font->Texture.width));
		memcpy(FTCharRow, &((unsigned char*)FT_GlyphSlot->bitmap.buffer)[yOffset*FT_GlyphSlot->bitmap.width], FT_GlyphSlot->bitmap.width);
	}
}

static struct FontGlyphSlot *UploadGlyph(struct UIDrawGlobal *gsGlobal, Font_t *font, wint_t character, FT_GlyphSlot FT_GlyphSlot, struct FontAtlas **AtlasOut)
{
	struct FontAtlas *atlas;
	struct FontGlyphSlot *GlyphSlot;
	unsigned short int i;

	*AtlasOut=NULL;
	GlyphSlot = NULL;
	for(i = 0,atlas=font->atlas; i < FNT_MAX_ATLASES; i++,atlas++)
	{
		if((GlyphSlot = AtlasAlloc(font, atlas, FT_GlyphSlot->bitmap.width, FT_GlyphSlot->bitmap.rows)) != NULL)
			break;
	}

	if(GlyphSlot != NULL)
	{
		GlyphSlot->character = character;
		GlyphSlot->left = FT_GlyphSlot->bitmap_left;
		GlyphSlot->top = FT_GlyphSlot->bitmap_top;
		GlyphSlot->width = FT_GlyphSlot->bitmap.width;
		GlyphSlot->height = FT_GlyphSlot->bitmap.rows;
		GlyphSlot->advance_x = FT_GlyphSlot->advance.x >> 6;
		GlyphSlot->advance_y = FT_GlyphSlot->advance.y >> 6;

		*AtlasOut = atlas;

		//Initiate a texture flush before reusing the VRAM page, if the slot was just used earlier.
		GsTextureFlush();

		AtlasCopyFT(font, atlas, GlyphSlot, FT_GlyphSlot);
		font->Texture.vram_addr = atlas->vram;
		GsLoadImage(atlas->buffer, &font->Texture);
	} else
		printf("Font: error - all atlas are full.\n");

	return GlyphSlot;
}

static int GetGlyph(struct UIDrawGlobal *gsGlobal, Font_t *font, wint_t character, int DrawMissingGlyph, struct FontGlyphSlotInfo *glyphInfo)
{
	int i, slot;
	struct FontAtlas *atlas;
	struct FontGlyphSlot *glyphSlot;
	FT_UInt glyphIndex;

	//Scan through all uploaded glyph slots.
	for(i=0,atlas=font->atlas; i<FNT_MAX_ATLASES; i++,atlas++)
	{
		for(slot=0; slot < atlas->NumGlyphs; slot++)
		{
			if(atlas->GlyphSlot[slot].character==character)
			{
				glyphInfo->slot = &atlas->GlyphSlot[slot];
				glyphInfo->vram = atlas->vram;
				return 0;
			}
		}
	}

	//Not in VRAM? Upload it.
	if((glyphIndex = FT_Get_Char_Index(font->FTFace, character)) != 0 || DrawMissingGlyph)
	{
		if(FT_Load_Glyph(font->FTFace, glyphIndex, FT_LOAD_RENDER))
			return -1;

		if((glyphSlot = UploadGlyph(gsGlobal, font, character, font->FTFace->glyph, &atlas)) == NULL)
			return -1;

//		printf("Uploading %c, %u, %u\n", character, GlyphSlot->VramPageX, GlyphSlot->VramPageY);

		glyphInfo->slot = glyphSlot;
		glyphInfo->vram = atlas->vram;
		return 0;
	} else //Otherwise, the glyph is missing from font
		return 1;

	return -1;
}

static int DrawGlyph(struct UIDrawGlobal *gsGlobal, Font_t *font, wint_t character, short int x, short int y, short int z, float scale, GS_RGBAQ colour, int DrawMissingGlyph, short int *width)
{
	struct FontGlyphSlot *glyphSlot;
	struct FontGlyphSlotInfo glyphInfo;
	struct FontAtlas *atlas;
	unsigned short int i, slot;
	short int XCoordinates, YCoordinates;
	int result;

	if(font->IsLoaded)
	{
		if ((result = GetGlyph(gsGlobal, font, character, DrawMissingGlyph, &glyphInfo)) != 0)
			return result;

		glyphSlot = glyphInfo.slot;
		font->Texture.vram_addr = glyphInfo.vram;

		YCoordinates=y+FNT_CHAR_HEIGHT*scale-glyphSlot->top*scale;
		XCoordinates=x+glyphSlot->left*scale;
		DrawSpriteTexturedClut(gsGlobal, &font->Texture, &font->Clut,
						XCoordinates, YCoordinates,			//x1, y1
						glyphSlot->VramPageX, glyphSlot->VramPageY,	//u1, v1
						XCoordinates+glyphSlot->width*scale, YCoordinates+glyphSlot->height*scale,	//x2, y2
						glyphSlot->VramPageX+glyphSlot->width, glyphSlot->VramPageY+glyphSlot->height,	//u2, v2
						z, colour);

		*width = glyphSlot->advance_x * scale;
	} else	//Not loaded
		return -1;

	return 0;
}

void FontPrintfWithFeedback(struct UIDrawGlobal *gsGlobal, short x, short int y, short int z, float scale, GS_RGBAQ colour, const char *string, short int *xRel, short int *yRel)
{
	wchar_t wchar;
	short int StartX, StartY, width;
	int charsize, bufmax;

	StartX = x;
	StartY = y;

	for(bufmax = strlen(string) + 1; *string != '\0'; string += charsize, bufmax -= charsize)
	{
		//Up to MB_CUR_MAX
		charsize = mbtowc(&wchar, string, bufmax);

		switch(wchar)
		{
			case '\r':
				x=StartX;
				break;
			case '\n':
				y+=(short int)(FNT_CHAR_HEIGHT*scale);
				x=StartX;
				break;
			case '\t':
				x+=(short int)(FNT_TAB_STOPS*FNT_CHAR_WIDTH*scale)-(unsigned int)x%(unsigned int)(FNT_TAB_STOPS*FNT_CHAR_WIDTH*scale);
				break;
			default:
				width = 0;
				if(DrawGlyph(gsGlobal, &GS_FTFont, wchar, x, y, z, scale, colour, 0, &width) != 0)
				{
					if(DrawGlyph(gsGlobal, &GS_sub_FTFont, wchar, x, y, z, scale, colour, 0, &width) != 0)
					{	//Cannot locate the glyph, so draw the missing glyph character.
						DrawGlyph(gsGlobal, &GS_FTFont, wchar, x, y, z, scale, colour, 1, &width);
					}
				}

				x += width;
		}
	}

	if(xRel != NULL)
		*xRel = x - StartX;
	if(yRel != NULL)
		*yRel = y - StartY;
}

void FontPrintf(struct UIDrawGlobal *gsGlobal, short int x, short int y, short int z, float scale, GS_RGBAQ colour, const char *string)
{
	return FontPrintfWithFeedback(gsGlobal, x, y, z, scale, colour, string, NULL, NULL);
}

static int GetGlyphWidth(struct UIDrawGlobal *gsGlobal, Font_t *font, wint_t character, int DrawMissingGlyph)
{
	struct FontGlyphSlotInfo glyphInfo;
	int result;

	if(font->IsLoaded)
	{	//Calling FT_Get_Advance is slow when I/O is slow, hence a cache is required. Here, the atlas is used as a cache.
		if ((result = GetGlyph(gsGlobal, font, character, DrawMissingGlyph, &glyphInfo)) != 0)
			return result;

		return glyphInfo.slot->advance_x;
	}

	return 0;
}

int FontGetGlyphWidth(struct UIDrawGlobal *gsGlobal, wint_t character)
{
	int width;

	if((width = GetGlyphWidth(gsGlobal, &GS_FTFont, character, 0)) == 0)
	{
		if((width = GetGlyphWidth(gsGlobal, &GS_sub_FTFont, character, 0)) == 0)
		{
			width = GetGlyphWidth(gsGlobal, &GS_FTFont, character, 1);
		}
	}
}
