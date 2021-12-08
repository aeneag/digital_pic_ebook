/*
* Created by Aen on 2021/7/15.
*/
#include <config.h>
#include <show_manager.h>
#include <string.h>

static P_Show_Ops g_pt_show_ops_head = NULL;


int Register_show_ops(P_Show_Ops pt_show_ops){
    P_Show_Ops ptTmp;

    if (!g_pt_show_ops_head)
    {
        g_pt_show_ops_head   = pt_show_ops;
        pt_show_ops->p_next = NULL;
    }
    else
    {
        ptTmp = g_pt_show_ops_head;
        while (ptTmp->p_next)
        {
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next	  = pt_show_ops;
        pt_show_ops->p_next = NULL;
    }
    return 0;
}

void Show_ops(void){
    int i = 0;
    P_Show_Ops ptTmp = g_pt_show_ops_head;
    while (ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}
P_Show_Ops Get_show_ops(char *pcName)
{
    P_Show_Ops ptTmp = g_pt_show_ops_head;

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
int Show_init(void){
    int iError;
    iError = FB_init();
    return iError;
}


