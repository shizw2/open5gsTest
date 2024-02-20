#ifndef __DPDK_COMMON_H__
#define __DPDK_COMMON_H__

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_mbuf.h>

#include <unistd.h>


#define MIN(a, b) (a < b ? a : b)

#define RX_PROCESS_SIZE 256
#define RX_BURST_SIZE 32
#define RX_EVENT_SIZE 8

#define MIN_PKT_LEN     64
#define MAC_ADDR_LEN 12
#define ETHER_PROTO_LEN 2
#define VLAN_HDR_LEN 4
#define L2_HDR_LEN 14
#define IP_HDR_LEN 20
#define IP6_HDR_LEN 40
#define UDP_HDR_LEN 8

#define BE_ETH_P_8021Q  0x81
#define BE_ETH_P_IP     0x8
#define BE_ETH_P_IPV6   0xDD86
#define BE_ETH_P_ARP    0x608

#define IP_DEFTTL  64   /* from RFC 1340. */
#define IP_VERSION 0x40
#define IP_HDRLEN  0x05 /* default IP header length == five 32-bits words. */
#define IP_VHL_DEF (IP_VERSION | IP_HDRLEN)

#define UPF_LPM_MAX_RULES 1024
#define UPF_LPM6_MAX_RULES 128
#define UPF_NUM_TBL8S 256 //maybe set by cfg;

enum {
    PREFETCH_READ = 0,
    PREFETCH_WRITE = 1,
    PREFETCH_TYPE_MAX = 2,
};

enum {
    PREFETCH_ONE_SHOT = 0,
    PREFETCH_L1_CACHE = 1,
    PREFETCH_L2_CACHE = 2,
    PREFETCH_L3_CACHE = 3,
    PREFETCH_LOCALITY_MAX = 4,
};

#define CACHE_LINE_SIZE 64

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

//#define NUM_MBUFS 262144
#define MBUF_CACHE_SIZE 256
#define BURST_SIZE 32
//#define TX_BURST_SIZE 32

#define MAX_TIMER_PERIOD 86400 /* 1 day max */

#define MAXBUFSIZE      256
#define MAX_PKT_LEN     65535
#define MAX_PKT_BURST 64

#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)

#define SWAP(a, b) \
        do { (a) = (a)^(b); (b) = (b)^(a); (a) = (b)^(a); } while (0)

static inline char *
ip2str(uint32_t ip)
{
    uint8_t *p = (uint8_t *)&ip;
    static char buf[4][16];
    static int count = 0;
    count = (count + 1) % 4; // 切换到下一个缓冲区
    sprintf(buf[count], "%u.%u.%u.%u%c", p[0], p[1], p[2], p[3], '\0');
    return buf[count];
}

static inline char *
ip2str2(uint32_t ip)
{
    uint8_t *p = (uint8_t *)&ip;
    static char buf[16];
    sprintf(buf, "%u.%u.%u.%u%c", p[0], p[1], p[2], p[3], '\0');
    return buf;
}

static inline char *
ip62str(void *ip)
{
    static char buf[4][64];
    static int count = 0;
    count = (count + 1) % 4; // 切换到下一个缓冲区
    inet_ntop(AF_INET6, (struct in6_addr *)ip, buf[count], 64);
    return buf[count];
}

static inline char *
ip62str2(void *ip)
{
    static char buf[64];
    inet_ntop(AF_INET6, (struct in6_addr *)ip, buf, 64);
    return buf;
}

static inline char *
ip_printf(char *l3_head, uint8_t dst)
{
    uint8_t off = 0;
    struct ip *ip_h = (struct ip *)l3_head;
    if (ip_h->ip_v == 6) {
        off = dst ? 24 : 8;
        return ip62str(l3_head + off);
    } else {
        off = dst ? 16 : 12;
        return ip2str(*(uint32_t *)(l3_head + off));
    }
}

static inline char *
ip_printf2(char *l3_head, uint8_t dst)
{
    uint8_t off = 0;
    struct ip *ip_h = (struct ip *)l3_head;
    if (ip_h->ip_v == 6) {
        off = dst ? 24 : 8;
        return ip62str2(l3_head + off);
    } else {
        off = dst ? 16 : 12;
        return ip2str2(*(uint32_t *)(l3_head + off));
    }
}

static inline char *
mac2str(struct rte_ether_addr *mac)
{
    static char buf[4][64];
    static int count = 0;
    count = (count + 1) % 4; // 切换到下一个缓冲区
    sprintf(buf[count], "%02X:%02X:%02X:%02X:%02X:%02X",
            mac->addr_bytes[0],mac->addr_bytes[1],mac->addr_bytes[2],
            mac->addr_bytes[3],mac->addr_bytes[4],mac->addr_bytes[5]);
    return buf[count];
}

