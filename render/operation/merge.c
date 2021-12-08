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
 * ����ͼƬ������ͼƬ
 * iStartXofNewPic, iStartYofNewPic : ����ͼƬ��(iStartXofNewPic, iStartYofNewPic)��ʼ�����������ںϲ�
 * iStartXofOldPic, iStartYofOldPic : �ϲ�����ͼƬ��(iStartXofOldPic, iStartYofOldPic)ȥ
 * iWidth, iHeight : �ϲ�����Ĵ�С
 * ptNewPic        : ��ͼƬ
 * ptOldPic        : ��ͼƬ
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