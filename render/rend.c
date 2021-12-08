/*
* Created by Aen on 2021/7/19.
*/
#include <show_manager.h>
#include <file.h>
#include <fonts_manager.h>
#include <config.h>
#include <draw.h>
#include <stdio.h>
#include <page_manager.h>
#include <stdio.h>
#include <string.h>
#include <encoding_manager.h>
#include <picfmt_manager.h>
#include <stdlib.h>

#if 0
int GetFontPixel()
{
}

int GetPicPixel()
{
}


int DrawPixel()
{
}
#endif


void Flush_video_mem_to_device(P_Video_Mem ptVideoMem)
{
    if (!ptVideoMem->bDevFrameBuffer)
    {
        Get_show_ops("fb")->Show_page(ptVideoMem);
    }
}
int Get_pixel_datas_for_icon(char *file_name, P_Pixel_Datas ptPixelDatas){
    T_File_Map tFileMap;
    int iError;
    int iXres, iYres, iBpp;

    snprintf(tFileMap.strFileName, 128, "%s/%s", ICON_PATH, file_name);
    tFileMap.strFileName[127] = '\0';

    iError = Map_file(&tFileMap);
    if (iError)
    {
        DBG_PRINTF("Aen--- MapFile %s error!\n", file_name);
        return -1;
    }

    iError = Parser("bmp")->Is_support(&tFileMap);
    if (iError == 0)
    {
        DBG_PRINTF("%s is not bmp file\n", file_name);
        return -1;
    }

    Get_show_resolution(&iXres, &iYres, &iBpp);
    ptPixelDatas->iBpp = iBpp;
    iError = Parser("bmp")->Get_pixel_datas(&tFileMap, ptPixelDatas);
    if (iError)
    {
        DBG_PRINTF("GetPixelDatas for %s error!\n", file_name);
        Un_map_file(&tFileMap);
        return -1;
    }
    Un_map_file(&tFileMap);
    return 0;
}
void Free_pixel_datas_for_icon(P_Pixel_Datas ptPixelDatas)
{
    Parser("bmp")->Free_pixel_datas(ptPixelDatas);
}

int Get_pixel_datas_frm_file(char *strFileName, P_Pixel_Datas ptPixelDatas)
{
    T_File_Map tFileMap;
    int iError;
    int iXres, iYres, iBpp;
    P_Pic_File_Parser ptParser;

    strncpy(tFileMap.strFileName, strFileName, 256);
    tFileMap.strFileName[255] = '\0';
//    int jj;
//    printf("\n===========");
//    for(jj=0;jj<16;jj++){
//        printf("%c",tFileMap.strFileName[jj]);
//    }
//    printf("\n");
    iError = Map_file(&tFileMap);
    if (iError)
    {
        DBG_PRINTF("MapFile %s error!\n", strFileName);
        return -1;
    }

    ptParser = Get_parser(&tFileMap);
    if (ptParser == NULL)
    {
        Un_map_file(&tFileMap);
        return -1;
    }

    Get_show_resolution(&iXres, &iYres, &iBpp);
    ptPixelDatas->iBpp = iBpp;
    iError = ptParser->Get_pixel_datas(&tFileMap, ptPixelDatas);
    if (iError)
    {
        DBG_PRINTF("GetPixelDatas for %s error!\n", strFileName);
        Un_map_file(&tFileMap);
        return -1;
    }

    Un_map_file(&tFileMap);
    return 0;
}

void Free_pixel_datas_frm_file(P_Pixel_Datas ptPixelDatas)
{
    //Parser("bmp")->FreePixelDatas(ptPixelDatas);
    free(ptPixelDatas->auc_pixel_datas);
}

