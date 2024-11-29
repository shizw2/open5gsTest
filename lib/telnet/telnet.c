#include <telnet.h>  


int save_fd;
/*telnet_cmd:ff fb 01 ff fb 03 ff fc 1f*/
//const unsigned char g_chCmdTelnet[9] = {0xff, 0xfb, 0x01, 0xff, 0xfb, 0x03, 0xff, 0xfc, 0x1f};
/*TELNET协议命令*/
/*0XFD: 认可选项请求*/
const unsigned char g_chCmdTelnet[12] = {0xff,0xfd,0x18, 0xff,0xfd,0x20, 0xff,0xfd,0x23, 0xff,0xfd,0x27};
char                g_cmdLine[1024]  = {0};
uint8_t             g_bCmdParaNum    = 0;
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
uint8_t             g_bCmdNameLen    = 0;

int pttSendTelnetCmd(int fd);
int pttReadCmdline(int sockfd, char *cmdLine, int size);
int pttioStdSet(int src_fd, int dest_fd, int *save_fd);
void pttRecoverIoStdSet(int src_fd, int dest_fd);
int pttCmdAnalyze(char *cmd);
int pttCmdProcess(int fd, char *cmdLine);
void pttTaskProcess(int sockfd, BOOL bLoginSuccess);
void *pttTelnetdPthread(void * arg);
void *pttTaskProcessPthread(void *arg);
int pttTelnetdStart(void);
void pttTelnetProcCmdCCM(char * pabCmd);
void *pttClientTaskProcess(int sockfd) ;
void show(void);
int pttTelnetd(ogs_list_t *cli_list);
int detect_interrupt_signal(const char *data, size_t len);

const char *strPassPromt = "Password:";
static TelnetCmdCallback cmd_callback = NULL;
char g_program_name[100];

telnet_command_t *commands = NULL;
int numCommands = 0;


void set_telnet_cmd_callback(TelnetCmdCallback callback) {
    cmd_callback = callback;
}

/*启动telnet服务*/
int pttTelnetdStart(void)
{
#if 1
    pthread_t id;
    int i,ret;

    ret=pthread_create(&id,NULL,(void *) pttTelnetdPthread,NULL);
    if(ret!=0){
        printf ("Telnet: telnet_starting.......err!\n");
        return -1;
    }

    
    printf("start pttTelnetdPthread  succcess\r\n");

#else 
    int pid;
    int status;

    if(pid=fork()) {
        //exit(0);        //是父进程，结束父进程
        waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
        return 0;
    } else if(pid < 0) {
        return -1;//exit(1);        //fork失败，退出
    }
    printf("Telnet: telnet_starting.......\n");
    pttTelnetd(23);
#endif  

    return 0;
}

int pttTelnetd(ogs_list_t *cli_list){
    int server_sockfd;//服务器端套接字  
    int client_sockfd;//客户端套接字  
    struct sockaddr_in remote_addr; //客户端网络地址结构体  
    socklen_t  sin_size;  
    BOOL bReuseaddr=TRUE;
    pid_t fpid;
    int status;
   
    char buf[OGS_ADDRSTRLEN];
    socklen_t addrlen;
    ogs_socknode_t *node = NULL;
    ogs_sockaddr_t *addr = NULL;
    
    /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/  
    if((server_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)  
    {    
        perror("socket");
        return -1;  
    }  

    /*设置地址可重用*/
    setsockopt (server_sockfd,SOL_SOCKET,SO_REUSEADDR,(char  *)&bReuseaddr, sizeof(bReuseaddr));

    /*将套接字绑定到服务器的网络地址上*/ 
    ogs_list_for_each(cli_list, node) {
        addr = node->addr;
        printf("begin bind socket to ip:%s, port %u.\r\n", OGS_ADDR(addr, buf),OGS_PORT(addr));          
        addrlen = ogs_sockaddr_len(addr); 
        if (bind(server_sockfd, &addr->sa, addrlen) != 0) {
            printf("socket bind(%d) [%s]:%d failed",
                    addr->ogs_sa_family, OGS_ADDR(addr, buf), OGS_PORT(addr));
            return -1;
        }
    }

    printf("begin listen socket.\r\n");

    /*监听连接请求*/  
    listen(server_sockfd,1);  

    printf("Telnet: listening for telnet requests....\n");

    sin_size=sizeof(struct sockaddr_in);  
    while(1) 
    {      
        /*等待客户端连接请求到达*/  
        if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)  
        {  
            perror("accept");  
            return 1;  
        }  
        printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));  

        pthread_t id;
        int i,ret;

        ret=pthread_create(&id,NULL,(void *) pttTaskProcessPthread,(void *)&client_sockfd);
        if(ret!=0){
            printf ("Telnet: telnet_starting.......err!\n");
            return -1;
        }
        printf ("Telnet: telnet_started ok!\n");
        pthread_join(id,NULL);
    } 

    close(client_sockfd);  
    close(server_sockfd);  
    return 0;       
}

