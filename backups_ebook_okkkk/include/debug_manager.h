/*
* Created by Aen on 2021/7/18.
*/
#ifndef _DEBUG_MANAGER_H
#define _DEBUG_MANAGER_H

#define DEBUG_CAN_USE 1
#define DEBUG_NOT_CAN_USE 0
#define SERVER_PORT 8888
#define PRINT_BUF_SIZE   (16*1024)

#define	DEBUG_EMERG	"<0>"	/* system is unusable			*/
#define	DEBUG_ALERT	"<1>"	/* action must be taken immediately	*/
#define	DEBUG_CRIT	"<2>"	/* critical conditions			*/
#define	DEBUG_ERR	    "<3>"	/* error conditions			*/
#define	DEBUG_WARNING	"<4>"	/* warning conditions			*/
#define	DEBUG_NOTICE	"<5>"	/* normal but significant condition	*/
#define	DEBUG_INFO	"<6>"	/* informational			*/
#define	DEBUG_DEBUG	"<7>"	/* debug-level messages			*/
#define DEFAULT_DBGLEVEL  4

typedef struct Debug_Ops {
    char *name;
    int Is_can_use;
    int (*Debug_init)(void);
    int (*Debug_exit)(void);
    int (*Debug_print)(char *format);
    struct Debug_Ops *p_next;
}T_Debug_Ops,*P_Debug_Ops;

int Register_debug_ops(P_Debug_Ops pt_debug_ops);
void Show_debug_ops(void);
P_Debug_Ops Get_debug_ops(char *pcName);
int Set_debug_level(char *strBuf);
int Set_debug_channel(char *strBuf);
int Debug_print(const char *format, ...);
int Debug_init(void);
int Init_debug_channel(void);


int Stdout_init(void);
int Net_print_init(void);
#endif //_DEBUG_MANAGER_H
