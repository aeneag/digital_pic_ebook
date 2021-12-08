/*
* Created by Aen on 2021/7/15.
*/
#include <config.h>
#include "fonts_manager.h"
#include <string.h>

static P_Font_Ops g_pt_font_ops_head = NULL;
static int g_dwFontSize;
int Register_font_ops(P_Font_Ops ptFontOpr){
    P_Font_Ops ptTmp;

    if (!g_pt_font_ops_head)
    {
        g_pt_font_ops_head   = ptFontOpr;
        ptFontOpr->p_next = NULL;
    }
    else
    {
        ptTmp = g_pt_font_ops_head;
        while (ptTmp->p_next)
        {
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next     = ptFontOpr;
        ptFontOpr->p_next = NULL;
    }

    return 0;
}
void Show_font_ops(void){
    int i = 0;
    P_Font_Ops ptTmp = g_pt_font_ops_head;

    while (ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}

int Fonts_init(void){
    int iError;
#if 0
    iError = ASCII_init();
    if (iError)
    {
        DBG_PRINTF("ASCIIInit error!\n");
        return -1;
    }

    iError = GBK_init();
    if (iError)
    {
        DBG_PRINTF("GBKInit error!\n");
        return -1;
    }
#endif
    iError = Free_type_init();
    if (iError)
    {
        DBG_PRINTF("FreeTypeInit error!\n");
        return -1;
    }

    return 0;
}
P_Font_Ops Get_font_ops(char *pcName){
    P_Font_Ops ptTmp = g_pt_font_ops_head;

    while (ptTmp)
    {
        if (strcmp(ptTmp->name, pcName) == 0)
        {
            return ptTmp;
        }
        ptTmp = ptTmp->p_next;
    }
    return NULL;
}
void Set_font_size(unsigned int dwFontSize)
{
    P_Font_Ops ptTmp = g_pt_font_ops_head;

    g_dwFontSize = dwFontSize;

    while (ptTmp)
    {
        if (ptTmp->Set_font_size)
        {
            ptTmp->Set_font_size(dwFontSize);
        }
        ptTmp = ptTmp->p_next;
    }
}
unsigned int Get_font_size(void)
{
    return g_dwFontSize;
}
int Get_font_bitmap(unsigned int dwCode, P_Font_Bit_Map ptFontBitMap)
{
    int iError = 0;
    P_Font_Ops ptTmp = g_pt_font_ops_head;

    while (ptTmp)
    {
        iError = ptTmp->Get_font_bitmap(dwCode, ptFontBitMap);
        if (0 == iError)
        {
            return 0;
        }
        ptTmp = ptTmp->p_next;
    }
    return -1;
}


int Set_fonts_detail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize)
{
    int iError = 0;
    P_Font_Ops ptFontOpr;

    ptFontOpr = Get_font_ops(pcFontsName);
    if (NULL == ptFontOpr)
    {
        return -1;
    }

    g_dwFontSize = dwFontSize;

    iError = ptFontOpr->Font_init(pcFontsFile, dwFontSize);

    return iError;
}