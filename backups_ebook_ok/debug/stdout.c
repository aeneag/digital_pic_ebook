/*
* Created by Aen on 2021/7/18.
*/
#include <config.h>
#include <debug_manager.h>
#include <stdio.h>
#include <string.h>
static int Stdout_debug_print(char *format){
    /* 直接把输出信息用printf打印出来 */
    printf("%s", format);
    return strlen(format);
}

static T_Debug_Ops g_t_stdout_ops = {
        .name        = "stdout",
        .Is_can_use  = DEBUG_CAN_USE,
        .Debug_print  = Stdout_debug_print,
};

int Stdout_init(void){
    return Register_debug_ops(&g_t_stdout_ops);
}