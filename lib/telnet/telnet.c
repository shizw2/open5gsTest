#include <telnet.h>  
#include "ogs-core.h"

int save_fd;
/*telnet_cmd:ff fb 01 ff fb 03 ff fc 1f*/
//const unsigned char g_chCmdTelnet[9] = {0xff, 0xfb, 0x01, 0xff, 0xfb, 0x03, 0xff, 0xfc, 0x1f};
/*TELNETЭ������*/
/*0XFD: �Ͽ�ѡ������*/
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
int pttTelnetd(int port);
void pttTaskProcess(int sockfd, BOOL bLoginSuccess);
void *pttTelnetdPthread(void * arg);
void *pttTaskProcessPthread(void *arg);
int pttTelnetdStart(void);
void pttTelnetProcCmdCCM(char * pabCmd);
void *pttClientTaskProcess(int sockfd) ;
void mcs(void);
int pttTelnetd_New(ogs_list_t *cli_list);

const char *strPassPromt = "Password:";
const char *strPass = "mcptt";


static TelnetCmdCallback cmd_callback = NULL;

void set_telnet_cmd_callback(TelnetCmdCallback callback) {
    cmd_callback = callback;
}

 
#if 0
int main()
{
    pttTelnetdStart();

    while(1)
    {}
}
#endif

/*����telnet����*/
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
        //exit(0);        //�Ǹ����̣�����������
        waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
        return 0;
    } else if(pid < 0) {
        return -1;//exit(1);        //forkʧ�ܣ��˳�
    }
    printf("Telnet: telnet_starting.......\n");
    pttTelnetd(23);
#endif  

    return 0;
}

int pttTelnetd_New(ogs_list_t *cli_list){
    int server_sockfd;//���������׽���  
    int client_sockfd;//�ͻ����׽���  
    struct sockaddr_in remote_addr; //�ͻ��������ַ�ṹ��  
    socklen_t  sin_size;  
    BOOL bReuseaddr=TRUE;
    pid_t fpid;
    int status;
   
    char buf[OGS_ADDRSTRLEN];
    socklen_t addrlen;
    ogs_socknode_t *node = NULL;
    ogs_sockaddr_t *addr = NULL;
    
    /*�������������׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ��*/  
    if((server_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)  
    {    
        perror("socket");
        return -1;  
    }  

    /*���õ�ַ������*/
    setsockopt (server_sockfd,SOL_SOCKET,SO_REUSEADDR,(char  *)&bReuseaddr, sizeof(bReuseaddr));

    /*���׽��ְ󶨵��������������ַ��*/ 
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

    /*������������*/  
    listen(server_sockfd,1);  

    printf("Telnet: listening for telnet requests....\n");

    sin_size=sizeof(struct sockaddr_in);  
    while(1) 
    {      
        /*�ȴ��ͻ����������󵽴�*/  
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
#if 0
/*telnet ����������*/
int pttTelnetd(int port)  
{  
    int server_sockfd;//���������׽���  
    int client_sockfd;//�ͻ����׽���  
    struct sockaddr_in server_addr;   //�����������ַ�ṹ��  
    struct sockaddr_in remote_addr; //�ͻ��������ַ�ṹ��  
    socklen_t  sin_size;  
    BOOL bReuseaddr=TRUE;
    pid_t fpid;
    int status;
    memset(&server_addr,0,sizeof(server_addr)); //���ݳ�ʼ��--����  
    server_addr.sin_family=AF_INET; //����ΪIPͨ��  
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//������IP��ַ--�������ӵ����б��ص�ַ��  
    //#ifdef FUNC_MCS_CCM
    //server_addr.sin_port=htons(CCM_TELNET_PORT); //������telnet�˿ں� 
    //#endif

    server_addr.sin_port = htons(port);
    
    printf("begin create socket %d.\r\n",port);

    /*�������������׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ��*/  
    if((server_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)  
    {    
        perror("socket");
        return -1;  
    }  

    /*���õ�ַ������*/
    setsockopt (server_sockfd,SOL_SOCKET,SO_REUSEADDR,(char  *)&bReuseaddr, sizeof(bReuseaddr));

    printf("begin bind socket to port %u.\r\n", ntohs(server_addr.sin_port));

        /*���׽��ְ󶨵��������������ַ��*/  
    if (bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr)) < 0)  
    {  
        perror("bind");  
        return -1;  
    }  


    printf("begin listen socket.\r\n");

    /*������������*/  
    listen(server_sockfd,1);  

    printf("Telnet: listening for telnet requests....\n");

    sin_size=sizeof(struct sockaddr_in);  
    while(1) 
    {      
        /*�ȴ��ͻ����������󵽴�*/  
        if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)  
        {  
            perror("accept");  
            return 1;  
        }  
        printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));  
#if 0       
        fpid = fork();
        if (fpid < 0) {
               perror("call fork() err!\n");
               exit(1);
        } else if (fpid == 0) {
               pttClientTaskProcess(client_sockfd);
               close(client_sockfd);
               exit(0);
        } else {
            waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
        }
#else 
        pthread_t id;
        int i,ret;

        ret=pthread_create(&id,NULL,(void *) pttTaskProcessPthread,(void *)&client_sockfd);
        if(ret!=0){
            printf ("Telnet: telnet_starting.......err!\n");
            return -1;
        }
        printf ("Telnet: telnet_started ok!\n");
        pthread_join(id,NULL);
#endif      

    }   
    close(client_sockfd);  
    close(server_sockfd);  
    return 0;   
} 
#endif

