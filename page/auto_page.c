/*
* Created by Aen on 2021/7/19.
*/
#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>
#include <unistd.h>
#include <draw.h>
static pthread_t g_tAutoPlayThreadID;
static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* 互斥量 */
static int g_bAutoPlayThreadShouldExit = 0;
static T_PageCfg g_tPageCfg;

/* 以深度优先的方式获得目录下的文件
 * 即: 先获得顶层目录下的文件, 再进入一级子目录A
 *     先获得一级子目录A下的文件, 再进入二级子目录AA, ...
 *     处理完一级子目录A后, 再进入一级子目录B
 *
 * "连播模式"下调用该函数获得要显示的文件
 * 有两种方法获得这些文件:
 * 1. 事先只需要调用一次函数,把所有文件的名字保存到某个缓冲区中
 * 2. 要使用文件时再调用函数,只保存当前要使用的文件的名字
 * 第1种方法比较简单,但是当文件很多时有可能导致内存不足.
 * 我们使用第2种方法:
 * 假设某目录(包括所有子目录)下所有的文件都给它编一个号
 * g_iStartNumberToRecord : 从第几个文件开始取出它们的名字
 * g_iCurFileNumber       : 本次函数执行时读到的第1个文件的编号
 * g_iFileCountHaveGet    : 已经得到了多少个文件的名字
 * g_iFileCountTotal      : 每一次总共要取出多少个文件的名字
 * g_iNextProcessFileIndex: 在g_apstrFileNames数组中即将要显示在LCD上的文件
 *
 */
static int g_iStartNumberToRecord = 0;
static int g_iCurFileNumber = 0;
static int g_iFileCountHaveGet = 0;
static int g_iFileCountTotal = 0;
static int g_iNextProcessFileIndex = 0;

#define FILE_COUNT 10
static char g_apstrFileNames[FILE_COUNT][256];

static void ResetAutoPlayFile(void)
{
    g_iStartNumberToRecord = 0;
    g_iCurFileNumber = 0;
    g_iFileCountHaveGet = 0;
    g_iFileCountTotal = 0;
    g_iNextProcessFileIndex = 0;
}

static int GetNextAutoPlayFile(char *strFileName)
{
    int iError;
//int jj;
    if (g_iNextProcessFileIndex < g_iFileCountHaveGet)
    { //printf("55555");
        strncpy(strFileName, g_apstrFileNames[g_iNextProcessFileIndex], 256);
//        for(jj=0;jj<16;jj++){
//            printf("%c",g_apstrFileNames[g_iNextProcessFileIndex][jj]);
//        }
//        printf("\n");
        g_iNextProcessFileIndex++;
        return 0;
    }
    else
    {
        g_iCurFileNumber    = 0;
        g_iFileCountHaveGet = 0;
        g_iFileCountTotal   = FILE_COUNT;
        g_iNextProcessFileIndex = 0;
        iError = Get_files_indir(g_tPageCfg.strSeletedDir, &g_iStartNumberToRecord, &g_iCurFileNumber, &g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames);
        if (iError || (g_iNextProcessFileIndex >= g_iFileCountHaveGet))
        {
            /* 再次从头读起(连播模式下循环显示) */
            g_iStartNumberToRecord = 0;
            g_iCurFileNumber    = 0;
            g_iFileCountHaveGet = 0;
            g_iFileCountTotal = FILE_COUNT;
            g_iNextProcessFileIndex = 0;

            iError = Get_files_indir(g_tPageCfg.strSeletedDir, &g_iStartNumberToRecord, &g_iCurFileNumber, &g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames);
        }

        if (iError == 0)
        {
            if (g_iNextProcessFileIndex < g_iFileCountHaveGet)
            {
                strncpy(strFileName, g_apstrFileNames[g_iNextProcessFileIndex], 256);
//                printf("66666");
//                for(jj=0;jj<16;jj++){
//                    printf("%c",strFileName[jj]);
//                }
//                printf("\n");
                g_iNextProcessFileIndex++;
                return 0;
            }
        }
    }

    return -1;
}

/* bCur = 1 表示必须获得videomem, 因为这是马上就要在LCD上显示出来的
 * bCur = 0 表示这是做准备用的, 有可能无法获得videomem
 */
