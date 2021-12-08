/*
* Created by Aen on 2021/7/16.
*/
#include "encoding_manager.h"
#include <config.h>
#include <string.h>
#include <stdlib.h>
static P_Encoding_Ops g_pt_encoding_ops_head;

int Register_encoding_ops(P_Encoding_Ops ptEncodingOps){
    P_Encoding_Ops ptTmp;
    if(!g_pt_encoding_ops_head){
        g_pt_encoding_ops_head = ptEncodingOps;
        g_pt_encoding_ops_head->p_next = NULL;
    } else{
        ptTmp = g_pt_encoding_ops_head;
        while (ptTmp->p_next){
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next = ptEncodingOps;
        ptEncodingOps->p_next = NULL;
    }
    return 0;
}
void Show_encoding_ops(void){
    int i = 0;
    P_Encoding_Ops  ptTmp = g_pt_encoding_ops_head;
    while (ptTmp){
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}

P_Encoding_Ops Select_encoding_ops_for_file(unsigned char *pucFileBufHead){
    P_Encoding_Ops  ptTmp = g_pt_encoding_ops_head;
    while (ptTmp)
    {
        if (ptTmp->is_support(pucFileBufHead))
            return ptTmp;
        else
            ptTmp = ptTmp->p_next;
    }
    return NULL;
}

int Add_font_ops_for_encoding(P_Encoding_Ops ptEncodingOps, P_Font_Ops ptFontOps){
    P_Font_Ops pt_font_ops_cpy;

    if (!ptEncodingOps || !ptFontOps)
    {
        return -1;
    }
    else
    {
        pt_font_ops_cpy = malloc(sizeof(T_Font_Ops));
        if (!pt_font_ops_cpy)
        {
            return -1;
        }
        else
        {
            memcpy(pt_font_ops_cpy, ptFontOps, sizeof(T_Font_Ops));
            pt_font_ops_cpy->p_next = ptEncodingOps->ptFontOprSupportedHead;
            ptEncodingOps->ptFontOprSupportedHead = pt_font_ops_cpy;
            return 0;
        }
    }
}
int Del_font_ops_frm_encoding(P_Encoding_Ops ptEncodingOps, P_Font_Ops ptFontOps){
    P_Font_Ops ptTmp;
    P_Font_Ops ptPre;

    if (!ptEncodingOps || !ptFontOps)
    {
        return -1;
    }
    else
    {
        ptTmp = ptEncodingOps->ptFontOprSupportedHead;
        if (strcmp(ptTmp->name, ptFontOps->name) == 0)
        {
            ptEncodingOps->ptFontOprSupportedHead = ptTmp->p_next;
            free(ptTmp);
            return 0;
        }

        ptPre = ptEncodingOps->ptFontOprSupportedHead;
        ptTmp = ptPre->p_next;
        while (ptTmp)
        {
            if (strcmp(ptTmp->name, ptFontOps->name) == 0)
            {
                ptPre->p_next = ptTmp->p_next;
                free(ptTmp);
                return 0;
            }
            else
            {
                ptPre = ptTmp;
                ptTmp = ptTmp->p_next;
            }
        }

        return -1;
    }
}

int Encoding_init(void){
    int iError;

    iError = Ascii_encoding_init();
    if (iError)
    {
        DBG_PRINTF("AsciiEncodingInit error!\n");
        return -1;
    }

    iError = Utf16le_encoding_init();
    if (iError)
    {
        DBG_PRINTF("Utf16leEncodingInit error!\n");
        return -1;
    }

    iError = Utf16be_encoding_init();
    if (iError)
    {
        DBG_PRINTF("Utf16beEncodingInit error!\n");
        return -1;
    }

    iError = Utf8_encoding_init();
    if (iError)
    {
        DBG_PRINTF("Utf8EncodingInit error!\n");
        return -1;
    }

    return 0;
}