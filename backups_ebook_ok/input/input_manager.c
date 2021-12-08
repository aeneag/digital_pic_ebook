/*
* Created by Aen on 2021/7/17.
*/

#include <config.h>
#include <input_manager.h>
#include <string.h>

static P_Input_ops g_pt_input_ops_head;
static T_Input_event g_t_input_event;

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;

int Register_input_ops(P_Input_ops pt_input_ops){
    P_Input_ops ptTmp;

    if (!g_pt_input_ops_head)
    {
        g_pt_input_ops_head   = pt_input_ops;
        pt_input_ops->p_next = NULL;
    }
    else
    {
        ptTmp = g_pt_input_ops_head;
        while (ptTmp->p_next)
        {
            ptTmp = ptTmp->p_next;
        }
        ptTmp->p_next	  = pt_input_ops;
        pt_input_ops->p_next = NULL;
    }
    return 0;
}

void Show_input_ops(void){
    int i = 0;
    P_Input_ops ptTmp = g_pt_input_ops_head;
    while (ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->p_next;
    }
}
static void *Input_event_tread_function(void *pVoid)
{
    T_Input_event tInputEvent;

    /* 定义函数指针 */
    int (*Get_input_event)(P_Input_event ptInputEvent);
    Get_input_event = (int (*)(P_Input_event))pVoid;

    while (1)
    {
        if(0 == Get_input_event(&tInputEvent))
        {
            /* 唤醒主线程, 把tInputEvent的值赋给一个全局变量 */
            /* 访问临界资源前，先获得互斥量 */
            pthread_mutex_lock(&g_tMutex);
            g_t_input_event = tInputEvent;

            /*  唤醒主线程 */
            pthread_cond_signal(&g_tConVar);

            /* 释放互斥量 */
            pthread_mutex_unlock(&g_tMutex);
        }
    }

    return NULL;
}

int All_input_devices_init(void){
    P_Input_ops ptTmp = g_pt_input_ops_head;
    int iError = -1;

    while (ptTmp)
    {
        if (0 == ptTmp->Device_init())
        {
            /* 创建子线程 */
            pthread_create(&ptTmp->tTreadID, NULL, Input_event_tread_function, ptTmp->Get_input_event);
            iError = 0;
        }
        ptTmp = ptTmp->p_next;
    }
    return iError;
}
int Get_input_event(P_Input_event pInputEvent){
    /* 休眠 */
    pthread_mutex_lock(&g_tMutex);
    pthread_cond_wait(&g_tConVar, &g_tMutex);

    /* 被唤醒后,返回数据 */
    *pInputEvent = g_t_input_event;
    pthread_mutex_unlock(&g_tMutex);
    return 0;
}



int Input_init(void){
    int iError;
    iError = Stdin_init();
    iError |= TouchScreen_init();
    return iError;
}
