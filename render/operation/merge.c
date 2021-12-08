/*
* Created by Aen on 2021/7/20.
*/
#include <stdio.h>
#include <pic_analysis.h>
#include <string.h>
int Pic_merge(int iX, int iY, P_Pixel_Datas ptSmallPic, P_Pixel_Datas ptBigPic)
{
    int i;
    unsigned char *pucSrc;
    unsigned char *pucDst;

    if ((ptSmallPic->iWidth > ptBigPic->iWidth)  ||
        (ptSmallPic->iHeight > ptBigPic->iHeight) ||
        (ptSmallPic->iBpp != ptBigPic->iBpp))
    {
        return -1;
    }

    pucSrc = ptSmallPic->auc_pixel_datas;
    pucDst = ptBigPic->auc_pixel_datas + iY * ptBigPic->iLineBytes + iX * ptBigPic->iBpp / 8;

    for (i = 0; i < ptSmallPic->iHeight; i++)
    {
        memcpy(pucDst, pucSrc, ptSmallPic->iLineBytes);

        pucSrc += ptSmallPic->iLineBytes;
        pucDst += ptBigPic->iLineBytes;
    }

    return 0;
}

/*
 * 把新图片并入老图片
 * iStartXofNewPic, iStartYofNewPic : 从新图片的(iStartXofNewPic, iStartYofNewPic)开始读出数据用于合并
 * iStartXofOldPic, iStartYofOldPic : 合并到老图片的(iStartXofOldPic, iStartYofOldPic)去
 * iWidth, iHeight : 合并区域的大小
 * ptNewPic        : 新图片
 * ptOldPic        : 老图片
 */
int Pic_merge_region(int iStartXofNewPic, int iStartYofNewPic, int iStartXofOldPic, int iStartYofOldPic, int iWidth, int iHeight, P_Pixel_Datas ptNewPic, P_Pixel_Datas ptOldPic)
{
    int i;
    unsigned char *pucSrc;
    unsigned char *pucDst;
    int iLineBytesCpy = iWidth * ptNewPic->iBpp / 8;

    if ((iStartXofNewPic < 0 || iStartXofNewPic >= ptNewPic->iWidth) || \
        (iStartYofNewPic < 0 || iStartYofNewPic >= ptNewPic->iHeight) || \
        (iStartXofOldPic < 0 || iStartXofOldPic >= ptOldPic->iWidth) || \
        (iStartYofOldPic < 0 || iStartYofOldPic >= ptOldPic->iHeight))
    {
        return -1;
    }

    pucSrc = ptNewPic->auc_pixel_datas + iStartYofNewPic * ptNewPic->iLineBytes + iStartXofNewPic * ptNewPic->iBpp / 8;
    pucDst = ptOldPic->auc_pixel_datas + iStartYofOldPic * ptOldPic->iLineBytes + iStartXofOldPic * ptOldPic->iBpp / 8;
    for (i = 0; i < iHeight; i++)
    {
        memcpy(pucDst, pucSrc, iLineBytesCpy);
        pucSrc += ptNewPic->iLineBytes;
        pucDst += ptOldPic->iLineBytes;
    }
    return 0;
}