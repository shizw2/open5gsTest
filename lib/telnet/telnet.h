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
#include "ogs-core.h"

#define BUFSIZE 1024
#define MAX_PARA 10



typedef int  BOOL;
#define TRUE 1
#define FALSE 0


/* 紧凑型结构定义方式 */
#define	 PACK_1  __attribute__ ((packed))
#define FUNC_MCS_CCM 

typedef enum { STRING, INTEGER } ParamType;
typedef void (*GenericFunc)(void); // 声明一个通用函数指针类型
typedef struct tagdsCmdParas
{
    uint8_t bLen;
    uint8_t abRsv[3];
    uint8_t abCont[128];
} PACK_1 T_pttCmdParas;


typedef struct telnet_command_s{
    const  char* command;
    void (*function)(void);
    int numParams;
    ParamType paramTypes[3];
} telnet_command_t;

typedef void (*TelnetCmdCallback)(char *);
uint32_t pttGetCmdWord32Value(T_pttCmdParas *ptCmdPara);
BOOL pttGetCmdParams(char *pabCmd);

void telnetMain(void *arg);
void telnet_proc_cmd(char * pabCmd); //每个NF实现自己的telnet命令
void set_telnet_cmd_callback(TelnetCmdCallback callback);

void funcO(void);
// 1个参数函数声明
void funcI(uint32_t num);
void funcS(char* str);

// 2个参数函数声明
void funcII(uint32_t num1, uint32_t num2);
void funcIS(uint32_t num, char* str);
void funcSI(char* str, uint32_t num);
void funcSS(char* str1, char* str2);

// 3个参数函数声明
void funcIII(uint32_t num1, uint32_t num2, uint32_t num3);
void funcIIS(uint32_t num1, uint32_t num2, char* str);
void funcISS(uint32_t num, char* str1, char* str2);
void funcSSI(char* str1, char* str2, uint32_t num);