static inline void
mac_print(struct rte_ether_addr *mac)
{
    printf("MAC addr: %02X:%02X:%02X:%02X:%02X:%02X\n\n",
            mac->addr_bytes[0],mac->addr_bytes[1],mac->addr_bytes[2],
            mac->addr_bytes[3],mac->addr_bytes[4],mac->addr_bytes[5]);
}

static inline void
mac_copy(void *src, void *dst)
{
    *(uint32_t *)((char *)dst) = *(uint32_t *)((char *)src);
    *(uint16_t *)((char *)dst + 4) = *(uint16_t *)((char *)src + 4);
}

static inline void
mac_swap(void *src, void *dst)
{
    SWAP(*(uint32_t *)src, *(uint32_t *)dst);
    SWAP(*(uint16_t *)((char *)src + 4), *(uint16_t *)((char *)dst + 4));
}

static inline uint8_t
mac_cmp(char *src, char *dst)
{
    return (*(src) == *(dst) && *(src + 1) == *(dst + 1) &&
            *(src + 2) == *(dst + 2) && *(src + 3) == *(dst + 3) &&
            *(src + 4) == *(dst + 4) && *(src + 5) == *(dst + 5));
}

static inline uint8_t
ipv4_cmp(void *src, void *dst)
{
    return (*(uint32_t *)src == *(uint32_t *)dst);
}

static inline void
ipv6_swap(void *src, void *dst)
{
    SWAP(*(uint64_t *)src, *(uint64_t *)dst);
    SWAP(*((uint64_t *)src + 1), *((uint64_t *)dst + 1));
}

static inline uint8_t
ipv6_cmp(void *src, void *dst)
{
    return ((*(uint64_t *)src == *(uint64_t *)dst) &&
            (*((uint64_t *)src + 1) == (*(uint64_t *)dst + 1)));
}

enum {
    PKT_TYPE_INIT,
    PKT_TYPE_IP_N3,
    PKT_TYPE_IP_N6,
    PKT_TYPE_PING,
    PKT_TYPE_PING6,
    PKT_TYPE_ARP,
    PKT_TYPE_ND,
    PKT_TYPE_MAX
};

struct packet {
    uint8_t is_ipv4:1;
    uint8_t l2_len:7;
    uint16_t l3_len;
    uint8_t l4_len;
    uint8_t tunnel_len;
    uint16_t inner_l3_len;
    uint8_t inner_l4_len;
    uint8_t pkt_type;
    struct packet *next;
} __attribute__((aligned(CACHE_LINE_SIZE)));


static inline void *
packet_meta(
        struct packet *pkt)
{
    assert(pkt);

    return (struct rte_mbuf *)pkt - 1;
}

static inline uint16_t
packet_data_len(
        struct packet *pkt)
{
    assert(pkt);

    return rte_pktmbuf_data_len((struct rte_mbuf *)pkt - 1);
}

static inline void *
packet_data(
        struct packet *pkt)
{
    assert(pkt);

    return rte_pktmbuf_mtod((struct rte_mbuf *)pkt - 1, void *);
}

static inline __attribute__((always_inline)) void
packet_prefetch(
        struct packet *pkt,
        unsigned int type,
        unsigned int locality)
{
    assert(pkt);
    assert(type < PREFETCH_TYPE_MAX);
    assert(locality < PREFETCH_LOCALITY_MAX);

    __builtin_prefetch(pkt, type, locality);

    return;
}

static inline __attribute__((always_inline)) void
packet_meta_prefetch(
        struct packet *pkt,
        unsigned int type,
        unsigned int locality)
{
    assert(pkt);
    assert(type < PREFETCH_TYPE_MAX);
    assert(locality < PREFETCH_LOCALITY_MAX);

    __builtin_prefetch(packet_meta(pkt), type, locality);

    return;
}

static inline __attribute__((always_inline)) void
packet_data_prefetch(
        struct packet *pkt,
        unsigned int type,
        unsigned int locality)
{
    uint8_t *data;

    assert(pkt);
    assert(type < PREFETCH_TYPE_MAX);
    assert(locality < PREFETCH_LOCALITY_MAX);

    data = packet_data(pkt);

    __builtin_prefetch(data, type, locality);
    __builtin_prefetch(data + CACHE_LINE_SIZE, type, locality);

    return;
}

static inline struct packet *
packet_clone(
        struct packet *pkt)
{
    struct packet *new_pkt;
    struct rte_mbuf *m, *new_m;

    assert(pkt);

    m = packet_meta(pkt);
    new_m = rte_pktmbuf_clone(m, m->pool);
    if (UNLIKELY(!new_m)) {
        return NULL;
    }

    new_pkt = (struct packet *)(new_m + 1);
    *new_pkt = *pkt;

    return new_pkt;
}

#endif /* __DPDK_COMMON_H__ */
