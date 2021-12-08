/*
* Created by Aen on 2021/7/18.
*/
#include <config.h>
#include <debug_manager.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

static int g_i_socket_server;
static struct sockaddr_in g_t_socket_server_addr;
static struct sockaddr_in g_t_socket_client_addr;
static char *g_t_net_print_buf;
static int g_t_have_connected = 0;

static int g_t_read_pos = 0;
static int g_t_write_pos = 0;

static pthread_t g_t_send_thread_id;
static pthread_t g_t_recv_thread_id;

static pthread_mutex_t g_t_net_debug_send_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_t_net_debug_send_con_var = PTHREAD_COND_INITIALIZER;

static int is_full(void){
    return (((g_t_write_pos + 1) % PRINT_BUF_SIZE) == g_t_read_pos);
}
static int is_empty(void){
    return (g_t_read_pos == g_t_write_pos);
}
static int Put_data(char c_val){
    if(is_full()){
        return -1;
    }else{
        g_t_net_print_buf[g_t_write_pos] = c_val;
        g_t_write_pos = (g_t_write_pos + 1) % PRINT_BUF_SIZE;
        return 0;
    }
}
static int Get_data(char *p_val){
    if(is_empty()){
        return -1;
    }else{
        *p_val = g_t_net_print_buf[g_t_read_pos];
        g_t_read_pos = (g_t_read_pos + 1) % PRINT_BUF_SIZE;
        return 0;
    }
}
static void *Net_debug_send_thread_function(void *pVoid){
    char strTmpBuf[512];
    char cVal;
    int i;
    int iAddrLen;
    int iSendLen;

    while (1)
    {
        pthread_mutex_lock(&g_t_net_debug_send_mutex);
        pthread_cond_wait(&g_t_net_debug_send_con_var, &g_t_net_debug_send_mutex);
        pthread_mutex_unlock(&g_t_net_debug_send_mutex);

        while (g_t_have_connected && !is_empty())
        {
            i = 0;

            while ((i < 512) && (0 == Get_data(&cVal)))
            {
                strTmpBuf[i] = cVal;
                i++;
            }
            iAddrLen = sizeof(struct sockaddr);
            iSendLen = sendto(g_i_socket_server, strTmpBuf, i, 0,
                              (const struct sockaddr *)&g_t_socket_client_addr, iAddrLen);
        }
    }
    return NULL;
}
static void *Net_debug_revc_thread_function(void *pVoid){

    socklen_t iAddrLen;
    int iRecvLen;
    char ucRecvBuf[1000];
    struct sockaddr_in tSocketClientAddr;

    while (1)
    {
        iAddrLen = sizeof(struct sockaddr);
        DBG_PRINTF("in NetDbgRecvTreadFunction\n");
        iRecvLen = recvfrom(g_i_socket_server, ucRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);

        if (iRecvLen > 0)
        {
            ucRecvBuf[iRecvLen] = '\0';
            DBG_PRINTF("netprint.c get msg: %s\n", ucRecvBuf);

            /* analysis data:
             * setclient            : 设置接收打印信息的客户端
             * dbglevel=0,1,2...    : 修改打印级别
             * stdout=0             : 关闭stdout打印
             * stdout=1             : 打开stdout打印
             * netprint=0           : 关闭netprint打印
             * netprint=1           : 打开netprint打印
             */
            if (strcmp(ucRecvBuf, "setclient")  == 0)
            {
                g_t_socket_client_addr = tSocketClientAddr;
                g_t_have_connected = 1;
            }
            else if (strncmp(ucRecvBuf, "dbglevel=", 9) == 0)
            {
                Set_debug_level(ucRecvBuf);
            }
            else
            {
                Set_debug_channel(ucRecvBuf);
            }
        }

    }
    return NULL;
}
static int Net_debug_init(void){
    int iRet;
    g_i_socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == g_i_socket_server)
    {
        printf("socket error!\n");
        return -1;
    }

    g_t_socket_server_addr.sin_family      = AF_INET;
    g_t_socket_server_addr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
    g_t_socket_server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(g_t_socket_server_addr.sin_zero, 0, 8);

    iRet = bind(g_i_socket_server, (const struct sockaddr *)&g_t_socket_server_addr, sizeof(struct sockaddr));
    if (-1 == iRet)
    {
        printf("bind error!\n");
        return -1;
    }

    g_t_net_print_buf = malloc(PRINT_BUF_SIZE);
    if (NULL == g_t_net_print_buf)
    {
        close(g_i_socket_server);
        return -1;
    }
    pthread_create(&g_t_send_thread_id, NULL, Net_debug_send_thread_function, NULL);
    pthread_create(&g_t_recv_thread_id, NULL, Net_debug_revc_thread_function, NULL);

    return  0;
}
int Net_debug_exit(void){
    close(g_i_socket_server);
    free(g_t_net_print_buf);
    return 0;
}

int Net_debug_print(char *strData){
    int i = 0;

    for (i = 0; i < strlen(strData); i++)
    {
        if (0 != Put_data(strData[i]))
            break;
    }

    pthread_mutex_lock(&g_t_net_debug_send_mutex);
    pthread_cond_signal(&g_t_net_debug_send_con_var);
    pthread_mutex_unlock(&g_t_net_debug_send_mutex);

    return i;
}
static T_Debug_Ops g_t_net_print_ops = {
        .name        = "netprint",
        .Is_can_use  = DEBUG_CAN_USE,
        .Debug_init  = Net_debug_init,
        .Debug_exit  = Net_debug_exit,
        .Debug_print = Net_debug_print,
};

int Net_print_init(void){
    return Register_debug_ops(&g_t_net_print_ops);
}