static P_Video_Mem PrepareNextPicture(int bCur)
{
    T_Pixel_Datas tOriginIconPixelDatas;
    T_Pixel_Datas tPicPixelDatas;
    P_Video_Mem ptVideoMem;
    int iError;
    int iXres, iYres, iBpp;
    int iTopLeftX, iTopLeftY;
    float k;
    char strFileName[256];

    Get_show_resolution(&iXres, &iYres, &iBpp);

    /* 获得显存 */
    ptVideoMem = Get_video_mem(-1, bCur);
    if (ptVideoMem == NULL)
    {
        DBG_PRINTF("can't get video mem for browse page!\n");
        return NULL;
    }
    Clear_video_mem(ptVideoMem, COLOR_BACK_GROUND);
//int jj;
    while (1)
    {
        iError = GetNextAutoPlayFile(strFileName); //error
//        for(jj=0;jj<16;jj++){
//            printf("%c",strFileName[jj]);
//        }
//        printf("\n");
        if (iError)
        {
            DBG_PRINTF("GetNextAutoPlayFile error\n");
            Put_video_mem(ptVideoMem);
            return NULL;
        }

        iError = Get_pixel_datas_frm_file(strFileName, &tOriginIconPixelDatas);
        if (0 == iError)
        {
            break;
        }
    }

    /* 把图片按比例缩放到LCD屏幕上, 居中显示 */
    k = (float)tOriginIconPixelDatas.iHeight / tOriginIconPixelDatas.iWidth;
    tPicPixelDatas.iWidth  = iXres;
    tPicPixelDatas.iHeight = iXres * k;
    if (tPicPixelDatas.iHeight > iYres)
    {
        tPicPixelDatas.iWidth  = iYres / k;
        tPicPixelDatas.iHeight = iYres;
    }
    tPicPixelDatas.iBpp        = iBpp;
    tPicPixelDatas.iLineBytes  = tPicPixelDatas.iWidth * tPicPixelDatas.iBpp / 8;
    tPicPixelDatas.iTotalBytes = tPicPixelDatas.iLineBytes * tPicPixelDatas.iHeight;
    tPicPixelDatas.auc_pixel_datas = malloc(tPicPixelDatas.iTotalBytes);
    if (tPicPixelDatas.auc_pixel_datas == NULL)
    {
        Put_video_mem(ptVideoMem);
        return NULL;
    }

    Pic_zoom(&tOriginIconPixelDatas, &tPicPixelDatas);

    /* 算出居中显示时左上角坐标 */
    iTopLeftX = (iXres - tPicPixelDatas.iWidth) / 2;
    iTopLeftY = (iYres - tPicPixelDatas.iHeight) / 2;
    Pic_merge(iTopLeftX, iTopLeftY, &tPicPixelDatas, &ptVideoMem->tPixelDatas);
    Free_pixel_datas_for_icon(&tOriginIconPixelDatas);
    free(tPicPixelDatas.auc_pixel_datas);

    return ptVideoMem;
}

static void *AutoPlayThreadFunction(void *pVoid)
{
    int bFirst = 1;
    int bExit;
    P_Video_Mem ptVideoMem;

    ResetAutoPlayFile();

    while (1)
    {
        /* 1. 先判断是否要退出 */
        pthread_mutex_lock(&g_tAutoPlayThreadMutex);
        bExit = g_bAutoPlayThreadShouldExit;
        pthread_mutex_unlock(&g_tAutoPlayThreadMutex);

        if (bExit)
        {
            return NULL;
        }

        /* 2. 准备要显示的图片 */
        ptVideoMem = PrepareNextPicture(0);

        /* 3. 时间到后就显示出来 */

        if (!bFirst)
        {
            sleep(g_tPageCfg.iIntervalSecond);       /* 先用休眠来代替 */
        }
        bFirst = 0;

        if (ptVideoMem == NULL)
        {
            ptVideoMem = PrepareNextPicture(1);
        }

        /* 刷到设备上去 */
        Flush_video_mem_to_device(ptVideoMem);

        /* 解放显存 */
        Put_video_mem(ptVideoMem);

    }
    return NULL;
}

static void Auto_page_run(P_Page_Params ptParentPageParams)
{
    T_Input_event tInputEvent;
    int iRet;
    char *pcTmp;

    g_bAutoPlayThreadShouldExit = 0;

    /* 获得配置值: 显示哪一个目录, 显示图片的间隔 */
    Get_page_cfg(&g_tPageCfg);

    /* 如果ptParentPageParams.strCurPictureFile[0] != '\0'
    * 自动播放这个目录下的图片: ptParentPageParams.strCurPictureFile
    */
    if (ptParentPageParams->strCurPictureFile[0] != '\0')
    {
        strcpy(g_tPageCfg.strSeletedDir, ptParentPageParams->strCurPictureFile);
        pcTmp = strrchr(g_tPageCfg.strSeletedDir, '/');
        *pcTmp = '\0';
    }

    /* 1. 启动一个线程来连续显示图片 */
    pthread_create(&g_tAutoPlayThreadID, NULL, AutoPlayThreadFunction, NULL);

    /* 2. 当前线程等待触摸屏输入, 先做简单点: 如果点击了触摸屏, 让线程退出 */
    while (1)
    {
        iRet = Get_input_event(&tInputEvent);
        if (0 == iRet)
        {
            pthread_mutex_lock(&g_tAutoPlayThreadMutex);
            g_bAutoPlayThreadShouldExit = 1;   /* AutoPlayThreadFunction线程检测到这个变量为1后会退出 */
            pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
            pthread_join(g_tAutoPlayThreadID, NULL);  /* 等待子线程退出 */
            return;
        }
    }

}

static T_Page_action g_tAutoPageAction = {
        .name          = "auto",
        .Run           = Auto_page_run,
};

int Auto_page_init(void)
{
    return Register_page_action(&g_tAutoPageAction);
}


