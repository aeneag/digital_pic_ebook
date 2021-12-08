/*
* Created by Aen on 2021/7/16.
*/
#include <config.h>
#include <encoding_manager.h>
#include <string.h>
static int is_ascii_coding(unsigned char *pucBufHead);
static int Ascii_get_code_frm_buf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);
static T_Encoding_Ops g_t_ascii_encoding_ops ={
        .name           = "utf-8",
        .iHeadLen       = 3,
        .is_support      = is_ascii_coding,
        .Get_code_frm_buf  = Ascii_get_code_frm_buf,
};

static int is_ascii_coding(unsigned char *pucBufHead){
    const char aStrUtf8[]    = {0xEF, 0xBB, 0xBF, 0};
    const char aStrUtf16le[] = {0xFF, 0xFE, 0};
    const char aStrUtf16be[] = {0xFE, 0xFF, 0};

    if (strncmp((const char*)pucBufHead, aStrUtf8, 3) == 0)
    {
        /* UTF-8 */
        return 0;
    }
    else if (strncmp((const char*)pucBufHead, aStrUtf16le, 2) == 0)
    {
        /* UTF-16 little endian */
        return 0;
    }
    else if (strncmp((const char*)pucBufHead, aStrUtf16be, 2) == 0)
    {
        /* UTF-16 big endian */
        return 0;
    }
    else
    {
        return 1;
    }
}
static int Ascii_get_code_frm_buf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode){
    unsigned char *pucBuf = pucBufStart;
    unsigned char c = *pucBuf;

    if ((pucBuf < pucBufEnd) && (c < (unsigned char)0x80))
    {
        /* Return ASCII code */
        *pdwCode = (unsigned int)c;
        return 1;
    }

    if (((pucBuf + 1) < pucBufEnd) && (c >= (unsigned char)0x80))
    {
        /* Return GBK code */
        *pdwCode = pucBuf[0] + (((unsigned int)pucBuf[1])<<8);
        return 2;
    }

    if (pucBuf < pucBufEnd)
    {
        /* It is possible that the file is corrupted, but a code is still returned, even if it is in error */
        *pdwCode = (unsigned int)c;
        return 1;
    }
    else
    {
        return 0;
    }
}
int  Ascii_encoding_init(void){
    Add_font_ops_for_encoding(&g_t_ascii_encoding_ops, Get_font_ops("freetype"));
    Add_font_ops_for_encoding(&g_t_ascii_encoding_ops, Get_font_ops("ascii"));
    Add_font_ops_for_encoding(&g_t_ascii_encoding_ops, Get_font_ops("gbk"));
    return Register_encoding_ops(&g_t_ascii_encoding_ops);
}