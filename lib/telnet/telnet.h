#include <stdio.h>  
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>


#define BUFSIZE 1024
#define MAX_PARA 10



typedef int  BOOL;
#define TRUE 1
#define FALSE 0


/* 紧凑型结构定义方式 */
#define	 PACK_1  __attribute__ ((packed))
#define FUNC_MCS_CCM 


typedef struct tagdsCmdParas
{
    uint8_t bLen;
    uint8_t abRsv[3];
    uint8_t abCont[128];
} PACK_1 T_pttCmdParas;

typedef void (*TelnetCmdCallback)(char *);





uint32_t pttGetCmdWord32Value(T_pttCmdParas *ptCmdPara);
BOOL pttGetCmdParams(char *pabCmd);

void telnetMain(void *arg);
void telnet_proc_cmd(char * pabCmd); //每个NF实现自己的telnet命令
void set_telnet_cmd_callback(TelnetCmdCallback callback);