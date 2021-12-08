/*
* Created by Aen on 2021/7/20.
*/
#include <pic_analysis.h>
#include <config.h>
#include <stdlib.h>
#include <string.h>
int Pic_zoom(P_Pixel_Datas ptOriginPic, P_Pixel_Datas ptZoomPic)
{
    unsigned long dwDstWidth = ptZoomPic->iWidth;
    unsigned long* pdwSrcXTable;
    unsigned long x;
    unsigned long y;
    unsigned long dwSrcY;
    unsigned char *pucDest;
    unsigned char *pucSrc;
    unsigned long dwPixelBytes = ptOriginPic->iBpp/8;

    if (ptOriginPic->iBpp != ptZoomPic->iBpp)
    {
        return -1;
    }

    pdwSrcXTable = malloc(sizeof(unsigned long) * dwDstWidth);
    if (NULL == pdwSrcXTable)
    {
        DBG_PRINTF("malloc error!\n");
        return -1;
    }

    for (x = 0; x < dwDstWidth; x++)//生成表 pdwSrcXTable
    {
        pdwSrcXTable[x]=(x*ptOriginPic->iWidth/ptZoomPic->iWidth);
    }

    for (y = 0; y < ptZoomPic->iHeight; y++)
    {
        dwSrcY = (y * ptOriginPic->iHeight / ptZoomPic->iHeight);

        pucDest = ptZoomPic->auc_pixel_datas + y*ptZoomPic->iLineBytes;
        pucSrc  = ptOriginPic->auc_pixel_datas + dwSrcY*ptOriginPic->iLineBytes;

        for (x = 0; x <dwDstWidth; x++)
        {
            /* 原图座标: pdwSrcXTable[x]，srcy
             * 缩放座标: x, y
			 */
            memcpy(pucDest+x*dwPixelBytes, pucSrc+pdwSrcXTable[x]*dwPixelBytes, dwPixelBytes);
        }
    }

    free(pdwSrcXTable);
    return 0;
}