/* 返回值: 设置了VideoMem中多少字节 */
static int Set_color_for_pixel_in_video_mem(int iX, int iY, P_Video_Mem ptVideoMem, unsigned int dwColor)
{
    unsigned char *pucVideoMem;
    unsigned short *pwVideoMem16bpp;
    unsigned int *pdwVideoMem32bpp;
    unsigned short wColor16bpp; /* 565 */
    int iRed;
    int iGreen;
    int iBlue;

    pucVideoMem      = ptVideoMem->tPixelDatas.auc_pixel_datas;
    pucVideoMem      += iY * ptVideoMem->tPixelDatas.iLineBytes + iX * ptVideoMem->tPixelDatas.iBpp / 8;
    pwVideoMem16bpp  = (unsigned short *)pucVideoMem;
    pdwVideoMem32bpp = (unsigned int *)pucVideoMem;

    //DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    //DBG_PRINTF("x = %d, y = %d\n", iX, iY);

    switch (ptVideoMem->tPixelDatas.iBpp)
    {
        case 8:
        {
            *pucVideoMem = (unsigned char)dwColor;
            return 1;
            break;
        }
        case 16:
        {
            iRed   = (dwColor >> (16+3)) & 0x1f;
            iGreen = (dwColor >> (8+2)) & 0x3f;
            iBlue  = (dwColor >> 3) & 0x1f;
            wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
            *pwVideoMem16bpp	= wColor16bpp;
            return 2;
            break;
        }
        case 32:
        {
            *pdwVideoMem32bpp = dwColor;
            return 4;
            break;
        }
        default :
        {
            return -1;
        }
    }

    return -1;
}

void Clear_rectangle_in_video_mem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, P_Video_Mem ptVideoMem, unsigned int dwColor)
{
    int x, y;
    for (y = iTopLeftY; y <= iBotRightY; y++)
        for (x = iTopLeftX; x <= iBotRightX; x++)
            Set_color_for_pixel_in_video_mem(x, y, ptVideoMem, dwColor);
}
static int Is_font_in_area(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, P_Font_Bit_Map ptFontBitMap)
{
    if ((ptFontBitMap->iXLeft >= iTopLeftX) && (ptFontBitMap->iXMax <= iBotRightX) && \
         (ptFontBitMap->iYTop >= iTopLeftY) && (ptFontBitMap->iYMax <= iBotRightY))
        return 1;
    else
        return 0;

}
/* 根据位图中的数据把字符显示到videomem中 */
static int Merge_one_font_to_video_mem(P_Font_Bit_Map ptFontBitMap, P_Video_Mem ptVideoMem)
{
    int i;
    int x, y;
    int bit;
    int iNum;
    unsigned char ucByte;

    if (ptFontBitMap->iBpp == 1)
    {
        for (y = ptFontBitMap->iYTop; y < ptFontBitMap->iYMax; y++)
        {
            i = (y - ptFontBitMap->iYTop) * ptFontBitMap->iPitch;
            for (x = ptFontBitMap->iXLeft, bit = 7; x < ptFontBitMap->iXMax; x++)
            {
                if (bit == 7)
                {
                    ucByte = ptFontBitMap->pucBuffer[i++];
                }

                if (ucByte & (1<<bit))
                {
                    iNum = Set_color_for_pixel_in_video_mem(x, y, ptVideoMem, COLOR_FORE_GROUND);
                }
                else
                {
                    /* 使用背景色 */
                    // g_ptDispOpr->ShowPixel(x, y, 0); /* 黑 */
                    iNum = Set_color_for_pixel_in_video_mem(x, y, ptVideoMem, COLOR_BACK_GROUND);
                }
                if (iNum == -1)
                {
                    return -1;
                }
                bit--;
                if (bit == -1)
                {
                    bit = 7;
                }
            }
        }
    }
    else if (ptFontBitMap->iBpp == 8)
    {
        for (y = ptFontBitMap->iYTop; y < ptFontBitMap->iYMax; y++)
            for (x = ptFontBitMap->iXLeft; x < ptFontBitMap->iXMax; x++)
            {
                //g_ptDispOpr->ShowPixel(x, y, ptFontBitMap->pucBuffer[i++]);
                if (ptFontBitMap->pucBuffer[i++])
                {
                    iNum = Set_color_for_pixel_in_video_mem(x, y, ptVideoMem, COLOR_FORE_GROUND);
                }
                else
                {
                    iNum = Set_color_for_pixel_in_video_mem(x, y, ptVideoMem, COLOR_BACK_GROUND);
                }

                if (iNum == -1)
                {
                    return -1;
                }
            }
    }
    else
    {
        DBG_PRINTF("ShowOneFont error, can't support %d bpp\n", ptFontBitMap->iBpp);
        return -1;
    }
    return 0;
}

