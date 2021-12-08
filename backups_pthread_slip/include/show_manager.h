/*
* Created by Aen on 2021/7/15.
*/

#ifndef _SHOW_MANAGER_H
#define _SHOW_MANAGER_H

typedef struct T_Show_Ops {
    char *name;
    int iXres;
    int iYres;
    int iBpp;
    int (*Device_init)(void);
    int (*Show_pixel)(int iPenX, int iPenY, unsigned int dwColor);
    int (*Clean_screen)(unsigned int dwBackColor);
    struct T_Show_Ops *p_next;
}T_Show_Ops, *P_Show_Ops;


int Register_show_ops(P_Show_Ops pt_show_ops);
void Show_ops(void);
P_Show_Ops Get_show_ops(char *pcName);
int Show_init(void);
int FB_init(void);

#endif //_SHOW_MANAGER_H