void * pttTelnetdPthread(void *arg)
{
    ogs_list_t *cli_list = (ogs_list_t *)arg;
    pttTelnetd_New(cli_list); 
    return NULL;
}

void telnetMain(void *arg)
{
    ogs_list_t *cli_list = (ogs_list_t *)arg;
    pttTelnetd_New(cli_list); 
    return ;
}

/*����telnetЭ������*/
int pttSendTelnetCmd(int fd)
{
    return write(fd, g_chCmdTelnet, sizeof(g_chCmdTelnet));
}

/*��ȡ�����ַ���*/
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
        else  if (*buf == '\r') // �س��������������
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

/*����ض���*/
/*��׼���루stdin������׼�����stdout������׼������Ϣ��stderr�����ļ��ŷֱ�Ϊ0��1��2*/
int pttioStdSet(int src_fd, int dest_fd, int *save_fd) 
{
    *save_fd = dup(dest_fd);
    dup2(src_fd, dest_fd);
    //close(src_fd);
    return *save_fd;
}

/*�ָ�����ض���*/
void pttRecoverIoStdSet(int src_fd, int dest_fd) 
{
    dup2(src_fd, dest_fd);
    close(src_fd);
}

/*�����ַ���*/
int pttCmdAnalyze(char *cmd)
{
    char *ptr = NULL;
    char *ptr_tmp;

    if(strlen(cmd) < 1 || strlen(cmd) > 48) {
        return -1;
    }
    /*ȥ������Ļ��з������������ַ�*/
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
    if(strlen(cmd) < 1 || strlen(cmd) > 48) 
    {
        return -1;
    }
    return 0;
}

#if 1
void mcs(void)
{
    printf("this is mcs system. \r\n");
}
#endif

/*ִ��������Ե�telnet�ն�*/
int pttCmdProcess(int fd, char *cmdLine)
{
    pttioStdSet(fd, 1, &save_fd); /*��׼����ض���*/

    /*��������������*/
    /*ʾ��*/
    //printf("%s\r\n",cmdLine);
    if (cmd_callback != NULL) {
        cmd_callback(cmdLine);
    }

    printf(">\r");
    
    pttRecoverIoStdSet(save_fd, 1); /*�ָ�����ض���*/
    return 0;
}

