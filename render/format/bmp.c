/*
* Created by Aen on 2021/7/20.
*/
#include <pic_analysis.h>
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <picfmt_manager.h>
#include <file.h>

#pragma pack(push) /* ����ǰpack����ѹջ���� */
#pragma pack(1)    /* �����ڽṹ�嶨��֮ǰʹ�� */

typedef struct tagBITMAPFILEHEADER { /* bmfh */
    unsigned short bfType;
    unsigned long  bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long  bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER { /* bmih */
    unsigned long  biSize;
    unsigned long  biWidth;
    unsigned long  biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long  biCompression;
    unsigned long  biSizeImage;
    unsigned long  biXPelsPerMeter;
    unsigned long  biYPelsPerMeter;
    unsigned long  biClrUsed;
    unsigned long  biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop) /* �ָ���ǰ��pack���� */

static int is_bmp_format(P_File_Map ptFileMap);
static int Get_pixel_datas_frm_bmp(P_File_Map ptFileMap, P_Pixel_Datas ptPixelDatas);
static int Free_pixel_datas_for_bmp(P_Pixel_Datas ptPixelDatas);

T_Pic_File_Parser g_t_bmp_parser = {
        .name           = "bmp",
        .Is_support      = is_bmp_format,
        .Get_pixel_datas  = Get_pixel_datas_frm_bmp,
        .Free_pixel_datas = Free_pixel_datas_for_bmp,
};


static int is_bmp_format(P_File_Map ptFileMap){
    unsigned char *aFileHead = ptFileMap->pucFileMapMem;
    if (aFileHead[0] != 0x42 || aFileHead[1] != 0x4d)
        return 0;
    else
        return 1;
}
static int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, unsigned char *pudSrcDatas, unsigned char *pudDstDatas)
{
    unsigned int dwRed;
    unsigned int dwGreen;
    unsigned int dwBlue;
    unsigned int dwColor;

    unsigned short *pwDstDatas16bpp = (unsigned short *)pudDstDatas;
    unsigned int   *pwDstDatas32bpp = (unsigned int *)pudDstDatas;

    int i;
    int pos = 0;

    if (iSrcBpp != 24)
    {
        return -1;
    }

    if (iDstBpp == 24)
    {
        memcpy(pudDstDatas, pudSrcDatas, iWidth*3);
    }
    else
    {
        for (i = 0; i < iWidth; i++)
        {
            dwBlue  = pudSrcDatas[pos++];
            dwGreen = pudSrcDatas[pos++];
            dwRed   = pudSrcDatas[pos++];
            if (iDstBpp == 32)
            {
                dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
                *pwDstDatas32bpp = dwColor;
                pwDstDatas32bpp++;
            }
            else if (iDstBpp == 16)
            {
                /* 565 */
                dwRed   = dwRed >> 3;
                dwGreen = dwGreen >> 2;
                dwBlue  = dwBlue >> 3;
                dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue);
                *pwDstDatas16bpp = dwColor;
                pwDstDatas16bpp++;
            }
        }
    }
    return 0;
}

static int Get_pixel_datas_frm_bmp(P_File_Map ptFileMap, P_Pixel_Datas ptPixelDatas){
    BITMAPFILEHEADER *ptBITMAPFILEHEADER;
    BITMAPINFOHEADER *ptBITMAPINFOHEADER;
    unsigned char *aFileHead = ptFileMap->pucFileMapMem;
    int iWidth;
    int iHeight;
    int iBMPBpp;
    int y;

    unsigned char *pucSrc;
    unsigned char *pucDest;
    int iLineWidthAlign;
    int iLineWidthReal;


    ptBITMAPFILEHEADER = (BITMAPFILEHEADER *)aFileHead;
    ptBITMAPINFOHEADER = (BITMAPINFOHEADER *)(aFileHead + sizeof(BITMAPFILEHEADER));

    iWidth = ptBITMAPINFOHEADER->biWidth;
    iHeight = ptBITMAPINFOHEADER->biHeight;
    iBMPBpp = ptBITMAPINFOHEADER->biBitCount;

    if (iBMPBpp != 24)
    {
        DBG_PRINTF("iBMPBpp = %d\n", iBMPBpp);
        DBG_PRINTF("sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
        return -1;
    }

    ptPixelDatas->iWidth  = iWidth;
    ptPixelDatas->iHeight = iHeight;
    //ptPixelDatas->iBpp    = iBpp;
    ptPixelDatas->auc_pixel_datas = malloc(iWidth * iHeight * ptPixelDatas->iBpp / 8);
    ptPixelDatas->iLineBytes    = iWidth * ptPixelDatas->iBpp / 8;
    if (NULL == ptPixelDatas->auc_pixel_datas)
    {
        return -1;
    }

    iLineWidthReal = iWidth * iBMPBpp / 8;
    iLineWidthAlign = (iLineWidthReal + 3) & ~0x3;   /* ��4ȡ�� */

    pucSrc = aFileHead + ptBITMAPFILEHEADER->bfOffBits;
    pucSrc = pucSrc + (iHeight - 1) * iLineWidthAlign;

    pucDest = ptPixelDatas->auc_pixel_datas;

    for (y = 0; y < iHeight; y++)
    {
        //memcpy(pucDest, pucSrc, iLineWidthReal);
        CovertOneLine(iWidth, iBMPBpp, ptPixelDatas->iBpp, pucSrc, pucDest);
        pucSrc  -= iLineWidthAlign;
        pucDest += ptPixelDatas->iLineBytes;
    }
    return 0;
}
static int Free_pixel_datas_for_bmp(P_Pixel_Datas ptPixelDatas){
    free(ptPixelDatas->auc_pixel_datas);
    return 0;
}

int BMP_parser_init(void)
{
    return Register_pic_file_parser(&g_t_bmp_parser);
}


























