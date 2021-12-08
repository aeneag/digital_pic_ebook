/*
* Created by Aen on 2021/7/18.
*/
#include <config.h>
#include <input_manager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int g_t_fd;

static int Button_device_init(void){
    g_t_fd = open("/dev/digital_pic_frame_button", O_RDWR);
    if (g_t_fd < 0)
    {
        printf("can't open!\n");
        return -1;
    }
    return 0;
}
static int Button_device_exit(void){
    close(g_t_fd);
    return 0;
}
static int Button_get_input_event(P_Input_event ptInputEvent){
    int iRet;
    unsigned char  iVal;
    ptInputEvent->iType = INPUT_TYPE_BUTTON;

    iRet = read(g_t_fd, &iVal, 1); /* 会休眠直到有输入 */
    gettimeofday(&ptInputEvent->tTime, NULL);
    if (iVal == 0x81) {
        ptInputEvent->iVal = 0x81;
    } else if (iVal == 0x82) {
        ptInputEvent->iVal = 0x82;
    } else if (iVal == 0x83) {
        ptInputEvent->iVal = 0x83;
    } else {
        ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
    }


    return 0;
}
static T_Input_ops g_t_button_ops = {
        .name            = "button",
        .Device_init     = Button_device_init,
        .Device_exit     = Button_device_exit,
        .Get_input_event = Button_get_input_event,
};
int Button_init(void){
    return Register_input_ops(&g_t_button_ops);
}

