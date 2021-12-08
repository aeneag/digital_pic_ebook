/*
* Created by Aen on 2021/7/15.
*/
#include <config.h>
#include <fonts_manager.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

static int FreeType_font_init(char *pcFontFile, unsigned int dwFontSize);
static int FreeType_get_font_bitmap(unsigned int dwCode, P_Font_Bit_Map ptFontBitMap);

static T_Font_Ops g_t_FreeType_font_ops = {
        .name               = "freetype",
        .Font_init          = FreeType_font_init,
        .Get_font_bitmap    = FreeType_get_font_bitmap,
};

static FT_Library g_tLibrary;
static FT_Face g_tFace;
static FT_GlyphSlot g_tSlot;

static int FreeType_font_init(char *pcFontFile, unsigned int dwFontSize){
    int iError;

    iError = FT_Init_FreeType(&g_tLibrary );			   /* initialize library */
    /* error handling omitted */
    if (iError)
    {
        DBG_PRINTF("FT_Init_FreeType failed\n");
        return -1;
    }

    iError = FT_New_Face(g_tLibrary, pcFontFile, 0, &g_tFace); /* create face object */
    /* error handling omitted */
    if (iError)
    {
        DBG_PRINTF("FT_Init_FreeType failed\n");
        return -1;
    }

    g_tSlot = g_tFace->glyph;

    iError = FT_Set_Pixel_Sizes(g_tFace, dwFontSize, 0);
    if (iError)
    {
        DBG_PRINTF("FT_Set_Pixel_Sizes failed : %d\n", dwFontSize);
        return -1;
    }


    return 0;
}
static int FreeType_get_font_bitmap(unsigned int dwCode, P_Font_Bit_Map ptFontBitMap){
    int iError;
    int iPenX = ptFontBitMap->iCurOriginX;
    int iPenY = ptFontBitMap->iCurOriginY;

    iError = FT_Load_Char(g_tFace, dwCode, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
    if (iError)
    {
        DBG_PRINTF("FT_Load_Char error for code : 0x%x\n", dwCode);
        return -1;
    }

    ptFontBitMap->iXLeft    = iPenX + g_tSlot->bitmap_left;
    ptFontBitMap->iYTop     = iPenY - g_tSlot->bitmap_top;
    ptFontBitMap->iXMax     = ptFontBitMap->iXLeft + g_tSlot->bitmap.width;
    ptFontBitMap->iYMax     = ptFontBitMap->iYTop  + g_tSlot->bitmap.rows;
    ptFontBitMap->iBpp      = 1;
    ptFontBitMap->iPitch    = g_tSlot->bitmap.pitch;
    ptFontBitMap->pucBuffer = g_tSlot->bitmap.buffer;

    ptFontBitMap->iNextOriginX = iPenX + g_tSlot->advance.x / 64;
    ptFontBitMap->iNextOriginY = iPenY;

    return 0;
}
int Free_type_init(void){
    return Register_font_ops(&g_t_FreeType_font_ops);
}
