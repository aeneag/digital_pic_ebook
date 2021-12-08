/*
* Created by Aen on 2021/7/17.
*/
#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>



static int Stdin_device_init(void)
{
    struct termios tTTYState;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);

    //turn off canonical mode
    tTTYState.c_lflag &= ~ICANON;
    //minimum of number input read.
    tTTYState.c_cc[VMIN] = 1;   /* ��һ������ʱ�����̷��� */

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

    return 0;
}

static int Stdin_device_exit(void)
{

    struct termios tTTYState;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);

    //turn on canonical mode
    tTTYState.c_lflag |= ICANON;

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);
    return 0;
}

static int Stdin_get_input_event(P_Input_event ptInputEvent)
{
    /* ��������ݾͶ�ȡ����������
     * ���û������, ���̷���, ���ȴ�
     */

    /* select, poll ���Բ��� UNIX�����߼���� */

    char c;

    /* �������� */
    ptInputEvent->iType = INPUT_TYPE_STDIN;

    c = fgetc(stdin);  /* ������ֱ�������� */
    gettimeofday(&ptInputEvent->tTime, NULL);
#if 0
    if (c == 'u')
    {
        ptInputEvent->iVal = INPUT_VALUE_UP;
    }
    else if (c == 'n')
    {
        ptInputEvent->iVal = INPUT_VALUE_DOWN;
    }
    else if (c == 'q')
    {
        ptInputEvent->iVal = INPUT_VALUE_EXIT;
    }
    else
    {
        ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
    }
#endif
    return 0;
}

static T_Input_ops g_t_stdin_ops = {
        .name             = "stdin",
        .Device_init      = Stdin_device_init,
        .Device_exit      = Stdin_device_exit,
        .Get_input_event  = Stdin_get_input_event,
};


int Stdin_init(void)
{
    return Register_input_ops(&g_t_stdin_ops);
}

