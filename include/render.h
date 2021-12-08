/*
* Created by Aen on 2021/7/20.
*/
#ifndef _RENDER_H
#define _RENDER_H

#include <page_manager.h>

int Get_pixel_datas_for_icon(char *file_name, P_Pixel_Datas ptPixelDatas);
void Free_pixel_datas_for_icon(P_Pixel_Datas ptPixelDatas);
int Get_pixel_datas_frm_file(char *strFileName, P_Pixel_Datas ptPixelDatas);

void Flush_video_mem_to_device(P_Video_Mem ptVideoMem);
int Pic_merge(int iX, int iY, P_Pixel_Datas ptSmallPic, P_Pixel_Datas ptBigPic);
int Pic_merge_region(int iStartXofNewPic, int iStartYofNewPic, int iStartXofOldPic, int iStartYofOldPic, int iWidth, int iHeight, P_Pixel_Datas ptNewPic, P_Pixel_Datas ptOldPic);
int Pic_zoom(P_Pixel_Datas ptOriginPic, P_Pixel_Datas ptZoomPic);
void Release_button(P_Layout ptLayout);
void Press_button(P_Layout ptLayout);
int Merger_string_to_center_of_rectangle_in_video_mem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char *pucTextString, P_Video_Mem ptVideoMem);
void Clear_rectangle_in_video_mem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, P_Video_Mem ptVideoMem, unsigned int dwColor);
int Is_picture_file_supported(char *strFileName);

#endif //_RENDER_H
