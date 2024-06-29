#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 65536

int main() {
    int s;
    struct sockaddr_in sin;
    unsigned char buffer[BUFFER_SIZE];
    struct ip *ip_header;

    // 创建原始套接字
    //s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字到本地地址
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(0);
    //sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_addr.s_addr = inet_addr("192.168.6.33"); // 设置为特定的IP地址
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        close(s);
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项，允许接收所有IP报文
    int one = 1;
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        close(s);
        exit(EXIT_FAILURE);
    }

    // 循环接收IP报文
    while (1) {
        ssize_t size = recv(s, buffer, BUFFER_SIZE, 0);
        if (size < 0) {
            perror("recv");
            continue;
        }

        // 打印IP头部信息
        ip_header = (struct ip *)buffer;
        printf("Received packet from %s to %s\n",
               inet_ntoa(ip_header->ip_src),
               inet_ntoa(ip_header->ip_dst));
    }

    close(s);
    return 0;
}
