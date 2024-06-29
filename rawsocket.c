#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <netinet/ip.h>  
#include <arpa/inet.h>  
#include <netdb.h>  
#include <linux/if_ether.h>  
#include <errno.h>  
  
#define BUF_SIZE 2048  
  
int main(int argc, char *argv[]) {  
    int sockfd;  
    struct sockaddr_in sin;  
    char buffer[BUF_SIZE];  
    struct ip *iphdr;  
  
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {  
        perror("socket creation failed");  
        exit(EXIT_FAILURE);  
    }  
  
    // 允许接收所有IP地址的数据包（如果需要的话）  
    int val = 1;  
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val)) < 0) {  
        perror("setsockopt() failed");  
        exit(EXIT_FAILURE);  
    }  
  
    // 绑定到特定的网络接口（如果需要的话）  
    // 你可以设置sin.sin_addr.s_addr来指定接口IP，但通常设置为INADDR_ANY来接收所有接口的数据  
    memset(&sin, 0, sizeof(sin));  
    sin.sin_family = AF_INET;  
    sin.sin_addr.s_addr = INADDR_ANY;  
    sin.sin_port = htons(0); // 不需要端口号，因为我们处理的是IP层  
  
    // 实际上，对于原始套接字，我们不需要bind，但如果你需要特定的接口，可以取消注释以下行  
    // if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {  
    //     perror("bind failed");  
    //     exit(EXIT_FAILURE);  
    // }  
  
    while (1) {  
        int len = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
        if (len < 0) {  
            perror("recvfrom");  
            exit(EXIT_FAILURE);  
        }  
  
        // 假设buffer现在包含了一个完整的IP数据包  
        iphdr = (struct ip *)buffer;  
  
        // 在这里处理数据包...  
        // 例如，打印IP头的一些字段  
        printf("Received IP packet with source %s and destination %s\n",  
               inet_ntoa(iphdr->ip_src),  
               inet_ntoa(iphdr->ip_dst));  
  
        // ... 其他处理 ...  
    }  
  
    close(sockfd);  
    return 0;  
}
