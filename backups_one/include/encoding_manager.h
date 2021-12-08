/*
* Created by Aen on 2021/7/16.
*/
#ifndef _ENCODING_MANAGER_H
#define _ENCODING_MANAGER_H
#include <fonts_manager.h>
#include <show_manager.h>
typedef struct Encoding_Ops{
    char *name;
    int iHeadLen;
    P_Font_Ops   ;
    int (*is_support)(unsigned char *pucBufHead);
    int (*Get_code_frm_buf)(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);
    struct Encoding_Ops *p_next;
}T_Encoding_Ops,*P_Encoding_Ops;

int Register_encoding_ops(P_Encoding_Ops ptEncodingOps);
void Show_encoding_ops(void);

P_Encoding_Ops Select_encoding_ops_for_file(unsigned char *pucFileBufHead);
int Add_font_ops_for_encoding(P_Encoding_Ops ptEncodingOps, P_Font_Ops ptFontOps);
int Del_font_ops_frm_encoding(P_Encoding_Ops ptEncodingOps, P_Font_Ops ptFontOps);

int Encoding_init(void);

int  Ascii_encoding_init(void);
int  Utf16be_encoding_init(void);
int  Utf16le_encoding_init(void);
int  Utf8_encoding_init(void);
#endif //_ENCODING_MANAGER_H
