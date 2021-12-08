/*
* Created by Aen on 2021/7/17.
*/
#include <config.h>
#include <input_manager.h>
#include <stdlib.h>

#include <tslib.h>
#include <draw.h>


static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;


/* 注意: 由于要用到LCD的分辨率, 此函数要在SelectAndInitDisplay之后调用 */
static int TouchScreen_device_init(void)
{
    char *pcTSName = NULL;

    if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL )
    {
        g_tTSDev = ts_open(pcTSName, 0);  /* 以阻塞方式打开 */
    }
    else
    {
        g_tTSDev = ts_open("/dev/event0", 1);
    }

    if (!g_tTSDev) {
        DBG_PRINTF("ts_open error!\n");
        return -1;
    }

    if (ts_config(g_tTSDev)) {
        DBG_PRINTF("ts_config error!\n");
        return -1;
    }

    if (Get_show_resolution(&giXres, &giYres))
    {
        return -1;
    }

    return 0;
}

static int TouchScreen_device_exit(void)
{
    return 0;
}
#if 0
static int isOutOf500ms(struct timeval *ptPreTime, struct timeval *ptNowTime)
{
    int iPreMs;
    int iNowMs;

    iPreMs = ptPreTime->tv_sec * 1000 + ptPreTime->tv_usec / 1000;
    iNowMs = ptNowTime->tv_sec * 1000 + ptNowTime->tv_usec / 1000;

    return (iNowMs > iPreMs + 500);
}
#endif
static int TouchScreen_get_input_event(P_Input_event ptInputEvent)
{
    struct ts_sample tSamp;
    struct ts_sample tSampPressed;
    struct ts_sample tSampReleased;

    int iRet;
    int bStart = 0;
    int tSPR;

    //static struct timeval tPreTime;
    while(1){
        iRet = ts_read(g_tTSDev, &tSamp, 1); /* 如果无数据则休眠 */
        if(iRet == 1){
            if(tSamp.pressure > 0 ){
                if(bStart == 0){
                    tSampPressed = tSamp;
                    bStart = 1;
                }
            }else{
                tSampReleased = tSamp;

                if(!bStart){
                    return  -1;
                }else{
                    tSPR = tSampPressed.x - tSampReleased.x;
                    ptInputEvent->tTime = tSamp.tv;
                    ptInputEvent->iType = INPUT_TYPE_TOUCHSCREEN;
                    if(tSPR >giXres/10 || tSPR < 0 - giXres / 10) {
                        if (tSPR > giXres / 5) {
                            ptInputEvent->iVal = INPUT_VALUE_DOWN;
                        } else if (tSPR < 0 - giXres / 5) {
                            ptInputEvent->iVal = INPUT_VALUE_UP;
                        } else {
                            ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
                        }
                    }else {
                        if (tSampPressed.x < giXres / 5) {
                            ptInputEvent->iVal = INPUT_VALUE_UP;
                        } else if (tSampPressed.x > 4 * giXres / 5) {
                            ptInputEvent->iVal = INPUT_VALUE_DOWN;
                        } else {
                            ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
                        }
                    }
                    return 0;
                }
            }
        }else{
            return -1;
        }

    }


#if 0
    struct ts_sample tSamp;
    int iRet;

    static struct timeval tPreTime;

    iRet = ts_read(g_tTSDev, &tSamp, 1); /* 如果无数据则休眠 */

    if (iRet < 0) {
        return -1;
    }

    /* 处理数据 */
    if (isOutOf500ms(&tPreTime, &tSamp.tv))
    {
        /* 如果此次触摸事件发生的时间, 距上次事件超过了500ms */
        tPreTime = tSamp.tv;
        ptInputEvent->tTime = tSamp.tv;
        ptInputEvent->iType = INPUT_TYPE_TOUCHSCREEN;

        if (tSamp.y < giYres/3)
        {
            ptInputEvent->iVal = INPUT_VALUE_UP;
        }
        else if (tSamp.y > 2*giYres/3)
        {
            ptInputEvent->iVal = INPUT_VALUE_DOWN;
        }
        else
        {
            ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
        }
        return 0;

    }
    else
    {
        return -1;
    }
#endif
    return 0;
}
static T_Input_ops g_t_touchScreen_ops = {
        .name          = "touchscreen",
        .Device_init    = TouchScreen_device_init,
        .Device_exit    = TouchScreen_device_exit,
        .Get_input_event = TouchScreen_get_input_event,
};

int TouchScreen_init(void)
{
    return Register_input_ops(&g_t_touchScreen_ops);
}

