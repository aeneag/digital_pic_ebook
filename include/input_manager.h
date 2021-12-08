/*
* Created by Aen on 2021/7/17.
*/
#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H
#include <sys/time.h>
#include <pthread.h>
#define INPUT_TYPE_STDIN        0
#define INPUT_TYPE_TOUCHSCREEN  1
#define INPUT_TYPE_BUTTON       2

#define INPUT_VALUE_UP          0
#define INPUT_VALUE_DOWN        1
#define INPUT_VALUE_EXIT        2
#define INPUT_VALUE_UNKNOWN     -1

typedef struct Input_event {
    struct timeval tTime;
    int iType;  /* stdin, touchsceen */
    int iVal;   /*  */
    int iX;
    int iY;
    int iPressure;
}T_Input_event, *P_Input_event;


typedef struct Input_ops {
        char *name;
        pthread_t tTreadID;
        int (*Device_init)(void);
        int (*Device_exit)(void);
        int (*Get_input_event)(P_Input_event pInputEvent);
        struct Input_ops *p_next;
}T_Input_ops,*P_Input_ops;

int Input_init(void);
int Register_input_ops(P_Input_ops pt_input_ops);
void Show_input_ops(void);

int All_input_devices_init(void);
int Get_input_event(P_Input_event pInputEvent);

int Stdin_init(void);
int TouchScreen_init(void);
int Button_init(void);
#endif //_INPUT_MANAGER_H
