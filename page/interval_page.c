/*
* Created by Aen on 2021/7/19.
*/
#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <fonts_manager.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <draw.h>
static T_Layout g_atIntervalPageIconsLayout[] = {
        {0, 0, 0, 0, "inc.bmp"},
        {0, 0, 0, 0, "time.bmp"},
        {0, 0, 0, 0, "dec.bmp"},
        {0, 0, 0, 0, "ok.bmp"},
        {0, 0, 0, 0, "cancel.bmp"},
        {0, 0, 0, 0, NULL},
};

/* ��ʾ���ֵĵط� */
static T_Layout g_tIntervalNumberLayout;

static int g_iIntervalSecond = 7;

static T_Page_Layout g_tIntervalPageLayout = {
        .iMaxTotalBytes = 0,
        .atLayout       = g_atIntervalPageIconsLayout,
};


void Get_interval_time(int *piIntervalSecond)
{
    *piIntervalSecond = g_iIntervalSecond;
}


/* �����ͼ������ֵ */
static void  CalcIntervalPageLayout(P_Page_Layout ptPageLayout)
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
     *                          1/2 * iHeight
     *          inc.bmp         iHeight * 28 / 128
     *         time.bmp         iHeight * 72 / 128
     *          dec.bmp         iHeight * 28 / 128
     *                          1/2 * iHeight
     *    ok.bmp     cancel.bmp 1/2 * iHeight
     *                          1/2 * iHeight
     *    ----------------------
     */
    iHeight = iYres / 3;
    iWidth  = iHeight;
    iStartY = iHeight / 2;

    /* incͼ�� */
    atLayout[0].iTopLeftY  = iStartY;
    atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight * 28 / 128 - 1;
    atLayout[0].iTopLeftX  = (iXres - iWidth * 52 / 128) / 2;
    atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth * 52 / 128 - 1;

    iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

    /* timeͼ�� */
    atLayout[1].iTopLeftY  = atLayout[0].iBotRightY + 1;
    atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight * 72 / 128 - 1;
    atLayout[1].iTopLeftX  = (iXres - iWidth) / 2;
    atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;
    iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

    /* decͼ�� */
    atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + 1;
    atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight * 28 / 128 - 1;
    atLayout[2].iTopLeftX  = (iXres - iWidth * 52 / 128) / 2;
    atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth * 52 / 128 - 1;
    iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

    /* okͼ�� */
    atLayout[3].iTopLeftY  = atLayout[2].iBotRightY + iHeight / 2 + 1;
    atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight / 2 - 1;
    atLayout[3].iTopLeftX  = (iXres - iWidth) / 3;
    atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth / 2 - 1;
    iTmpTotalBytes = (atLayout[3].iBotRightX - atLayout[3].iTopLeftX + 1) * (atLayout[3].iBotRightY - atLayout[3].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

    /* okͼ�� */
    atLayout[4].iTopLeftY  = atLayout[3].iTopLeftY;
    atLayout[4].iBotRightY = atLayout[3].iBotRightY;
    atLayout[4].iTopLeftX  = atLayout[3].iTopLeftX * 2 + iWidth/2;
    atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth/2 - 1;
    iTmpTotalBytes = (atLayout[4].iBotRightX - atLayout[4].iTopLeftX + 1) * (atLayout[4].iBotRightY - atLayout[4].iTopLeftY + 1) * iBpp / 8;
    if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
    {
        ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
    }

    /* ������ʾ���ֵ�����Ƚ�����, ��������
     * time.bmpԭͼ��СΪ128x72, ������������ִ�СΪ52x40
     * ����CalcIntervalPageLayout����������
     */
    iWidth  = atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1;
    iHeight = atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1;

    g_tIntervalNumberLayout.iTopLeftX  = atLayout[1].iTopLeftX + (128 - 52) / 2 * iWidth / 128;
    g_tIntervalNumberLayout.iBotRightX = atLayout[1].iBotRightX - (128 - 52) / 2 * iWidth / 128 + 1;

    g_tIntervalNumberLayout.iTopLeftY  = atLayout[1].iTopLeftY + (72 - 40) / 2 * iHeight / 72;
    g_tIntervalNumberLayout.iBotRightY = atLayout[1].iBotRightY - (72 - 40) / 2 * iHeight / 72 + 1;

}

static int Interval_page_get_input_event(P_Page_Layout atLayout, P_Input_event ptInputEvent)
{
    return Generic_get_input_event(atLayout, ptInputEvent);
}

/* ����ͼ��: ����ͼ���е����� */
static int GenerateIntervalPageSpecialIcon(int dwNumber, P_Video_Mem ptVideoMem)
{
    unsigned int dwFontSize;
    char strNumber[3];
    int iError;


    dwFontSize = g_tIntervalNumberLayout.iBotRightY - g_tIntervalNumberLayout.iTopLeftY;
    //DBG_PRINTF("dwFontSize = %d\n", dwFontSize);
    Set_font_size(dwFontSize);

    /* ��ʾ��λ����: 00~59 */
    if (dwNumber > 59)
    {
        return -1;
    }

    snprintf(strNumber, 3, "%02d", dwNumber);
    //DBG_PRINTF("strNumber = %s, len = %d\n", strNumber, strlen(strNumber));

    iError = Merger_string_to_center_of_rectangle_in_video_mem(g_tIntervalNumberLayout.iTopLeftX, g_tIntervalNumberLayout.iTopLeftY, g_tIntervalNumberLayout.iBotRightX, g_tIntervalNumberLayout.iBotRightY, (unsigned char *)strNumber, ptVideoMem);

    return iError;
}

