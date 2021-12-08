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
static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* ������ */
static int g_bAutoPlayThreadShouldExit = 0;
static T_PageCfg g_tPageCfg;

/* ��������ȵķ�ʽ���Ŀ¼�µ��ļ�
 * ��: �Ȼ�ö���Ŀ¼�µ��ļ�, �ٽ���һ����Ŀ¼A
 *     �Ȼ��һ����Ŀ¼A�µ��ļ�, �ٽ��������Ŀ¼AA, ...
 *     ������һ����Ŀ¼A��, �ٽ���һ����Ŀ¼B
 *
 * "����ģʽ"�µ��øú������Ҫ��ʾ���ļ�
 * �����ַ��������Щ�ļ�:
 * 1. ����ֻ��Ҫ����һ�κ���,�������ļ������ֱ��浽ĳ����������
 * 2. Ҫʹ���ļ�ʱ�ٵ��ú���,ֻ���浱ǰҪʹ�õ��ļ�������
 * ��1�ַ����Ƚϼ�,���ǵ��ļ��ܶ�ʱ�п��ܵ����ڴ治��.
 * ����ʹ�õ�2�ַ���:
 * ����ĳĿ¼(����������Ŀ¼)�����е��ļ���������һ����
 * g_iStartNumberToRecord : �ӵڼ����ļ���ʼȡ�����ǵ�����
 * g_iCurFileNumber       : ���κ���ִ��ʱ�����ĵ�1���ļ��ı��
 * g_iFileCountHaveGet    : �Ѿ��õ��˶��ٸ��ļ�������
 * g_iFileCountTotal      : ÿһ���ܹ�Ҫȡ�����ٸ��ļ�������
 * g_iNextProcessFileIndex: ��g_apstrFileNames�����м���Ҫ��ʾ��LCD�ϵ��ļ�
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
            /* �ٴδ�ͷ����(����ģʽ��ѭ����ʾ) */
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

/* bCur = 1 ��ʾ������videomem, ��Ϊ�������Ͼ�Ҫ��LCD����ʾ������
 * bCur = 0 ��ʾ������׼���õ�, �п����޷����videomem
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

    /* ����Դ� */
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

    /* ��ͼƬ���������ŵ�LCD��Ļ��, ������ʾ */
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

    /* ���������ʾʱ���Ͻ����� */
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
        /* 1. ���ж��Ƿ�Ҫ�˳� */
        pthread_mutex_lock(&g_tAutoPlayThreadMutex);
        bExit = g_bAutoPlayThreadShouldExit;
        pthread_mutex_unlock(&g_tAutoPlayThreadMutex);

        if (bExit)
        {
            return NULL;
        }

        /* 2. ׼��Ҫ��ʾ��ͼƬ */
        ptVideoMem = PrepareNextPicture(0);

        /* 3. ʱ�䵽�����ʾ���� */

        if (!bFirst)
        {
            sleep(g_tPageCfg.iIntervalSecond);       /* �������������� */
        }
        bFirst = 0;

        if (ptVideoMem == NULL)
        {
            ptVideoMem = PrepareNextPicture(1);
        }

        /* ˢ���豸��ȥ */
        Flush_video_mem_to_device(ptVideoMem);

        /* ����Դ� */
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

    /* �������ֵ: ��ʾ��һ��Ŀ¼, ��ʾͼƬ�ļ�� */
    Get_page_cfg(&g_tPageCfg);

    /* ���ptParentPageParams.strCurPictureFile[0] != '\0'
    * �Զ��������Ŀ¼�µ�ͼƬ: ptParentPageParams.strCurPictureFile
    */
    if (ptParentPageParams->strCurPictureFile[0] != '\0')
    {
        strcpy(g_tPageCfg.strSeletedDir, ptParentPageParams->strCurPictureFile);
        pcTmp = strrchr(g_tPageCfg.strSeletedDir, '/');
        *pcTmp = '\0';
    }

    /* 1. ����һ���߳���������ʾͼƬ */
    pthread_create(&g_tAutoPlayThreadID, NULL, AutoPlayThreadFunction, NULL);

    /* 2. ��ǰ�̵߳ȴ�����������, �����򵥵�: �������˴�����, ���߳��˳� */
    while (1)
    {
        iRet = Get_input_event(&tInputEvent);
        if (0 == iRet)
        {
            pthread_mutex_lock(&g_tAutoPlayThreadMutex);
            g_bAutoPlayThreadShouldExit = 1;   /* AutoPlayThreadFunction�̼߳�⵽�������Ϊ1����˳� */
            pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
            pthread_join(g_tAutoPlayThreadID, NULL);  /* �ȴ����߳��˳� */
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