/*
 * 在videomem的指定矩形中间显示字符串
 * 参考: 03.freetype\02th_arm\06th_show_lines_center
 */
int Merger_string_to_center_of_rectangle_in_video_mem(int iTopLeftX, int iTopLeftY, int iBotRightX,
                                                      int iBotRightY, unsigned char *pucTextString, P_Video_Mem ptVideoMem)
{int iLen;
    int iError;
    unsigned char *pucBufStart;
    unsigned char *pucBufEnd;
    unsigned int dwCode;
    T_Font_Bit_Map tFontBitMap;

    int bHasGetCode = 0;

    int iMinX = 32000, iMaxX = -1;
    int iMinY = 32000, iMaxY = -1;

//    int iFirstFontTopLeftX = 32000, iFirstFontTopLeftY = 32000;
//    int iNewFirstFontTopLeftX , iNewFirstFontTopLeftY;
    int iStrTopLeftX , iStrTopLeftY;

    int iWidth, iHeight;

    tFontBitMap.iCurOriginX = 0;
    tFontBitMap.iCurOriginY = 0;
    pucBufStart = pucTextString;
    pucBufEnd   = pucTextString + strlen((char *)pucTextString);

    /* 0. 清除这个区域 */
    Clear_rectangle_in_video_mem(iTopLeftX, iTopLeftY, iBotRightX, iBotRightY, ptVideoMem, COLOR_BACK_GROUND);

    /* 1.先计算字符串显示的总体宽度、高度 */
    while (1)
    {
        /* 从字符串中逐个取出字符 */
        iLen = Get_code_frm_buf(pucBufStart, pucBufEnd, &dwCode);
        if (0 == iLen)
        {
            /* 字符串结束 */
            if (!bHasGetCode)
            {
                //DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            else
            {
                break;
            }
        }

        bHasGetCode = 1;
        pucBufStart += iLen;

        /* 获得字符的位图, 位图信息里含有字符显示时的左上角、右下角坐标 */
        iError = Get_font_bitmap(dwCode, &tFontBitMap);
        if (0 == iError)
        {
//            if (32000 == iFirstFontTopLeftX)
//            {
//                iFirstFontTopLeftX = tFontBitMap.iXLeft;
//                iFirstFontTopLeftY = tFontBitMap.iYTop;
//            }

            if (iMinX > tFontBitMap.iXLeft)
            {
                iMinX = tFontBitMap.iXLeft;
            }
            if (iMaxX < tFontBitMap.iXMax)
            {
                iMaxX = tFontBitMap.iXMax;
            }

            if (iMinY > tFontBitMap.iYTop)
            {
                iMinY = tFontBitMap.iYTop;
            }
            if (iMaxY < tFontBitMap.iXMax)
            {
                iMaxY = tFontBitMap.iYMax;
            }

            tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
            tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
        }
        else
        {
            DBG_PRINTF("GetFontBitmap for calc width/height error!\n");
        }
    }

    iWidth  = iMaxX - iMinX;
    iHeight = iMaxY - iMinY;

    if (iWidth > iBotRightX - iTopLeftX)
    {
        //DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    if (iHeight > iBotRightY - iTopLeftY)
    {
        //DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //DBG_PRINTF("iHeight = %d, iBotRightY - iTopLeftX = %d - %d = %d\n", iHeight, iBotRightY, iTopLeftY, iBotRightY - iTopLeftY);
        return -1;
    }
    //DBG_PRINTF("iWidth = %d, iHeight = %d\n", iWidth, iHeight);

    /* 2.确定第1个字符的原点
     * 2.1 先计算左上角坐标
     */
    iStrTopLeftX = iTopLeftX + (iBotRightX - iTopLeftX - iWidth) / 2;
    iStrTopLeftY = iTopLeftY + (iBotRightY - iTopLeftY - iHeight) / 2;
    //DBG_PRINTF("iNewFirstFontTopLeftX = %d, iNewFirstFontTopLeftY = %d\n", iNewFirstFontTopLeftX, iNewFirstFontTopLeftY);

    /*
     * 2.2 再计算第1个字符原点坐标
     */
//    tFontBitMap.iCurOriginX = iNewFirstFontTopLeftX - iFirstFontTopLeftX;
//    tFontBitMap.iCurOriginY = iNewFirstFontTopLeftY - iFirstFontTopLeftY;
    tFontBitMap.iCurOriginX = iStrTopLeftX - iMinX;
    tFontBitMap.iCurOriginY = iStrTopLeftY - iMinY;

    //DBG_PRINTF("iCurOriginX = %d, iCurOriginY = %d\n", tFontBitMap.iCurOriginX, tFontBitMap.iCurOriginY);

    pucBufStart = pucTextString;
    bHasGetCode = 0;
    while (1)
    {
        /* 从字符串中逐个取出字符 */
        iLen = Get_code_frm_buf(pucBufStart, pucBufEnd, &dwCode);
        if (0 == iLen)
        {
            /* 字符串结束 */
            if (!bHasGetCode)
            {
                DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            else
            {
                break;
            }
        }

        bHasGetCode = 1;
        pucBufStart += iLen;

        /* 获得字符的位图 */
        iError = Get_font_bitmap(dwCode, &tFontBitMap);
        if (0 == iError)
        {
            //DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
            /* 显示一个字符 */
            if (Is_font_in_area(iTopLeftX, iTopLeftY, iBotRightX, iBotRightY, &tFontBitMap))
            {
                if (Merge_one_font_to_video_mem(&tFontBitMap, ptVideoMem))
                {
                    DBG_PRINTF("MergeOneFontToVideoMem error for code 0x%x\n", dwCode);
                    return -1;
                }
            }
            else
            {
                return 0;
            }
            //DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

            tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
            tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
        }
        else
        {
            DBG_PRINTF("GetFontBitmap for drawing error!\n");
        }
    }
   // Clear_rectangle_in_video_mem(iTopLeftX, iTopLeftY, iBotRightX, iBotRightY, ptVideoMem, 0x0);
    return 0;
}

static void InvertButton(P_Layout ptLayout)
{

    int iY;
    int i;
    int iButtonWidthBytes;
    unsigned char *pucVideoMem;
    P_Show_Ops ptDispOpr = Get_show_ops("fb");

    pucVideoMem = ptDispOpr->pucShowMem;
    pucVideoMem += ptLayout->iTopLeftY * ptDispOpr->iLineWidth + ptLayout->iTopLeftX * ptDispOpr->iBpp / 8; /* 图标在Framebuffer中的地址 */
    iButtonWidthBytes = (ptLayout->iBotRightX - ptLayout->iTopLeftX +1 ) * ptDispOpr->iBpp / 8;

    for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++){
        for (i = 0; i < iButtonWidthBytes; i++){
            pucVideoMem[i] = ~pucVideoMem[i];
        }
        pucVideoMem += ptDispOpr->iLineWidth;
    }

}

void Release_button(P_Layout ptLayout)
{
    InvertButton(ptLayout);
}

void Press_button(P_Layout ptLayout)
{
    InvertButton(ptLayout);
}
int Is_picture_file_supported(char *strFileName)
{
    T_File_Map tFileMap;
    int iError;
/////////////////////////////////////
    char *pcTmp;
    char is_pic_name[256];
    char file_type[10];
    strncpy(is_pic_name, strFileName, 256);
    pcTmp = strrchr(is_pic_name, '.');
    *pcTmp = '\0';
    strcpy(file_type, pcTmp+1);

    printf("file_type =%s \n",file_type);
    if(strcmp(file_type,"txt") == 0){
        return -8;
    }
    printf("--------support pic text\n");
////////////////////////////////////////

    strncpy(tFileMap.strFileName, strFileName, 256);
    tFileMap.strFileName[255] = '\0';
    iError = Map_file(&tFileMap);
    if (iError)
    {
        DBG_PRINTF("MapFile %s error!\n", strFileName);
        return 0;
    }

    if (Get_parser(&tFileMap) == NULL)
    {
        Un_map_file(&tFileMap);
        return 0;
    }

    Un_map_file(&tFileMap);
    return 1;
}