void * pttTelnetdPthread(void *arg)
{
    ogs_list_t *cli_list = (ogs_list_t *)arg;
    pttTelnetd(cli_list);
    return NULL;
}

void telnetMain(void *arg)
{
    ogs_list_t *cli_list = (ogs_list_t *)arg;
    pttTelnetd(cli_list); 
    return ;
}

/*发送telnet协议命令*/
int pttSendTelnetCmd(int fd)
{
    return write(fd, g_chCmdTelnet, sizeof(g_chCmdTelnet));
}

/*读取命令字符串*/
int pttReadCmdline(int sockfd, char *cmdLine, int size)
{
    int ret, rev_count = 0;
    char *buf = NULL;
    buf = cmdLine;
    while(1 == (ret = read(sockfd, buf, 1))) 
    {
        rev_count++;
        if(rev_count > BUFSIZE - 2) 
        {
            return rev_count;
        }
        
        if(*buf == '\n') 
        {
            return rev_count;
        }
        else  if (*buf == '\r') // 回车符，进行命令处理
        {
            return rev_count;
        }
        else if (*buf == 0)
        {
            return rev_count;
        }
        buf++;
    }
    return ret;
} 

/*输出重定向*/
/*标准输入（stdin），标准输出（stdout），标准出错信息（stderr）的文件号分别为0，1，2*/
int pttioStdSet(int src_fd, int dest_fd, int *save_fd) 
{
    *save_fd = dup(dest_fd);
    dup2(src_fd, dest_fd);
    //close(src_fd);
    return *save_fd;
}

/*恢复输出重定向*/
void pttRecoverIoStdSet(int src_fd, int dest_fd) 
{
    dup2(src_fd, dest_fd);
    close(src_fd);
}

