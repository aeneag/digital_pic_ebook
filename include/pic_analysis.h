/*
* Created by Aen on 2021/7/20.
*/
#ifndef _PIC_ANALYSIS_H
#define _PIC_ANALYSIS_H

#include <file.h>

typedef struct Pixel_Datas{
    int iHeight;
    int iWidth;
    int iBpp;
    int iLineBytes;  /* 一行数据有多少字节 */
    int iTotalBytes; /* 所有字节数 */
    unsigned char *auc_pixel_datas;
}T_Pixel_Datas,*P_Pixel_Datas;

typedef struct Pic_File_Parser {
    char *name;
    int (*Is_support)(P_File_Map ptFileMap);
    int (*Get_pixel_datas)(P_File_Map ptFileMap,P_Pixel_Datas pPixelDatas);
    int (*Free_pixel_datas)(P_Pixel_Datas pPixelDatas);
    struct Pic_File_Parser *p_next;
}T_Pic_File_Parser,*P_Pic_File_Parser;
#endif //_PIC_ANALYSIS_H
