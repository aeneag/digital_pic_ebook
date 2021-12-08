#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <show_manager.h>
#include <string.h>
#include <input_manager.h>
#include <string.h>
#include <debug_manager.h>

#include <pic_analysis.h>
#include <render.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <picfmt_manager.h>

/* digitpic <bmp_file> */

int main(int argc, char **argv)
{
    int iError;
    Debug_init();
    Init_debug_channel();

    Show_init();
    SelectAndInitDisplay("fb");

    Alloc_video_mem(7);

    Input_init();
    All_input_devices_init();

    iError = Fonts_init();
    if (iError)
    {
        DBG_PRINTF("FontsInit error!\n");
    }

    iError = Set_fonts_detail("freetype", argv[1], 24);
    if (iError)
    {
        DBG_PRINTF("SetFontsDetail error!\n");
    }
    iError = Encoding_init();
    if (iError)
    {
        DBG_PRINTF("EncodingInit error!\n");
        return -1;
    }

    Pages_init();
    Pic_fmts_init();
    Page("main")->Run(NULL);

    return 0;
}

#if 0
/* digitpic <bmp_file> */
int main(int argc, char **argv)
{
    int iFdBmp;
    int iRet;
    unsigned char *pucBMPmem;
    struct stat tBMPstat;

    P_Show_Ops ptDispOpr;

    extern T_Pic_File_Parser g_t_bmp_parser;

    T_Pixel_Datas tPixelDatas;
    T_Pixel_Datas tPixelDatasSmall;

    T_Pixel_Datas tPixelDatasFB;

    if (argc != 2)
    {
        printf("%s <bmp_file>\n", argv[0]);
        return -1;
    }

    Debug_init();
    Init_debug_channel();

    Show_init();

    ptDispOpr = Get_show_ops("fb");
    ptDispOpr->Device_init();
    ptDispOpr->Clean_screen(COLOR_BACK_GROUND);


    /* 打开BMP文件 */
    iFdBmp = open(argv[1], O_RDWR);
    if (iFdBmp == -1)
    {
        DBG_PRINTF("can't open %s\n", argv[1]);
    }

    fstat(iFdBmp, &tBMPstat);
    pucBMPmem = (unsigned char *)mmap(NULL , tBMPstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
    if (pucBMPmem == (unsigned char *)-1)
    {
        DBG_PRINTF("mmap error!\n");
        return -1;
    }

    /* 提取BMP文件的RGB数据, 缩放, 在LCD上显示出来 */
    iRet = g_t_bmp_parser.Is_support(pucBMPmem);
    if (iRet == 0)
    {
        DBG_PRINTF("%s is not bmp file\n", argv[1]);
        return -1;
    }

    tPixelDatas.iBpp = ptDispOpr->iBpp;
    iRet = g_t_bmp_parser.Get_pixel_datas(pucBMPmem, &tPixelDatas);
    if (iRet)
    {
        DBG_PRINTF("GetPixelDatas error!\n");
        return -1;
    }

    tPixelDatasFB.iWidth        = ptDispOpr->iXres;
    tPixelDatasFB.iHeight       = ptDispOpr->iYres;
    tPixelDatasFB.iBpp          = ptDispOpr->iBpp;
    tPixelDatasFB.iLineBytes    = ptDispOpr->iXres * ptDispOpr->iBpp / 8;
    tPixelDatasFB.auc_pixel_datas = ptDispOpr->pucShowMem;

    Pic_merge(0, 0, &tPixelDatas, &tPixelDatasFB);

    tPixelDatasSmall.iWidth  = tPixelDatas.iWidth/2;
    tPixelDatasSmall.iHeight = tPixelDatas.iHeight/2;
    tPixelDatasSmall.iBpp    = tPixelDatas.iBpp;
    tPixelDatasSmall.iLineBytes = tPixelDatasSmall.iWidth * tPixelDatasSmall.iBpp / 8;
    tPixelDatasSmall.auc_pixel_datas = malloc(tPixelDatasSmall.iLineBytes * tPixelDatasSmall.iHeight);

    Pic_zoom(&tPixelDatas, &tPixelDatasSmall);
    Pic_merge(128, 128, &tPixelDatasSmall, &tPixelDatasFB);


    return 0;
}
#endif
///* digitpic <bmp_file> */
//int main(int argc, char **argv)
//{
//    int iFdBmp;
//    int iRet;
//    unsigned char *pucBMPmem;
//    struct stat tBMPstat;
//
//    P_Show_Ops ptDispOpr;
//
//    extern T_Pic_File_Parser g_t_bmp_parser;
//
//    T_Pixel_Datas tPixelDatas;
//    T_Pixel_Datas tPixelDatasSmall;
//
//    T_Pixel_Datas tPixelDatasFB;
//
//    if (argc != 2)
//    {
//        printf("%s <bmp_file>\n", argv[0]);
//        return -1;
//    }
//
//    Debug_init();
//    Init_debug_channel();
//
//    Show_init();
//
//    ptDispOpr = Get_show_ops("fb");
//    ptDispOpr->Device_init();
//    ptDispOpr->Clean_screen(0);
//
//
//    /* 打开BMP文件 */
//    iFdBmp = open(argv[1], O_RDWR);
//    if (iFdBmp == -1)
//    {
//        DBG_PRINTF("can't open %s\n", argv[1]);
//    }
//
//    fstat(iFdBmp, &tBMPstat);
//    pucBMPmem = (unsigned char *)mmap(NULL , tBMPstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
//    if (pucBMPmem == (unsigned char *)-1)
//    {
//        DBG_PRINTF("mmap error!\n");
//        return -1;
//    }
//    printf("-----------------\n");
//    /* 提取BMP文件的RGB数据, 缩放, 在LCD上显示出来 */
//    iRet = g_t_bmp_parser.Is_support(pucBMPmem);
//    if (iRet == 0)
//    {
//        DBG_PRINTF("%s is not bmp file\n", argv[1]);
//        return -1;
//    }
//    printf("----------2-------\n");
//    tPixelDatas.iBpp = ptDispOpr->iBpp;
//    iRet = g_t_bmp_parser.Get_pixel_datas(pucBMPmem, &tPixelDatas);
//    if (iRet)
//    {
//        DBG_PRINTF("GetPixelDatas error!\n");
//        return -1;
//    }
//    printf("----------3-------\n");
//    printf("tPixelDatas.iWidth        = %d \n",tPixelDatas.iWidth );
//    tPixelDatasFB.iWidth        = ptDispOpr->iXres;
//    tPixelDatasFB.iHeight       = ptDispOpr->iYres;
//    tPixelDatasFB.iBpp          = ptDispOpr->iBpp;
//    tPixelDatasFB.iLineBytes    = ptDispOpr->iXres * ptDispOpr->iBpp / 8;
//    tPixelDatasFB.auc_pixel_datas = ptDispOpr->pucShowMem;
//    printf("----------4-------\n");
//    PicMerge(0, 0, &tPixelDatas, &tPixelDatasFB);
//    printf("----------4.4-------\n");
//    tPixelDatasSmall.iWidth  = tPixelDatas.iWidth/2;
//    tPixelDatasSmall.iHeight = tPixelDatas.iHeight/2;
//    tPixelDatasSmall.iBpp    = tPixelDatas.iBpp;
//    tPixelDatasSmall.iLineBytes = tPixelDatasSmall.iWidth * tPixelDatasSmall.iBpp / 8;
//    printf("----------4.5-------\n");
//    tPixelDatasSmall.auc_pixel_datas = malloc(tPixelDatasSmall.iLineBytes * tPixelDatasSmall.iHeight);
//    printf("----------5-------\n");
//    PicZoom(&tPixelDatas, &tPixelDatasSmall);
//    PicMerge(128, 128, &tPixelDatasSmall, &tPixelDatasFB);
//
//    printf("----------end-------\n");
//    return 0;
//}

#if 0
/* ./show_file [-s Size] [-f freetype_font_file] [-h HZK] <text_file> */
int main(int argc, char **argv)
{
	int iError;
	unsigned int dwFontSize = 16;//default parameters
	char acHzkFile[128];
	char acFreetypeFile[128];
	char acTextFile[128];

	char acDisplay[128];


	int bList = 0;

    T_Input_event tInputEvent;

	acHzkFile[0]  = '\0';
	acFreetypeFile[0] = '\0';
	acTextFile[0] = '\0';

	strcpy(acDisplay, "fb");//init show lcd default parameters

    iError = Debug_init();
    if (iError)
    {
        DBG_PRINTF("Debug init error!\n");
        return -1;
    }
    Init_debug_channel();

	while ((iError = getopt(argc, argv, "ls:f:h:d:")) != -1)
	{
		switch(iError)
		{
			case 'l':
			{
				  bList = 1;
				  break;
			}
			case 's':
			{
				  dwFontSize = strtoul(optarg, NULL, 0);
				  break;
			}
			case 'f':
			{
				  strncpy(acFreetypeFile, optarg, 128);
				  acFreetypeFile[127] = '\0';
				  break;
			}			
			case 'h':
			{
					strncpy(acHzkFile, optarg, 128);
					acHzkFile[127] = '\0';
					break;
			}
			case 'd':
			{
				strncpy(acDisplay, optarg, 128);
				acDisplay[127] = '\0';
				break;
			}
			default:
			{
					DBG_PRINTF("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
					DBG_PRINTF("Usage: %s -l\n", argv[0]);
					return -1;
					break;
			}
		}
	}

	if (!bList && (optind >= argc))
	{
		DBG_PRINTF("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
		DBG_PRINTF("Usage: %s -l\n", argv[0]);
		return -1;
	}
		
	iError = Show_init();
	if (iError)
	{
		DBG_PRINTF("DisplayInit error!\n");
		return -1;
	}

	iError = Fonts_init();
	if (iError)
	{
		DBG_PRINTF("FontsInit error!\n");
		return -1;
	}

	iError = Encoding_init();
	if (iError)
	{
		DBG_PRINTF("EncodingInit error!\n");
		return -1;
	}

    iError = Input_init();
    if (iError)
    {
        DBG_PRINTF("InputInit error!\n");
        return -1;
    }


    if (bList)
	{
		DBG_PRINTF("supported display:\n");
		Show_ops();

		DBG_PRINTF("supported font:\n");
		Show_font_ops();

		DBG_PRINTF("supported encoding:\n");
		Show_encoding_ops();

        DBG_PRINTF("supported input:\n");
        Show_input_ops();

        DBG_PRINTF("supported debug chanel:\n");
        Show_debug_ops();
		return 0;
	}

	strncpy(acTextFile, argv[optind], 128);
	acTextFile[127] = '\0';
		
	iError = OpenTextFile(acTextFile);
	if (iError)
	{
		DBG_PRINTF("OpenTextFile error!\n");
		return -1;
	}

	iError = SetTextDetail(acHzkFile, acFreetypeFile, dwFontSize);
	if (iError)
	{
		DBG_PRINTF("SetTextDetail error!\n");
		return -1;
	}

	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	iError = SelectAndInitDisplay(acDisplay);
	if (iError)
	{
		DBG_PRINTF("SelectAndInitDisplay error!\n");
		return -1;
	}

    iError = All_input_devices_init();
    if (iError)
    {
        DBG_PRINTF("Error AllInputDevicesInit\n");
        return -1;
    }

	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	iError = ShowNextPage();
	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	if (iError)
	{
		DBG_PRINTF("Error to show first page\n");
		return -1;
	}

    DBG_PRINTF("Enter 'n' to show next page, 'u' to show previous page, 'q' to exit: ");

    while (1)
	{
        if (0 == Get_input_event(&tInputEvent))
        {
            if (tInputEvent.iVal == INPUT_VALUE_DOWN)
            {
                ShowNextPage();
            }
            else if (tInputEvent.iVal == INPUT_VALUE_UP)
            {
                ShowPrePage();
            }
            else if (tInputEvent.iVal == INPUT_VALUE_EXIT)
            {
                return 0;
            }
        }


	}
	return 0;
}
#endif

