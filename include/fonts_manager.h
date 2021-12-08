/*
* Created by Aen on 2021/7/15.
*/
#ifndef _FONTS_MANAGER_H
#define _FONTS_MANAGER_H
typedef struct Font_Bit_Map {
    int iXLeft;
    int iYTop;
    int iXMax;
    int iYMax;
    int iBpp;
    int iPitch;
    int iCurOriginX;
    int iCurOriginY;
    int iNextOriginX;
    int iNextOriginY;
    unsigned char *pucBuffer;
}T_Font_Bit_Map, *P_Font_Bit_Map;

typedef struct Font_Ops {
    char *name;
    int (*Font_init)(char *pcFontFile, unsigned int dwFontSize);
    int (*Get_font_bitmap)(unsigned int dwCode, P_Font_Bit_Map ptFontBitMap);
    void (*Set_font_size)(unsigned int dwFontSize);
    struct Font_Ops *p_next;
}T_Font_Ops, *P_Font_Ops;

int Register_font_ops(P_Font_Ops ptFontOpr);
void Show_font_ops(void);

int ASCII_init(void);
int GBK_init(void);
int Free_type_init(void);

int Fonts_init(void);
P_Font_Ops Get_font_ops(char *pcName);

void Set_font_size(unsigned int dwFontSize);
unsigned int Get_font_size(void);
int Get_font_bitmap(unsigned int dwCode, P_Font_Bit_Map ptFontBitMap);
int Set_fonts_detail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize);
#endif //_FONTS_MANAGER_H