BOOL pttGetCmdParams(char *pabCmd)
{
    BOOL blLBracket   = FALSE; /*������*/
    BOOL blRBracket   = FALSE; /*������*/
    uint16_t i          = 0;

    if (NULL == pabCmd)
    {
        return FALSE;
    }

    /* ��ʼ�� */
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

            /* ���������Ž��� */
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


/*telnet����������*/
void pttTaskProcess(int sockfd, BOOL bLoginSuccess)
{
    char cmdLine[BUFSIZE]={0};
    int count = 0;
    int ret;

    pttioStdSet(sockfd, 1, &save_fd);
    printf("\r\r\nwelcome to mcs manage system. \r\n");
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
            //exit(1);�Է��Ͽ����ӣ�����
            return ;
        }
        //printf("[%s,%d]rev count:%d,buf:%s\n",__FUNCTION__,__LINE__,count, cmdLine);
        ret = pttCmdAnalyze(cmdLine);
        if(ret == 0) 
        {
            printf("[%s,%d]rev count:%d,buf:%s.\n",__FUNCTION__,__LINE__,count, cmdLine);
            
            if (strncmp(cmdLine, "quit", 4) == 0){
                printf("telnet quit.\r\n");
                //����˳��ͻ���
                // ��ͻ��˷��ͻس����з��������ͻ��˴����ر�����
                //shutdown(sockfd, SHUT_RDWR);

                // �ȴ�һ��ʱ����ȷ���ͻ��˽��ղ�������Ϣ
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
                printf(">");
                fflush(stdout);
                pttRecoverIoStdSet(save_fd, 1);
            }

            
        }else{
            printf("pttCmdAnalyze faild,cmdLine:%s\r\n",cmdLine);
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
 *--- AllocateMemory - ����ռ䲢��d��ָ�����ݸ���
 *------------------------------------------------------
 */
void AllocateMemory(char **s, int l, char *d)
{
    *s = malloc(l + 1);
    bzero(*s, l + 1);
    memcpy(*s, d, l);
}
/************���ڱ��ĵ�*************************************************************
*filename: telnet-server.c
*purpose: ������Linux����C����д��telnet��������û���û��������룬ֱ���Կ��������ߵ���ݵ�¼ϵͳ
*wrote by: zhoulifa(zhoulifa@163.com) ������(http://zhoulifa.bokee.com)
Linux������ Linux֪ʶ������ SOHO�� ������ ���ó�C����
*date time:2007-01-27 17:02
*Note: �κ��˿������⸴�ƴ��벢������Щ�ĵ�����Ȼ���������ҵ��;
* ������ѭGPL
*Thanks to: Google.com
*Hope:ϣ��Խ��Խ����˹����Լ���������Ϊ��ѧ������չ����
* �Ƽ�վ�ھ��˵ļ���Ͻ������죡��л�п�Դǰ���Ĺ��ף�
**********************************************************************************/

/*------------------------------------------------------
 *--- getoption - ����ȡ������Ĳ���
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
        /* ������֧����һЩ������
         * --host IP��ַ ���� -H IP��ַ
         * --port �˿� ���� -P �˿�
         * --back �������� ���� -B ��������
         * --dir ����Ĭ��Ŀ¼ ���� -D ����Ĭ��Ŀ¼
         * --log ��־���·�� ���� -L ��־���·��
         * --daemon ʹ��������̨����ģʽ
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
    pid_t fpid; //fpid��ʾfork�������ص�ֵ  

    /* ��ó������Ĳ������� IP ���˿ڡ�����������ҳ��Ŀ¼��Ŀ¼���λ�õ� */
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
        ("host=%s port=%s back=%s dirroot=%s logdir=%s %s�Ǻ�̨����ģʽ(����ID��%d)\n",
         host, port, back, dirroot, logdir, daemon_y_n?"":"��", getpid());

    /* fork() ���δ��ں�̨����ģʽ�� */
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

    /* �����ӽ����˳����������ʬ���� */
    signal(SIGCHLD, SIG_IGN);

    /* ���� socket */
    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        if (!daemon_y_n) {
            prterrmsg("socket()");
        } else {
            wrterrmsg("socket()");
        }
    }

    /* ���ö˿ڿ������� */
    addrlen = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &addrlen,
               sizeof(addrlen));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(host);
    addrlen = sizeof(struct sockaddr_in);
    /* �󶨵�ַ���˿ڵ���Ϣ */
    if (bind(sock_fd, (struct sockaddr *) &addr, addrlen) < 0) {
        if (!daemon_y_n) {
            prterrmsg("bind()");
        } else {
            wrterrmsg("bind()");
        }
    }

    /* �������� */
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
        /* �������������� */
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
        /* ����һ���ӽ���ȥ�������󣬵�ǰ���̼����ȴ��µ����ӵ��� */
        fpid = fork();
        if (fpid < 0)  
        {
                printf("error in fork!");   
        }
        else if (fpid == 0)
        {  
                printf("i am the child process, my process id is %d/n",getpid());   
                
                wrtinfomsg("new sub pro to solve this client.\r\n");
                         /* ��socket������Ϊ��׼���롢��������������� */
                         dup2(new_fd, 0);
                         dup2(new_fd, 1);
                         dup2(new_fd, 2);
                         /* �л���ָ��Ŀ¼���� */
                         chdir(dirroot);
                         /* ����ʽִ��shell */
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