/*解析字符串*/
int pttCmdAnalyze(char *cmd)
{
    char *ptr = NULL;
    char *ptr_tmp;

    if(strlen(cmd) < 1 || strlen(cmd) > 60) {
        return -1;
    }
    /*去除多余的换行符及其他多余字符*/
    while((ptr = strstr(cmd, "\r")) != 0 ) {
        while(*ptr != 0) {
            *ptr = *(ptr+1);
            ptr++;
        }
    }

    while((ptr = strstr(cmd, "\n")) != 0 ) {
        while(*ptr != 0) {
            *ptr = *(ptr+1);
            ptr++;
        }
    }

#if 1
    ptr = cmd;
    while((!((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') || (*ptr >= '0' && *ptr <= '9'))) && (*ptr != 0)) {
        ptr_tmp = ptr;
        while(*ptr_tmp != 0) {
            *ptr_tmp = *(ptr_tmp+1);
            ptr_tmp++;
        }
    }

#endif  
    if(strlen(cmd) < 1 || strlen(cmd) > 60) 
    {
        return -1;
    }
    return 0;
}

#if 1
void show(void)
{
    printf("this is %s management system. \r\n",g_program_name);
}
#endif

void set_telnet_commands(telnet_command_t *nf_commands, int nf_numCommands){
    commands = nf_commands;
    numCommands = nf_numCommands;
    printf("numCommands:%d\r\n",numCommands);
}

/*执行命令并回显到telnet终端*/
int pttCmdProcess(int fd, char *cmdLine)
{
    int i;
    pttioStdSet(fd, 1, &save_fd); /*标准输出重定向*/

    /*这里添加命令处理函数*/
    /*示例*/
    //printf("%s\r\n",cmdLine);
    //if (cmd_callback != NULL) {
    //    cmd_callback(cmdLine);
    //}

    uint32_t dwPara1 = 0;
    uint32_t dwPara2 = 0;
    uint32_t dwPara3 = 0;
    char* strPara1 = NULL;
    char* strPara2 = NULL;
    char* strPara3 = NULL;
    
    if (strncmp(cmdLine, "help", 4) == 0){
        printf("Supported commands:\n");
        for (i = 0; i < numCommands; i++) {
            printf("- %s\n", commands[i].command);
        }
        printf(">\r");    
        pttRecoverIoStdSet(save_fd, 1); /*恢复输出重定向*/
        return 0;
    }
    
    if (!pttGetCmdParams(cmdLine))
    {
        printf(">\r");    
        pttRecoverIoStdSet(save_fd, 1); /*恢复输出重定向*/
        return -1;
    }    
        
    for (i = 0; i < numCommands; i++)
    {       
        if (strcmp(g_chCmdName, commands[i].command) == 0){
            telnet_command_t cmd = commands[i];
            
            union {
                uint32_t intValue;
                char* strValue;
            } params[3];

            int paramIndex = 0;

            for (paramIndex = 0; paramIndex < cmd.numParams; paramIndex++)
            {
                if (cmd.paramTypes[paramIndex] == INTEGER)
                {                    
                    params[paramIndex].intValue = pttGetCmdWord32Value(&g_tCmdPara[paramIndex]);
                    //printf("intValue:%d\r\n",params[paramIndex].intValue);
                }
                else if (cmd.paramTypes[paramIndex] == STRING)
                {
                    params[paramIndex].strValue = (char*)g_tCmdPara[paramIndex].abCont;
                    //printf("strValue:%s\r\n",params[paramIndex].strValue);
                }
                // 处理其他可能的参数类型
            }

            // 根据参数数量调用相应的函数，并传递参数数组
            switch (cmd.numParams)
            {
                case 0:
                    ((void (*)(void))cmd.function)();
                    break;
                case 1:
                    if (cmd.paramTypes[0] == INTEGER)
                    {
                        ((void (*)(uint32_t))cmd.function)(params[0].intValue);
                    }
                    else if (cmd.paramTypes[0] == STRING)
                    {
                        ((void (*)(char*))cmd.function)(params[0].strValue);
                    }
                    break;
                case 2:
                    if (cmd.paramTypes[0] == INTEGER && cmd.paramTypes[1] == INTEGER)
                    {
                        ((void (*)(uint32_t, uint32_t))cmd.function)(params[0].intValue, params[1].intValue);
                    }
                    else if (cmd.paramTypes[0] == INTEGER && cmd.paramTypes[1] == STRING)
                    {
                        ((void (*)(uint32_t, char*))cmd.function)(params[0].intValue, params[1].strValue);
                    }
                    else if (cmd.paramTypes[0] == STRING && cmd.paramTypes[1] == INTEGER)
                    {
                        ((void (*)(char*, uint32_t))cmd.function)(params[0].strValue, params[1].intValue);
                    }
                    else if (cmd.paramTypes[0] == STRING && cmd.paramTypes[1] == STRING)
                    {
                        ((void (*)(char*, char*))cmd.function)(params[0].strValue, params[1].strValue);
                    }
                    break;
                case 3:
                    if (cmd.paramTypes[0] == INTEGER && cmd.paramTypes[1] == INTEGER && cmd.paramTypes[2] == INTEGER)
                    {
                        ((void (*)(uint32_t, uint32_t, uint32_t))cmd.function)(params[0].intValue, params[1].intValue, params[2].intValue);
                    }
                    else if (cmd.paramTypes[0] == INTEGER && cmd.paramTypes[1] == INTEGER && cmd.paramTypes[2] == STRING)
                    {
                        ((void (*)(uint32_t, uint32_t, char*))cmd.function)(params[0].intValue, params[1].intValue, params[2].strValue);
                    }
                    else if (cmd.paramTypes[0] == INTEGER && cmd.paramTypes[1] == STRING && cmd.paramTypes[2] == INTEGER)
                    {
                        ((void (*)(uint32_t, char*, uint32_t))cmd.function)(params[0].intValue, params[1].strValue, params[2].intValue);
                    }
                    else if (cmd.paramTypes[0] == INTEGER && cmd.paramTypes[1] == STRING && cmd.paramTypes[2] == STRING)
                    {
                        ((void (*)(uint32_t, char*, char*))cmd.function)(params[0].intValue, params[1].strValue, params[2].strValue);
                    }
                    else if (cmd.paramTypes[0] == STRING && cmd.paramTypes[1] == INTEGER && cmd.paramTypes[2] == INTEGER)
                    {
                        ((void (*)(char*, uint32_t, uint32_t))cmd.function)(params[0].strValue, params[1].intValue, params[2].intValue);
                    }
                    else if (cmd.paramTypes[0] == STRING && cmd.paramTypes[1] == INTEGER && cmd.paramTypes[2] == STRING)
                    {
                        ((void (*)(char*, uint32_t, char*))cmd.function)(params[0].strValue, params[1].intValue, params[2].strValue);
                    }
                    else if (cmd.paramTypes[0] == STRING && cmd.paramTypes[1] == STRING && cmd.paramTypes[2] == INTEGER)
                    {
                        ((void (*)(char*, char*, uint32_t))cmd.function)(params[0].strValue, params[1].strValue, params[2].intValue);
                    }
                    else if (cmd.paramTypes[0] == STRING && cmd.paramTypes[1] == STRING && cmd.paramTypes[2] == STRING)
                    {
                        ((void (*)(char*, char*, char*))cmd.function)(params[0].strValue, params[1].strValue, params[2].strValue);
                    }
                    break;
      
                default:
                    printf("Invalid number of parameters\n");
                    break;
            }

            break;
        }
    }
    
    if (i == numCommands){
        printf("Command not found: %s\n", g_chCmdName);
    }
    
    
    printf(">\r");    
    pttRecoverIoStdSet(save_fd, 1); /*恢复输出重定向*/
    return 0;
}

BOOL pttGetCmdParams(char *pabCmd)
{
    BOOL blLBracket   = FALSE; /*左括号*/
    BOOL blRBracket   = FALSE; /*右括号*/
    uint16_t i          = 0;

    if (NULL == pabCmd)
    {
        return FALSE;
    }

    /* 初始化 */
    g_bCmdParaNum = 0;
    g_bCmdNameLen = 0;

    memset(g_chCmdName, 0, sizeof(g_chCmdName));
    memset(&g_tCmdPara, 0, sizeof(g_tCmdPara));

    //printf("%s\r\n", pabCmd);

    for(i=0; i<128 && *(pabCmd+i)!= '\0'; i++)
    {
        if (!blLBracket)
        {
            if (*(pabCmd+i) == ' ')
            {
                continue;
            }

            if ((*(pabCmd+i) >= 'a' && *(pabCmd+i) <= 'z') ||
                (*(pabCmd+i) >= 'A' && *(pabCmd+i) <= 'Z') ||
                (*(pabCmd+i) >= '0' && *(pabCmd+i) <= '9') ||
                (*(pabCmd+i) == '_'))
            {
                g_chCmdName[g_bCmdNameLen++]=*(pabCmd+i);
            }
            else if (*(pabCmd+i) == '(')
            {
                blLBracket = TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            if (*(pabCmd+i) == ' ')
            {
                continue;
            }

            /* 碰到右括号结束 */
            if (*(pabCmd+i) == ')')
            {
                g_bCmdParaNum++;
                blRBracket = TRUE;
                break;
            }

            if (*(pabCmd+i) == ',')
            {
                g_bCmdParaNum++;
                continue;
            }

            g_tCmdPara[g_bCmdParaNum].abCont[g_tCmdPara[g_bCmdParaNum].bLen] = *(pabCmd+i);
            g_tCmdPara[g_bCmdParaNum].bLen++;
        }
    }

    if (!blLBracket || !blRBracket)
    {
        printf("the input shell command format error, function shall end with ()\r\n");
        return FALSE;
    }

    return TRUE;
}

int detect_interrupt_signal(const char *data, size_t len) {
    const int IAC = 255;
    const int INTERRUPT = 244; // F4的十进制表示
    int i;

    for (i = 0; i < len;) {
        if ((unsigned char)data[i] == IAC) {
            // 检查是否是中断信号
            if (i + 1 < len && (unsigned char)data[i + 1] == INTERRUPT) {
                // 客户端发送了中断信号
                return 1;
            }
            // 跳过IAC和其后的字节
            i += 2;
        } else {
            // 普通的数据字节，移动到下一个位置
            i++;
        }
    }
    return 0;
}

/*telnet交互处理函数*/
void pttTaskProcess(int sockfd, BOOL bLoginSuccess)
{
    char cmdLine[BUFSIZE]={0};
    int count = 0;
    int ret;
    const char *strPass = "5gc";
    
    // 获取当前程序的文件路径
    char path[100];
    if (readlink("/proc/self/exe", path, sizeof(path)) == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    
    // 从文件路径中提取程序名称
    char* programName = strrchr(path, '/');
    if (programName != NULL) {
        programName++; // 移动到下一个字符，跳过斜杠
    } else {
        programName = path; // 如果没有斜杠，则使用整个路径作为程序名称
    }
    strcpy(g_program_name, programName);

    pttioStdSet(sockfd, 1, &save_fd);
    printf("\r\r\nwelcome to %s management system. \r\n", programName);
    printf("%s", strPassPromt);
    fflush(stdout);
    pttRecoverIoStdSet(save_fd, 1);

    
    while(1) {

        printf("task_processing ,sockfd=%u.\r\n", sockfd);
        pttSendTelnetCmd(sockfd);

        memset(cmdLine, 0, sizeof(cmdLine));

        count = pttReadCmdline(sockfd, cmdLine, BUFSIZE);
        if(count <= 0) {
            //perror("read err");
            //exit(1);对方断开连接，返回
            return ;
        }
        printf("[%s,%d]rev count:%d,buf:%s\n",__FUNCTION__,__LINE__,count, cmdLine); //少两个字符
        
        if (detect_interrupt_signal(cmdLine, count)){
            printf("client interrupt.\r\n");
            return ;
        }
        
        ret = pttCmdAnalyze(cmdLine);
        if(ret == 0) 
        {
            printf("[%s,%d]rev count:%d,buf:%s.\n",__FUNCTION__,__LINE__,count, cmdLine);
            
            if (strncmp(cmdLine, "quit", 4) == 0){
                printf("telnet quit.\r\n");
                //如何退出客户端
                // 向客户端发送回车换行符，触发客户端处理并关闭连接
                //shutdown(sockfd, SHUT_RDWR);

                // 等待一段时间以确保客户端接收并处理消息
                //sleep(1);
                return;
            }

            if (bLoginSuccess && strlen(cmdLine) > 0)
            {
                pttCmdProcess(sockfd, cmdLine);
            }


            if (!bLoginSuccess && strcmp(cmdLine, strPass) == 0)
            {
                pttioStdSet(sockfd, 1, &save_fd);
                printf("\r\n");
                pttRecoverIoStdSet(save_fd, 1);
                bLoginSuccess = TRUE;
            }
            memset(cmdLine, 0, sizeof(cmdLine));
            
            if (!bLoginSuccess)
            {
                pttioStdSet(sockfd, 1, &save_fd);
                printf("%s", strPassPromt);
                fflush(stdout);
                pttRecoverIoStdSet(save_fd, 1);
            }
            else
            {
                pttioStdSet(sockfd, 1, &save_fd);
                printf("aaa>");
                fflush(stdout);
                pttRecoverIoStdSet(save_fd, 1);
            }            
        }else{
            //pttioStdSet(sockfd, 1, &save_fd);
            printf("pttCmdAnalyze faild,cmdLine:%s\r\n",cmdLine);
            //printf("aaa>");
            //fflush(stdout);
            //pttRecoverIoStdSet(save_fd, 1);            
        }
    }
    
    printf("test:telnet quit.\r\n");
}
void __attribute__((noreturn)) *pttTaskProcessPthread(void *arg) 
{
    int sockfd;
    sockfd = * (int*)arg;

    BOOL bLoginSuccess = FALSE;

    printf("pttTaskProcessPthread \r\n");
    pttTaskProcess(sockfd, bLoginSuccess);
    printf("pttTaskProcessPthread exit\r\n");
    close(sockfd);  
    pthread_exit((void *)1);
}

void __attribute__((noreturn)) *pttClientTaskProcess(int sockfd)  
{

    BOOL bLoginSuccess = FALSE;

    pttTaskProcess(sockfd, bLoginSuccess);

    pthread_exit((void *)1);
}


uint32_t pttGetCmdWord32Value(T_pttCmdParas *ptCmdPara)
{
    uint8_t         i = 0;
    uint32_t  dwValue = 0;

    if (NULL == ptCmdPara || g_bCmdParaNum == 0)
    {
        return 0;
    }

    if (ptCmdPara->bLen > 10)
    {
        return 0;
    }

    for (i=0; i<ptCmdPara->bLen; i++)
    {
        dwValue = dwValue*10 + ptCmdPara->abCont[i]-'0';
    }

    return dwValue;
}


#if 0
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#define DEFAULTIP "192.168.0.75"
#define DEFAULTPORT "2323"
#define DEFAULTBACK "10"
#define DEFAULTDIR "/tmp"
#define DEFAULTLOG "/tmp/telnet-server.log"

void prterrmsg(char *msg);
#define prterrmsg(msg)        { perror(msg); abort(); }
void wrterrmsg(char *msg);
#define wrterrmsg(msg)        { fputs(msg, logfp); fputs(strerror(errno), logfp);fflush(logfp); abort(); }

void prtinfomsg(char *msg);
#define prtinfomsg(msg)        { fputs(msg, stdout);  }
void wrtinfomsg(char *msg);
#define wrtinfomsg(msg)        {  fputs(msg, logfp); fflush(logfp);}

#define MAXBUF        1024

char buffer[MAXBUF + 1];
char *host = 0;
char *port = 0;
char *back = 0;
char *dirroot = 0;
char *logdir = 0;
unsigned char daemon_y_n = 0;
FILE *logfp;

#define MAXPATH        150

/*------------------------------------------------------
 *--- AllocateMemory - 分配空间并把d所指的内容复制
 *------------------------------------------------------
 */
void AllocateMemory(char **s, int l, char *d)
{
    *s = malloc(l + 1);
    bzero(*s, l + 1);
    memcpy(*s, d, l);
}
/************关于本文档*************************************************************
*filename: telnet-server.c
*purpose: 这是在Linux下用C语言写的telnet服务器，没有用户名和密码，直接以开启服务者的身份登录系统
*wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
*date time:2007-01-27 17:02
*Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
* 但请遵循GPL
*Thanks to: Google.com
*Hope:希望越来越多的人贡献自己的力量，为科学技术发展出力
* 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
**********************************************************************************/

/*------------------------------------------------------
 *--- getoption - 分析取出程序的参数
 *------------------------------------------------------
 */
void getoption(int argc, char **argv)
{
    int c, len;
    char *p = 0;

    opterr = 0;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"host", 1, 0, 0},
            {"port", 1, 0, 0},
            {"back", 1, 0, 0},
            {"dir", 1, 0, 0},
            {"log", 1, 0, 0},
            {"daemon", 0, 0, 0},
            {0, 0, 0, 0}
        };
        /* 本程序支持如一些参数：
         * --host IP地址 或者 -H IP地址
         * --port 端口 或者 -P 端口
         * --back 监听数量 或者 -B 监听数量
         * --dir 服务默认目录 或者 -D 服务默认目录
         * --log 日志存放路径 或者 -L 日志存放路径
         * --daemon 使程序进入后台运行模式
         */
        c = getopt_long(argc, argv, "H:P:B:D:L",
                        long_options, &option_index);
        if (c == -1 || c == '?')
            break;

        if(optarg)        len = strlen(optarg);
        else        len = 0;

        if ((!c && !(strcasecmp(long_options[option_index].name, "host")))
            || c == 'H')
            p = host = malloc(len + 1);
        else if ((!c
                  &&
                  !(strcasecmp(long_options[option_index].name, "port")))
                 || c == 'P')
            p = port = malloc(len + 1);
        else if ((!c
                  &&
                  !(strcasecmp(long_options[option_index].name, "back")))
                 || c == 'B')
            p = back = malloc(len + 1);
        else if ((!c
                  && !(strcasecmp(long_options[option_index].name, "dir")))
                 || c == 'D')
            p = dirroot = malloc(len + 1);
        else if ((!c
                  && !(strcasecmp(long_options[option_index].name, "log")))
                 || c == 'L')
            p = logdir = malloc(len + 1);
        else if ((!c
                  &&
                  !(strcasecmp
                    (long_options[option_index].name, "daemon")))) {
            daemon_y_n = 1;
            continue;
        }
        else
            break;
        bzero(p, len + 1);
        memcpy(p, optarg, len);
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    int sock_fd, addrlen;
    pid_t fpid; //fpid表示fork函数返回的值  

    /* 获得程序工作的参数，如 IP 、端口、监听数、网页根目录、目录存放位置等 */
    getoption(argc, argv);

    if (!host) {
        addrlen = strlen(DEFAULTIP);
        AllocateMemory(&host, addrlen, DEFAULTIP);
    }
    if (!port) {
        addrlen = strlen(DEFAULTPORT);
        AllocateMemory(&port, addrlen, DEFAULTPORT);
    }
    if (!back) {
        addrlen = strlen(DEFAULTBACK);
        AllocateMemory(&back, addrlen, DEFAULTBACK);
    }
    if (!dirroot) {
        addrlen = strlen(DEFAULTDIR);
        AllocateMemory(&dirroot, addrlen, DEFAULTDIR);
    }
    if (!logdir) {
        addrlen = strlen(DEFAULTLOG);
        AllocateMemory(&logdir, addrlen, DEFAULTLOG);
    }

    printf
        ("host=%s port=%s back=%s dirroot=%s logdir=%s %s是后台工作模式(进程ID：%d)\n",
         host, port, back, dirroot, logdir, daemon_y_n?"":"不", getpid());

    /* fork() 两次处于后台工作模式下 */
    if (daemon_y_n) {
        if (fork())
            exit(0);
        if (fork())
            exit(0);
        close(0), close(1), close(2);
        logfp = fopen(logdir, "a+");
        if (!logfp)
            exit(0);
    }

    /* 处理子进程退出以免产生僵尸进程 */
    signal(SIGCHLD, SIG_IGN);

    /* 创建 socket */
    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        if (!daemon_y_n) {
            prterrmsg("socket()");
        } else {
            wrterrmsg("socket()");
        }
    }

    /* 设置端口快速重用 */
    addrlen = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &addrlen,
               sizeof(addrlen));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(host);
    addrlen = sizeof(struct sockaddr_in);
    /* 绑定地址、端口等信息 */
    if (bind(sock_fd, (struct sockaddr *) &addr, addrlen) < 0) {
        if (!daemon_y_n) {
            prterrmsg("bind()");
        } else {
            wrterrmsg("bind()");
        }
    }

    /* 开启临听 */
    if (listen(sock_fd, atoi(back)) < 0) {
        if (!daemon_y_n) {
            prterrmsg("listen()");
        } else {
            wrterrmsg("listen()");
        }
    }
    while (1) {
        int new_fd;
        addrlen = sizeof(struct sockaddr_in);
        /* 接受新连接请求 */
        new_fd = accept(sock_fd, (struct sockaddr *) &addr, &addrlen);
        if (new_fd < 0) {
            if (!daemon_y_n) {
                prterrmsg("accept()");
            } else {
                wrterrmsg("accept()");
            }
            break;
        }
        bzero(buffer, MAXBUF + 1);
        sprintf(buffer, "connection come from: %s:%d\n",
                inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        if (!daemon_y_n) {
            prtinfomsg(buffer);
        } else {
            wrtinfomsg(buffer);
        }
        /* 产生一个子进程去处理请求，当前进程继续等待新的连接到来 */
        fpid = fork();
        if (fpid < 0)  
        {
                printf("error in fork!");   
        }
        else if (fpid == 0)
        {  
                printf("i am the child process, my process id is %d/n",getpid());   
                
                wrtinfomsg("new sub pro to solve this client.\r\n");
                         /* 把socket连接作为标准输入、输出、出错句柄来用 */
                         dup2(new_fd, 0);
                         dup2(new_fd, 1);
                         dup2(new_fd, 2);
                         /* 切换到指定目录工作 */
                         chdir(dirroot);
                         /* 交互式执行shell */
                         execl("/bin/bash", "-l", "--login", "-i", "-r", "-s", (char *)NULL);

                
               
         }  
         else 
         {  
             printf("i am the parent process, my process id is %d/n",getpid()); 
         }  

        
 
        close(new_fd);
    }
    close(sock_fd);
    return 0;
}

#endif

