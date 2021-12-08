/*
* Created by Aen on 2021/7/15.
*/
#include <config.h>
#include <show_manager.h>
#include <string.h>
#include <draw.h>


static P_Show_Ops g_pt_show_ops_head = NULL;
static P_Video_Mem g_pt_video_mem_head = NULL;

int Register_show_ops(P_Show_Ops pt_show_ops){
    P_Show_Ops ptTmp;

    if (!g_pt_show_ops_head)
    {
        g_pt_show_ops_head   = pt_show_ops;
        pt_show_ops->p_next = NULL;
    }
    else
    {
        ptTmp = g_pt_show_ops_head;
        while (ptTmp->p_next)
        {
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next	  = pt_show_ops;
        pt_show_ops->p_next = NULL;
    }
    return 0;
}

void Show_ops(void){
    int i = 0;
    P_Show_Ops ptTmp = g_pt_show_ops_head;
    while (ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}
P_Show_Ops Get_show_ops(char *pcName)
{
    P_Show_Ops ptTmp = g_pt_show_ops_head;

    while (ptTmp)
    {
        if (strcmp(ptTmp->name, pcName) == 0)
        {
            return ptTmp;
        }
        ptTmp = ptTmp->p_next;
    }
    return NULL;
}
///////////////////////////////
int Alloc_video_mem(int num){
    int i;
    int iXres;
    int iYres;
    int iBpp;
    int iVMSize;
    int iLineBytes;
    P_Video_Mem ptNew;

    P_Show_Ops pShowOps = Get_show_ops("fb");
    Get_show_resolution(&iXres, &iYres, &iBpp);
    iVMSize = iXres*iYres*iBpp/8;
    iLineBytes = iXres * iBpp / 8;

    ptNew = malloc(sizeof(T_Video_Mem));
    if (ptNew == NULL)
    {
        return -1;
    }
    ptNew->tPixelDatas.auc_pixel_datas = pShowOps->pucShowMem;

    ptNew->iID = 0;
    ptNew->bDevFrameBuffer = 1;
    ptNew->eVideoMemState = VMS_FREE;
    ptNew->ePicState	  = PS_BLANK;
    ptNew->tPixelDatas.iWidth  = iXres;
    ptNew->tPixelDatas.iHeight = iYres;
    ptNew->tPixelDatas.iBpp    = iBpp;
    ptNew->tPixelDatas.iLineBytes  = iLineBytes;
    ptNew->tPixelDatas.iTotalBytes = iVMSize;

    if (num != 0)
    {
        ptNew->eVideoMemState = VMS_USED_FOR_CURRENT;
    }


    ptNew->p_next = g_pt_video_mem_head;
    g_pt_video_mem_head = ptNew;


    for (i = 0; i < num; i++)
    {
        ptNew = malloc(sizeof(T_Video_Mem) + iVMSize);
        if (ptNew == NULL)
        {
            return -1;
        }
        ptNew->tPixelDatas.auc_pixel_datas = (unsigned char *)(ptNew + 1);

        ptNew->iID = 0;
        ptNew->bDevFrameBuffer = 0;
        ptNew->eVideoMemState = VMS_FREE;
        ptNew->ePicState      = PS_BLANK;
        ptNew->tPixelDatas.iWidth  = iXres;
        ptNew->tPixelDatas.iHeight = iYres;
        ptNew->tPixelDatas.iBpp    = iBpp;
        ptNew->tPixelDatas.iLineBytes = iLineBytes;
        ptNew->tPixelDatas.iTotalBytes = iVMSize;

        ptNew->p_next = g_pt_video_mem_head;
        g_pt_video_mem_head = ptNew;
    }
    return 0;
}
P_Video_Mem Get_video_mem(int iID, int bCur)
{
    P_Video_Mem ptTmp = g_pt_video_mem_head;

    while (ptTmp)
    {
        if ((ptTmp->eVideoMemState == VMS_FREE) && (ptTmp->iID == iID))
        {
            ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CURRENT : VMS_USED_FOR_PREPARE;
            return ptTmp;
        }
        ptTmp = ptTmp->p_next;
    }

    ptTmp = g_pt_video_mem_head;
    while (ptTmp)
    {
        if (ptTmp->eVideoMemState == VMS_FREE)
        {
            ptTmp->iID = iID;
            ptTmp->ePicState = PS_BLANK;
            ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CURRENT : VMS_USED_FOR_PREPARE;
            return ptTmp;
        }
        ptTmp = ptTmp->p_next;
    }

    return NULL;
}
void Put_video_mem(P_Video_Mem ptVideoMem)
{
    ptVideoMem->eVideoMemState = VMS_FREE;
}
/* 获得得硬件的显存, 在该显存上操作就可以直接在LCD上显示出来 */
P_Video_Mem Get_dev_video_mem(void)
{
    P_Video_Mem ptTmp = g_pt_video_mem_head;

    while (ptTmp)
    {
        if (ptTmp->bDevFrameBuffer)
        {
            return ptTmp;
        }
        ptTmp = ptTmp->p_next;
    }
    return NULL;
}


void Clear_video_mem(P_Video_Mem ptVideoMem, unsigned int dwColor)
{
    unsigned char *pucVM;
    unsigned short *pwVM16bpp;
    unsigned int *pdwVM32bpp;
    unsigned short wColor16bpp; /* 565 */
    int iRed;
    int iGreen;
    int iBlue;
    int i = 0;

    pucVM	   = ptVideoMem->tPixelDatas.auc_pixel_datas;
    pwVM16bpp  = (unsigned short *)pucVM;
    pdwVM32bpp = (unsigned int *)pucVM;

    switch (ptVideoMem->tPixelDatas.iBpp)
    {
        case 8:
        {
            memset(pucVM, dwColor, ptVideoMem->tPixelDatas.iTotalBytes);
            break;
        }
        case 16:
        {
            iRed   = (dwColor >> (16+3)) & 0x1f;
            iGreen = (dwColor >> (8+2)) & 0x3f;
            iBlue  = (dwColor >> 3) & 0x1f;
            wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
            while (i < ptVideoMem->tPixelDatas.iTotalBytes)
            {
                *pwVM16bpp	= wColor16bpp;
                pwVM16bpp++;
                i += 2;
            }
            break;
        }
        case 32:
        {
            while (i < ptVideoMem->tPixelDatas.iTotalBytes)
            {
                *pdwVM32bpp = dwColor;
                pdwVM32bpp++;
                i += 4;
            }
            break;
        }
        default :
        {
            DBG_PRINTF("can't support %d bpp\n", ptVideoMem->tPixelDatas.iBpp);
            return;
        }
    }

}
/* 把显存中某区域全部清为某种颜色 */
void Clear_video_mem_region(P_Video_Mem ptVideoMem, P_Layout ptLayout, unsigned int dwColor)
{
    unsigned char *pucVM;
    unsigned short *pwVM16bpp;
    unsigned int *pdwVM32bpp;
    unsigned short wColor16bpp; /* 565 */
    int iRed;
    int iGreen;
    int iBlue;
    int iX;
    int iY;
    int iLineBytesClear;
    int i;

    pucVM	   = ptVideoMem->tPixelDatas.auc_pixel_datas + ptLayout->iTopLeftY * ptVideoMem->tPixelDatas.iLineBytes + ptLayout->iTopLeftX * ptVideoMem->tPixelDatas.iBpp / 8;
    pwVM16bpp  = (unsigned short *)pucVM;
    pdwVM32bpp = (unsigned int *)pucVM;

    iLineBytesClear = (ptLayout->iBotRightX - ptLayout->iTopLeftX + 1) * ptVideoMem->tPixelDatas.iBpp / 8;

    switch (ptVideoMem->tPixelDatas.iBpp)
    {
        case 8:
        {
            for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++)
            {
                memset(pucVM, dwColor, iLineBytesClear);
                pucVM += ptVideoMem->tPixelDatas.iLineBytes;
            }
            break;
        }
        case 16:
        {
            iRed   = (dwColor >> (16+3)) & 0x1f;
            iGreen = (dwColor >> (8+2)) & 0x3f;
            iBlue  = (dwColor >> 3) & 0x1f;
            wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
            for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++)
            {
                i = 0;
                for (iX = ptLayout->iTopLeftX; iX <= ptLayout->iBotRightX; iX++)
                {
                    pwVM16bpp[i++]	= wColor16bpp;
                }
                pwVM16bpp = (unsigned short *)((unsigned int)pwVM16bpp + ptVideoMem->tPixelDatas.iLineBytes);
            }
            break;
        }
        case 32:
        {
            for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++)
            {
                i = 0;
                for (iX = ptLayout->iTopLeftX; iX <= ptLayout->iBotRightX; iX++)
                {
                    pdwVM32bpp[i++]	= dwColor;
                }
                pdwVM32bpp = (unsigned int *)((unsigned int)pdwVM32bpp + ptVideoMem->tPixelDatas.iLineBytes);
            }
            break;
        }
        default :
        {
            DBG_PRINTF("can't support %d bpp\n", ptVideoMem->tPixelDatas.iBpp);
            return;
        }
    }

}

int Show_init(void){
    int iError;
    iError = FB_init();
    return iError;
}


