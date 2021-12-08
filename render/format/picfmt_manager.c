/*
* Created by Aen on 2021/7/23.
*/
#include <picfmt_manager.h>



static P_Pic_File_Parser g_ptPicFileParserHead = NULL;

int Register_pic_file_parser(P_Pic_File_Parser ptPicFileParser)
{
    P_Pic_File_Parser ptTmp;

    if (!g_ptPicFileParserHead)
    {
        g_ptPicFileParserHead   = ptPicFileParser;
        ptPicFileParser->p_next = NULL;
    }
    else
    {
        ptTmp = g_ptPicFileParserHead;
        while (ptTmp->p_next)
        {
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next	  = ptPicFileParser;
        ptPicFileParser->p_next = NULL;
    }

    return 0;
}


void Show_pic_fmts(void)
{
    int i = 0;
    P_Pic_File_Parser ptTmp = g_ptPicFileParserHead;

    while (ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}

P_Pic_File_Parser Parser(char *pcName)
{
    P_Pic_File_Parser ptTmp = g_ptPicFileParserHead;

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

P_Pic_File_Parser Get_parser(P_File_Map ptFileMap)
{
    P_Pic_File_Parser ptTmp = g_ptPicFileParserHead;

    while (ptTmp)
    {
        if (ptTmp->Is_support(ptFileMap))
        {
            return ptTmp;
        }
        ptTmp = ptTmp->p_next;
    }
    return NULL;
}

int Pic_fmts_init(void)
{
    int iError;

    iError = BMP_parser_init();
    iError |= JPG_parser_init();

    return iError;
}



