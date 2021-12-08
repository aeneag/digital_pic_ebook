/*
* Created by Aen on 2021/7/19.
*/
#include <config.h>
#include <page_manager.h>
#include <show_manager.h>
#include <render.h>
#include <draw.h>

static T_Layout g_atMainPageLayout[] = {
        {0, 0, 0, 0, "browse_mode.bmp"},
        {0, 0, 0, 0, "continue_mod.bmp"},
        {0, 0, 0, 0, "setting.bmp"},
        {0, 0, 0, 0, NULL},
};
static T_Page_Layout g_tMainPageLayout = {
        .iMaxTotalBytes = 0,
        .atLayout       = g_atMainPageLayout,
};
/* 计算各图标坐标值 */
static void  CalcMainPageLayout(P_Page_Layout ptPageLayout)
{
    int iStartY;
    int iWidth;
    int iHeight;
    int iXres, iYres, iBpp;
    int iTmpTotalBytes;
    P_Layout atLayout;

    atLayout = ptPageLayout->atLayout;
    Get_show_resolution(&iXres, &iYres, &iBpp);
    ptPageLayout->iBpp = iBpp;

    /*
     *    ----------------------
     *                           1/2 * iHeight
     *          browse_mode.bmp  iHeight
     *                           1/2 * iHeight
     *         continue_mod.bmp     iHeight
     *                           1/2 * iHeight
     *          setting.bmp       iHeight
     *                           1/2 * iHeight
     *    ----------------------
     */

    iHeight = iYres * 2 / 10;
    iWidth  = iHeight;
    iStartY = iHeight / 2;

    /* select_fold图标 */
    atLayout[0].iTopLeftY  = iStartY;
    atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
    atLayout[0].iTopLeftX  = (iXres - iWidth * 2) / 2;
    atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth * 2 - 1;

    iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }


    /* interval图标 */
    atLayout[1].iTopLeftY  = atLayout[0].iBotRightY + iHeight / 2 + 1;
    atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
    atLayout[1].iTopLeftX  = (iXres - iWidth * 2) / 2;
    atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth * 2 - 1;

    iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

    /* return图标 */
    atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + iHeight / 2 + 1;
    atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
    atLayout[2].iTopLeftX  = (iXres - iWidth * 2) / 2;
    atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth * 2 - 1;

    iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

}
static void Show_main_page(P_Page_Layout ptPageLayout){
    P_Video_Mem ptVideoMem;
    int iError;
    P_Layout atLayout = ptPageLayout->atLayout;
    /* 1. 获得显存 */
    ptVideoMem = Get_video_mem(ID("main"), 1);
    if (ptVideoMem == NULL)
    {
        DBG_PRINTF("can't get video mem for main page!\n");
        return;
    }

    /* 2. 描画数据 */
    if (atLayout[0].iTopLeftX == 0)
    {
        CalcMainPageLayout(ptPageLayout);
    }

    iError = Generate_page(ptPageLayout, ptVideoMem);

    Flush_video_mem_to_device(ptVideoMem);

    Put_video_mem(ptVideoMem);
}


int Main_page_get_input_event(P_Page_Layout atLayout, P_Input_event ptInputEvent)
{
    return Generic_get_input_event(atLayout, ptInputEvent);
}
static void Main_page_run(P_Page_Params ptParentPageParams)
{
    int iIndex;
    T_Input_event tInputEvent;
    int bPressed = 0;
    int iIndexPressed = -1;
    T_Page_Params tPageParams;
    tPageParams.iPageID = ID("main");

    /* 1. 显示页面 */
    Show_main_page(&g_tMainPageLayout);
    /* 2. 创建Prepare线程 */

    /* 3. 调用GetInputEvent获得输入事件，进而处理 */
    while (1)
    {
        iIndex = Main_page_get_input_event(&g_tMainPageLayout, &tInputEvent);
        if (tInputEvent.iPressure == 0)
        {
            /* 如果是松开 */
            if (bPressed)
            {
                /* 曾经有按钮被按下 */
                Release_button(&g_atMainPageLayout[iIndexPressed]);
                bPressed = 0;

                if (iIndexPressed == iIndex) /* 按下和松开都是同一个按钮 */
                {
                    switch (iIndexPressed)
                    {
                        case 0: /* 浏览按钮 */
                        {
                            /* tPageParams.strCurPictureFile[0]为'\0'时, 使用设置页面里选定的目录来显示 */
                            tPageParams.strCurPictureFile[0] = '\0';
                            Page("manual")->Run(&tPageParams);

                            /* 从设置页面返回后显示当首的主页面 */
                            Show_main_page(&g_tMainPageLayout);

                            break;
                        }
                        case 1: /* 连播按钮 */
                        {
                            Page("auto")->Run(&tPageParams);
                            /* 从设置页面返回后显示当首的主页面 */
                            Show_main_page(&g_tMainPageLayout);

                            break;
                        }
                        case 2: /* 设置按钮 */
                        {
                            Page("setting")->Run(&tPageParams);

                            /* 从设置页面返回后显示当首的主页面 */
                            Show_main_page(&g_tMainPageLayout);

                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }

                iIndexPressed = -1;
            }
        }
        else
        {
            /* 按下状态 */
            if (iIndex != -1)
            {
                if (!bPressed)
                {
                    /* 未曾按下按钮 */
                    bPressed = 1;
                    iIndexPressed = iIndex;
                    Press_button(&g_atMainPageLayout[iIndexPressed]);
                }
            }
        }
    }
}
static T_Page_action g_t_main_page_action = {
        .name           = "main",
        .Run            = Main_page_run,
        .Get_input_event = Main_page_get_input_event,
      //  .Prepare       = MainPagePrepare;
};


int Main_page_init(void)
{
    return Register_page_action(&g_t_main_page_action);
}
