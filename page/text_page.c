#include <page_manager.h>
#include <draw.h>
#include <config.h>

/*
* Created by Aen on 2021/7/26.
*/
#define AC_HZk_FILE  "HZK16"
#define AC_FREETYPE_FILE " ./MSYH.TTF"
#define FONT_SIZE 28
static int giXres;
static int giYres;
static int giBpp;
static int Open_text_page(P_Page_Params ptParentPageParams){
    int iError;
    iError = OpenTextFile(ptParentPageParams->strCurPictureFile);
    if (iError)
    {
        DBG_PRINTF("OpenTextFile error!\n");
        return -1;
    }
    iError = SetTextDetail(AC_HZk_FILE, AC_FREETYPE_FILE, FONT_SIZE);
    if (iError)
    {
        DBG_PRINTF("SetTextDetail error!\n");
        return -1;
    }
    if (Get_show_resolution(&giXres, &giYres, &giBpp))
    {
        return -1;
    }
    return 0;
}
static int TextPageGetInputEvent(P_Page_Layout atLayout,P_Input_event ptInputEvent)
{
    T_Input_event tInputEvent;
    int iRet;
    iRet = Get_input_event(&tInputEvent);
    if(!iRet){
        *ptInputEvent = tInputEvent;
        printf("ival == %d\n",ptInputEvent->iVal);
        printf("itype == %d\n",ptInputEvent->iType);
        return 1;
    }
    return 0;
}


/* manual页面的Run参数含义:
 * 如果它的父页面是主页面   - 显示browse页面,
 * 否则 - 显示图片
 */
static void Text_page_run(P_Page_Params ptParentPageParams){
    int iError;
    iError = Open_text_page(ptParentPageParams);
    if (iError)
    {
        DBG_PRINTF("Error to open file \n");
    }

    iError = ShowNextPage();
    DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    if (iError)
    {
        DBG_PRINTF("Error to show first page\n");
    }


    int bStart = 0;
    int tSPR;
    int get_pressed_X = 0;
    int get_released_X = 0;
    T_Input_event ptInputEvent;
    ptInputEvent.iPressure = 0; /* 避免编译警告 */
    ptInputEvent.iType = 0;
    ptInputEvent.iX = 0;
    ptInputEvent.iVal = 0;
    while(1) {
        TextPageGetInputEvent(NULL,&ptInputEvent);

        if (ptInputEvent.iType == INPUT_TYPE_TOUCHSCREEN) {
            printf("x y  %d   %d          ",ptInputEvent.iX,ptInputEvent.iY);
            if (ptInputEvent.iPressure > 0) {
                if (bStart == 0) {
                    get_pressed_X = ptInputEvent.iX;
                    bStart = 1;
                }
            } else {
                get_released_X = ptInputEvent.iX;

                if(bStart == 1) {
                    tSPR = get_pressed_X - get_released_X;
                    if (tSPR > giXres / 10 || tSPR < 0 - giXres / 10) {
                        if (tSPR > giXres / 5) {
                            //ptInputEvent.iVal = INPUT_VALUE_DOWN;
                            ShowNextPage();
                        } else if (tSPR < 0 - giXres / 5) {
                           // ptInputEvent.iVal = INPUT_VALUE_UP;
                            ShowPrePage();
                        } else {
                            ptInputEvent.iVal = INPUT_VALUE_UNKNOWN;
                        }
                    } else {
                        if (get_pressed_X < giXres / 5) {
                            //ptInputEvent.iVal = INPUT_VALUE_UP;
                            ShowPrePage();
                        } else if (get_pressed_X > 4 * giXres / 5) {
                            //ptInputEvent.iVal = INPUT_VALUE_DOWN;
                            ShowNextPage();
                        } else {
                            ptInputEvent.iVal = INPUT_VALUE_UNKNOWN;
                        }
                    }
                    get_pressed_X = 0;
                    get_released_X = 0;
                    bStart = 0;

                }
            }
        }
        if (ptInputEvent.iType == INPUT_TYPE_BUTTON) {
            if (ptInputEvent.iVal == 0x81) {
                ShowPrePage();
            } else if (ptInputEvent.iVal == 0x82) {
                ShowNextPage();
            } else if (ptInputEvent.iVal == 0x83) {
                CloseTextFile();
                return;
            } else {
                ptInputEvent.iVal = INPUT_VALUE_UNKNOWN;
            }
        }
    }


}

static T_Page_action g_tTextPageAction = {
        .name               = "text",
        .Run                = Text_page_run,
        .Get_input_event    = TextPageGetInputEvent,
};

int Text_page_init(void)
{
    return Register_page_action(&g_tTextPageAction);
}


