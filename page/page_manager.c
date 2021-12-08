/*
* Created by Aen on 2021/7/19.
*/
#include <config.h>
#include <page_manager.h>
#include <string.h>
#include <render.h>
#include <stdlib.h>
static P_Page_action g_pt_page_action_head;

int Register_page_action(P_Page_action ptPageAction)
{
    P_Page_action ptTmp;

    if (!g_pt_page_action_head)
    {
        g_pt_page_action_head   = ptPageAction;
        ptPageAction->p_next = NULL;
    }
    else
    {
        ptTmp = g_pt_page_action_head;
        while (ptTmp->p_next)
        {
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next = ptPageAction;
        ptPageAction->p_next = NULL;
    }

    return 0;
}


void Show_pages(void)
{
    int i = 0;
    P_Page_action ptTmp = g_pt_page_action_head;

    while (ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}

P_Page_action Page(char *pcName)
{
    P_Page_action ptTmp = g_pt_page_action_head;

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

int ID(char *name)
{
    return (int)(name[0]) + (int)(name[1]) + (int)(name[2]) + (int)(name[3]);
}
int Generate_page(P_Page_Layout ptPageLayout, P_Video_Mem ptVideoMem)
{
    T_Pixel_Datas tOriginIconPixelDatas;
    T_Pixel_Datas tIconPixelDatas;
    int iError;
    P_Layout atLayout = ptPageLayout->atLayout;

    /* 描画数据 */
    if (ptVideoMem->ePicState != PS_GENERATED)
    {
        Clear_video_mem(ptVideoMem, COLOR_BACK_GROUND);

        tIconPixelDatas.iBpp          = ptPageLayout->iBpp;
        tIconPixelDatas.auc_pixel_datas = malloc(ptPageLayout->iMaxTotalBytes);
        if (tIconPixelDatas.auc_pixel_datas == NULL)
        {
            return -1;
        }

        while (atLayout->strIconName)
        {
            iError =
                    Get_pixel_datas_for_icon(atLayout->strIconName, &tOriginIconPixelDatas);
            if (iError)
            {
                DBG_PRINTF("GetPixelDatasForIcon %s error!\n", atLayout->strIconName);
                return -1;
            }
            tIconPixelDatas.iHeight = atLayout->iBotRightY - atLayout->iTopLeftY + 1;
            tIconPixelDatas.iWidth  = atLayout->iBotRightX - atLayout->iTopLeftX+ 1;
            tIconPixelDatas.iLineBytes  = tIconPixelDatas.iWidth * tIconPixelDatas.iBpp / 8;
            tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;
            Pic_zoom(&tOriginIconPixelDatas, &tIconPixelDatas);
            Pic_merge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);
            Free_pixel_datas_for_icon(&tOriginIconPixelDatas);
            atLayout++;
        }
        free(tIconPixelDatas.auc_pixel_datas);
        ptVideoMem->ePicState = PS_GENERATED;
    }

    return 0;
}


int Generic_get_input_event(P_Page_Layout ptPageLayout, P_Input_event ptInputEvent)
{
    T_Input_event tInputEvent;
    int iRet;
    int i = 0;
    P_Layout atLayout = ptPageLayout->atLayout;

    /* 获得原始的触摸屏数据
     * 它是调用input_manager.c的函数，此函数会让当前线否休眠
     * 当触摸屏线程获得数据后，会把它唤醒
     */
    iRet = Get_input_event(&tInputEvent);
    if (iRet)
    {
        return -1;
    }

    if (tInputEvent.iType != INPUT_TYPE_TOUCHSCREEN)
    {
        return -1;
    }

    *ptInputEvent = tInputEvent;

    /* 处理数据 */
    /* 确定触点位于哪一个按钮上 */
    while (atLayout[i].strIconName)
    {
        if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
			 (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
        {
            /* 找到了被点中的按钮 */
            return i;
        }
        else
        {
            i++;
        }
    }

    /* 触点没有落在按钮上 */
    return -1;
}

int Pages_init(void)
{
    int iError;
    iError  = Main_page_init();
    iError |= Setting_page_init();
    iError |= Interval_page_init();
    iError |= Browse_page_init();
    iError |= Auto_page_init();
    iError |= Manual_page_init();
    iError |= Text_page_init();
    return iError;
}

int Time_ms_between(struct timeval tTimeStart, struct timeval tTimeEnd)
{
    int iMS;
    iMS = (tTimeEnd.tv_sec - tTimeStart.tv_sec) * 1000 + (tTimeEnd.tv_usec - tTimeStart.tv_usec) / 1000;
    return iMS;
}

void Get_page_cfg(P_PageCfg ptPageCfg)
{
    Get_selected_dir(ptPageCfg->strSeletedDir);
    Get_interval_time(&ptPageCfg->iIntervalSecond);
}

