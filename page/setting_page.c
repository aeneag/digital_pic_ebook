/*
* Created by Aen on 2021/7/19.
*/

#include <config.h>
#include <page_manager.h>
#include <show_manager.h>
#include <render.h>
#include <draw.h>


static T_Layout g_atSettingPageLayout[] = {
        {0, 0, 0, 0, "select_fold.bmp"},
        {0, 0, 0, 0, "interval.bmp"},
        {0, 0, 0, 0, "return.bmp"},
        {0, 0, 0, 0, NULL},
};
static T_Page_Layout g_tSettingPageLayout = {
        .iMaxTotalBytes = 0,
        .atLayout       = g_atSettingPageLayout,
};

static void  CalcSettingPageLayout(P_Page_Layout ptPageLayout)
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
     *          select_fold.bmp  iHeight
     *                           1/2 * iHeight
     *          interval.bmp     iHeight
     *                           1/2 * iHeight
     *          return.bmp       iHeight
     *                           1/2 * iHeight
     *    ----------------------
     */

    iHeight = iYres * 2 / 10;
    iWidth  = iHeight;
    iStartY = iHeight / 2;

    /* select_foldͼ�� */
    atLayout[0].iTopLeftY  = iStartY;
    atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
    atLayout[0].iTopLeftX  = (iXres - iWidth * 2) / 2;
    atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth * 2 - 1;

    iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }


    /* intervalͼ�� */
    atLayout[1].iTopLeftY  = atLayout[0].iBotRightY + iHeight / 2 + 1;
    atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
    atLayout[1].iTopLeftX  = (iXres - iWidth * 2) / 2;
    atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth * 2 - 1;

    iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

    /* returnͼ�� */
    atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + iHeight / 2 + 1;
    atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
    atLayout[2].iTopLeftX  = (iXres - iWidth) / 2;
    atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;

    iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

}

static void Show_setting_page(P_Page_Layout ptPageLayout)
{
    P_Video_Mem ptVideoMem;
    int iError;
    P_Layout atLayout = ptPageLayout->atLayout;
    /* 1. ����Դ� */
    ptVideoMem = Get_video_mem(ID("setting"), 1);
    if (ptVideoMem == NULL)
    {
        DBG_PRINTF("can't get video mem for setting page!\n");
        return;
    }

    /* 2. �軭���� */
    if (atLayout[0].iTopLeftX == 0)
    {
        CalcSettingPageLayout(ptPageLayout);
    }

    iError = Generate_page(ptPageLayout, ptVideoMem);

    /* 3. ˢ���豸��ȥ */
    Flush_video_mem_to_device(ptVideoMem);

    /* 4. ����Դ� */
    Put_video_mem(ptVideoMem);
}



static int Setting_page_get_input_event(P_Page_Layout atLayout, P_Input_event ptInputEvent)
{
    return Generic_get_input_event(atLayout,ptInputEvent);
}


static void Setting_page_run(P_Page_Params ptParentPageParams)
{
    int iIndex;
    T_Input_event  tInputEvent;
    int bPressed = 0;
    int iIndexPressed = -1;
    T_Page_Params tPageParams;
    tPageParams.iPageID = ID("setting");
    /* 1. ��ʾҳ�� */
    Show_setting_page(&g_tSettingPageLayout);

    /* 2. ����Prepare�߳� */

    /* 3. ����GetInputEvent��������¼����������� */
    while (1)
    {
        iIndex = Setting_page_get_input_event(&g_tSettingPageLayout, &tInputEvent);
        if (tInputEvent.iPressure == 0)
        {
            /* ������ɿ� */
            if (bPressed)
            {
                /* �����а�ť������ */
                Release_button(&g_atSettingPageLayout[iIndexPressed]);
                bPressed = 0;

                if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
                {
                    switch (iIndexPressed)
                    {
                        case 0: /* "ѡ��Ŀ¼"��ť */
                        {
                            /* browseҳ���Run��������:
                      * 0 - ���ڹۿ�ͼƬ
                      * 1 - �������/ѡ���ļ���, ����ļ��޷�Ӧ
                      */
                            Page("browse")->Run(&tPageParams);
                            Show_setting_page(&g_tSettingPageLayout);
                            break;
                        }
                        case 1: /* interval��ť */
                        {
                            Page("interval")->Run(&tPageParams);
                            Show_setting_page(&g_tSettingPageLayout);
                            break;
                        }
                        case 2: /* ���ذ�ť */
                        {
                            return;
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
            /* ����״̬ */
            if (iIndex != -1)
            {
                if (!bPressed)
                {
                    /* δ�����°�ť */
                    bPressed = 1;
                    iIndexPressed = iIndex;
                    Press_button(&g_atSettingPageLayout[iIndexPressed]);
                }
            }
        }
    }
}

static T_Page_action g_tSettingPageAction = {
        .name            = "setting",
        .Run             = Setting_page_run,
        .Get_input_event = Setting_page_get_input_event,
        //.Prepare       = MainPagePrepare;
};

int Setting_page_init(void)
{
    return Register_page_action(&g_tSettingPageAction);
}