static void Show_interval_page(P_Page_Layout ptPageLayout)
{
    P_Video_Mem ptVideoMem;
    int iError;
    P_Layout atLayout = ptPageLayout->atLayout;

    /* 1. ����Դ� */
    ptVideoMem = Get_video_mem(ID("interval"), 1);
    if (ptVideoMem == NULL)
    {
        DBG_PRINTF("can't get video mem for interval page!\n");
        return;
    }

    /* 2. �軭���� */
    /* �����û�м������ͼ������� */
    if (atLayout[0].iTopLeftX == 0)
    {
        CalcIntervalPageLayout(ptPageLayout);
    }

    iError = Generate_page(ptPageLayout, ptVideoMem);

    iError = GenerateIntervalPageSpecialIcon(g_iIntervalSecond, ptVideoMem);
    if (iError)
    {
        DBG_PRINTF("GenerateIntervalPageSpecialIcon error!\n");
    }

    /* 3. ˢ���豸��ȥ */
    Flush_video_mem_to_device(ptVideoMem);

    /* 4. ����Դ� */
    Put_video_mem(ptVideoMem);
}

static void Interval_page_run(P_Page_Params ptParentPageParams)
{
    int iIndex;
    T_Input_event tInputEvent;
    T_Input_event tInputEventPrePress;
    int bPressed = 0;
    int iIndexPressed = -1;
    int iIntervalSecond = g_iIntervalSecond;
    P_Video_Mem ptDevVideoMem;
    int bFast = 0;  /* ��ʾ���ټӼ� */

    ptDevVideoMem = Get_dev_video_mem();

    /* ������ֻ��Ϊ�˱�����뾯�� */
    tInputEventPrePress.tTime.tv_sec = 0;
    tInputEventPrePress.tTime.tv_usec = 0;

    /* 1. ��ʾҳ�� */
    Show_interval_page(&g_tIntervalPageLayout);

    /* 2. ����Prepare�߳� */

    /* 3. ����GetInputEvent��������¼����������� */
    tInputEvent.iPressure = 0; /* ������뾯�� */
    while (1)
    {
        iIndex =  Interval_page_get_input_event(&g_tIntervalPageLayout, &tInputEvent);
        if (tInputEvent.iPressure == 0)
        {
            /* ������ɿ� */
            if (bPressed)
            {
                bFast = 0;

                /* �����а�ť������ */
                Release_button(&g_atIntervalPageIconsLayout[iIndexPressed]);
                //printf(" Release_button");
                bPressed = 0;

                if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
                {
                    switch (iIndexPressed)
                    {
                        case 0: /* inc��ť */
                        {
                            iIntervalSecond++;
                            //printf("inc   = %d",iIntervalSecond);
                            if (iIntervalSecond == 60)
                            {
                                iIntervalSecond = 0;
                            }
                            GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
                            break;
                        }
                        case 2: /* dec��ť */
                        {
                            iIntervalSecond--;
                           // Debug_print("dec   = %d",iIntervalSecond);
                            if (iIntervalSecond == -1)
                            {
                                iIntervalSecond = 59;
                            }
                            GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
                            break;
                        }
                        case 3: /* ok��ť */
                        {
                            g_iIntervalSecond = iIntervalSecond;
                            //Debug_print("inc   = %d",iIntervalSecond);
                            return;
                            break;
                        }
                        case 4: /* cancel��ť */
                        {
                            return;
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
            /* ����״̬ */
            if (iIndex != -1)
            {
                if (!bPressed && (iIndex != 1))
                {
                    /* δ�����°�ť */
                    bPressed = 1;
                    iIndexPressed = iIndex;
                    tInputEventPrePress = tInputEvent;  /* ��¼���� */
                    Press_button(&g_atIntervalPageIconsLayout[iIndexPressed]);
                }

                /* ������µ���"inc.bmp"��"dec.bmp"
                 * ����2���, �ɿ�ĵ������С: ÿ50ms�仯һ��
                 */
                if ((iIndexPressed == 0) || (iIndexPressed == 2))
                {
                    if (bFast && (Time_ms_between(tInputEventPrePress.tTime, tInputEvent.tTime) > 50))
                    {
                        iIntervalSecond = iIndexPressed ? (iIntervalSecond - 1) : (iIntervalSecond + 1);
                        if (iIntervalSecond == 60)
                        {
                            iIntervalSecond = 0;
                        }
                        else if (iIntervalSecond == -1)
                        {
                            iIntervalSecond = 59;
                        }
                        GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
                        tInputEventPrePress = tInputEvent;
                    }

                    if (Time_ms_between(tInputEventPrePress.tTime, tInputEvent.tTime) > 2000)
                    {
                        bFast = 1;
                        tInputEventPrePress = tInputEvent;
                    }

                }
            }
        }
    }
}

static T_Page_action g_tIntervalPageAction = {
        .name          = "interval",
        .Run           = Interval_page_run,
        .Get_input_event = Interval_page_get_input_event,
        //.Prepare       = MainPagePrepare;
};

int Interval_page_init(void)
{
    return Register_page_action(&g_tIntervalPageAction);
}





