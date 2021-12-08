/*
* Created by Aen on 2021/7/16.
*/
#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int is_utf16be_coding(unsigned char *pucBufHead);
static int Utf16be_get_code_frm_buf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_Encoding_Ops g_t_Utf16be_encoding_ops = {
        .name          = "utf-16be",
        .iHeadLen	   = 2,
        .is_support     = is_utf16be_coding,
        .Get_code_frm_buf = Utf16be_get_code_frm_buf,
};

static int is_utf16be_coding(unsigned char *pucBufHead)
{
    const char aStrUtf16be[] = {0xFE, 0xFF, 0};

    if (strncmp((const char*)pucBufHead, aStrUtf16be, 2) == 0)
    {
        /* UTF-16 big endian */
        return 1;
    }
    else
    {
        return 0;
    }
}

static int Utf16be_get_code_frm_buf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
    if (pucBufStart + 1 < pucBufEnd)
    {
        *pdwCode = (((unsigned int)pucBufStart[0])<<8) + pucBufStart[1];
        return 2;
    }
    else
    {
        return 0;
    }
}

int  Utf16be_encoding_init(void)
{
    Add_font_ops_for_encoding(&g_t_Utf16be_encoding_ops, Get_font_ops("freetype"));
    Add_font_ops_for_encoding(&g_t_Utf16be_encoding_ops, Get_font_ops("ascii"));
    return Register_encoding_ops(&g_t_Utf16be_encoding_ops);
}



