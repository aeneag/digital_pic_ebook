/*
* Created by Aen on 2021/7/16.
*/
#include <config.h>
#include <encoding_manager.h>
#include <string.h>
static int is_utf16le_coding(unsigned char *pucBufHead);
static int Utf16le_get_code_frm_buf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_Encoding_Ops g_t_utf16le_encoding_ops = {
        .name          = "utf-16le",
        .iHeadLen	   = 2,
        .is_support     = is_utf16le_coding,
        .Get_code_frm_buf = Utf16le_get_code_frm_buf,
};
static int is_utf16le_coding(unsigned char *pucBufHead){
    const char aStrUtf16le[] = {0xFF, 0xFE, 0};
    if (strncmp((const char *)pucBufHead, aStrUtf16le, 2) == 0)
    {
        /* UTF-16 little endian */
        return 1;
    }
    else
    {
        return 0;
    }
}
static int Utf16le_get_code_frm_buf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode){
    if (pucBufStart + 1 < pucBufEnd)
    {
        *pdwCode = (((unsigned int)pucBufStart[1])<<8) + pucBufStart[0];
        return 2;
    }
    else
    {
        return 0;
    }
}
int  Utf16le_encoding_init(void){
    Add_font_ops_for_encoding(&g_t_utf16le_encoding_ops, Get_font_ops("freetype"));
    Add_font_ops_for_encoding(&g_t_utf16le_encoding_ops, Get_font_ops("ascii"));
    return Register_encoding_ops(&g_t_utf16le_encoding_ops);
}