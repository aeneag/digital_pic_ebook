/*
* Created by Aen on 2021/7/15.
*/

#ifndef _SHOW_MANAGER_H
#define _SHOW_MANAGER_H
#include <pic_analysis.h>

typedef struct Layout {
    int iTopLeftX;
    int iTopLeftY;
    int iBotRightX;
    int iBotRightY;
    char *strIconName;
}T_Layout, *P_Layout;

typedef  enum {
    VMS_FREE = 0,
    VMS_USED_FOR_PREPARE,
    VMS_USED_FOR_CURRENT,
}E_Video_Mem_State;

typedef  enum {
    PS_BLANK = 0,
    PS_GENERATING,
    PS_GENERATED,
}E_Pic_State;

typedef struct Video_Mem {
    int iID;
    int bDevFrameBuffer;
    E_Video_Mem_State eVideoMemState;
    E_Pic_State ePicState;
    T_Pixel_Datas tPixelDatas;
    struct Video_Mem *p_next;
}T_Video_Mem,*P_Video_Mem;

typedef struct T_Show_Ops {
    char *name;
    int iXres;
    int iYres;
    int iBpp;
    int iLineWidth;
    unsigned char *pucShowMem;
    int (*Device_init)(void);
    int (*Show_pixel)(int iPenX, int iPenY, unsigned int dwColor);
    int (*Clean_screen)(unsigned int dwBackColor);
    int (*Show_page)(P_Video_Mem pVideoMem);
    struct T_Show_Ops *p_next;
}T_Show_Ops, *P_Show_Ops;


int Register_show_ops(P_Show_Ops pt_show_ops);
void Show_ops(void);
P_Show_Ops Get_show_ops(char *pcName);
int Show_init(void);
int FB_init(void);

int Alloc_video_mem(int num);
P_Video_Mem Get_video_mem(int iID, int bCur);
void Put_video_mem(P_Video_Mem ptVideoMem);
void Clear_video_mem(P_Video_Mem ptVideoMem, unsigned int dwColor);
P_Video_Mem Get_dev_video_mem(void);
void Clear_video_mem_region(P_Video_Mem ptVideoMem, P_Layout ptLayout, unsigned int dwColor);

#endif //_SHOW_MANAGER_H
