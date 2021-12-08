/*
* Created by Aen on 2021/7/19.
*/
#ifndef _PAGE_MANAGER_H
#define _PAGE_MANAGER_H
#include <input_manager.h>
#include <show_manager.h>

typedef struct Page_Params {
    int iPageID;                  /* 页面的ID */
    char strCurPictureFile[256];  /* 要处理的第1个图片文件 */
}T_Page_Params, *P_Page_Params;



typedef struct Page_Layout {
    int iTopLeftX;
    int iTopLeftY;
    int iBotRightX;
    int iBotRightY;
    int iBpp;
    int iMaxTotalBytes;
    P_Layout atLayout;
}T_Page_Layout, *P_Page_Layout;

typedef struct Page_Action {
    char *name;
    void (*Run)(P_Page_Params ptParentPageParams);
    int (*Get_input_event)(P_Page_Layout atLayout,P_Input_event ptInputEvent);
    int (*Prepare)(void);
    struct Page_Action *p_next;
}T_Page_action, *P_Page_action;

typedef struct PageCfg {
    int  iIntervalSecond;
    char strSeletedDir[256];
}T_PageCfg, *P_PageCfg;


//#define ID(name)   (int(name[0]) + int(name[1]) + int(name[2]) + int(name[3]))

int ID(char *name);

P_Page_action Page(char *pcName);
void Show_pages(void);
int Register_page_action(P_Page_action ptPageAction);
int Generic_get_input_event(P_Page_Layout ptPageLayout, P_Input_event ptInputEvent);
int Generate_page(P_Page_Layout ptPageLayout, P_Video_Mem ptVideoMem);
int Pages_init(void);

int Main_page_init(void);
int Setting_page_init(void);
int Interval_page_init(void);
int Browse_page_init(void);
int Auto_page_init(void);
int Manual_page_init(void);
int Text_page_init(void);

int Time_ms_between(struct timeval tTimeStart, struct timeval tTimeEnd);



void Get_page_cfg(P_PageCfg ptPageCfg);
void Get_selected_dir(char *strSeletedDir);
void Get_interval_time(int *piIntervalSecond);
#endif //_PAGE_MANAGER_H
