/*
* Created by Aen on 2021/7/18.
*/
#include <debug_manager.h>
#include <config.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

static P_Debug_Ops g_pt_debug_ops_head;
static int g_i_debug_level_limit = 8;

int Register_debug_ops(P_Debug_Ops pt_debug_ops){
    P_Debug_Ops ptTmp;

    if (!g_pt_debug_ops_head)
    {
        g_pt_debug_ops_head   = pt_debug_ops;
        pt_debug_ops->p_next = NULL;
    }
    else
    {
        ptTmp = g_pt_debug_ops_head;
        while (ptTmp->p_next)
        {
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next	  = pt_debug_ops;
        pt_debug_ops->p_next = NULL;
    }
    return 0;
}

void Show_debug_ops(void){
    int i = 0;
    P_Debug_Ops ptTmp = g_pt_debug_ops_head;
    while (ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}
P_Debug_Ops Get_debug_ops(char *pcName)
{
    P_Debug_Ops ptTmp = g_pt_debug_ops_head;

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

/* strBuf = "dbglevel=6" */
int Set_debug_level(char *strBuf)
{
    g_i_debug_level_limit = strBuf[9] - '0';
    return 0;
}
/*
 * stdout=0			   : 关闭stdout打印
 * stdout=1			   : 打开stdout打印
 * netprint=0		   : 关闭netprint打印
 * netprint=1		   : 打开netprint打印
 */

int Set_debug_channel(char *strBuf)
{
    char *pStrTmp;
    char strName[100];
    P_Debug_Ops ptTmp;

    pStrTmp = strchr(strBuf, '=');
    if (!pStrTmp)
    {
        return -1;
    }
    else
    {
        strncpy(strName, strBuf, pStrTmp-strBuf);
        strName[pStrTmp-strBuf] = '\0';
        ptTmp = Get_debug_ops(strName);
        if (!ptTmp)
            return -1;

        if (pStrTmp[1] == '0')
            ptTmp->Is_can_use = DEBUG_NOT_CAN_USE;
        else
            ptTmp->Is_can_use = DEBUG_CAN_USE;
        return 0;
    }

}

int Debug_print(const char *format, ...){
    char strTmpBuf[1000];
    char *pcTmp;
    va_list tArg;
    int iNum;
    P_Debug_Ops ptTmp = g_pt_debug_ops_head;
    int dbglevel = DEFAULT_DBGLEVEL;

    va_start (tArg, format);
    iNum = vsprintf (strTmpBuf, format, tArg);
    va_end (tArg);
    strTmpBuf[iNum] = '\0';

    pcTmp = strTmpBuf;

    if ((strTmpBuf[0] == '<') && (strTmpBuf[2] == '>'))
    {
        dbglevel = strTmpBuf[1] - '0';
        if (dbglevel >= 0 && dbglevel <= 9)
        {
            pcTmp = strTmpBuf + 3;
        }
        else
        {
            dbglevel = DEFAULT_DBGLEVEL;
        }
    }

    if (dbglevel > g_i_debug_level_limit)
    {
        return -1;
    }

    while (ptTmp)
    {
        if (ptTmp->Is_can_use)
        {
            ptTmp->Debug_print(pcTmp);
        }
        ptTmp = ptTmp->p_next;
    }

    return 0;

}



int Debug_init(void){
    int iError;
    iError = Stdout_init();
    iError |= Net_print_init();
    return iError;
}
int Init_debug_channel(void)
{
    P_Debug_Ops ptTmp = g_pt_debug_ops_head;
    while (ptTmp)
    {
        if (ptTmp->Is_can_use && ptTmp->Debug_init)
        {
            ptTmp->Debug_init();
        }
        ptTmp = ptTmp->p_next;
    }

    return 0;
}
