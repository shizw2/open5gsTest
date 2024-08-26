#ifndef __UPF_DPDK_H__
#define __UPF_DPDK_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <rte_mempool.h>
#include <rte_ip_frag.h>
#include <rte_arp.h>
#include "dpdk-common.h"
#include "ogs-core.h"
#include "dpdk-arp-nd.h"
#include "tw_timer.h"
#include "hash.h"
#include <rte_common.h>
#include <rte_eal.h>
#include <rte_ip.h>
#include <rte_per_lcore.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_pci.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_lpm.h>
#include <rte_lpm6.h>


#if defined(VER_RELEASE)
#undef ogs_debug
#define ogs_debug(...) 
#endif

#define PORT_N3 0
#define PORT_N6 1

#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */

#define CPU_CACHE_LINE_SIZE 64
#define ALIGNMENT1          (sizeof(uint64_t))
#define ALIGN_UP(x, s)     (((x) + s - 1) & (~(s - 1)))

#define MAX_JUMBO_PKT_LEN  9600

/* Configure how many packets ahead to prefetch, when reading packets */
#define PREFETCH_OFFSET	  3

#define NB_MBUF   (1 << 21)
#define MEMPOOL_CACHE_SIZE 256

#define RTE_RX_DESC_DEFAULT 256
#define RTE_TX_DESC_DEFAULT 1024

#define	DEFAULT_FLOW_TTL	MS_PER_S
#define	DEFAULT_FLOW_NUM	0x1000

#define	MAX_PACKET_FRAG 8
#define MBUF_TABLE_SIZE (MAX_PKT_BURST + MAX_PACKET_FRAG)

/* Should be power of two. */
#define	IP_FRAG_TBL_BUCKET_ENTRIES	16
#define IP_FRAG_NB_MBUF 8192
/*
 * Default byte size for the IPv6 Maximum Transfer Unit (MTU).
 * This value includes the size of IPv6 header.
 */
#define	IPV4_MTU_DEFAULT	RTE_ETHER_MTU
#define	IPV6_MTU_DEFAULT	RTE_ETHER_MTU

#define DKUF_TICKS_SIZE 100
#define DKUF_NB_TIMERS  (1 << 18)

struct mbuf_table {
    uint16_t cnt;
    struct rte_mbuf *m_table[MBUF_TABLE_SIZE];
};

struct lcore_statistic {
    uint64_t rx[2];
    uint64_t tx[2];
    uint64_t rx_event;
    uint64_t f2f_enqueue;
    uint64_t f2f_dequeue;
    uint64_t enqueue;
    uint64_t dequeue;
    uint64_t enqueue_failed;
    uint64_t sess_match[2];
    uint64_t sess_unmatch[2];
    uint64_t sess_report[2];
    uint64_t rx_dropped[2];
    uint64_t tx_dropped[2];
    uint64_t fwd;
};

enum {
    PFCP_THREAD = 1,
    DISPATCHER_THREAD,
    FORWARD_THREAD,
};

struct arp_request {
    struct rte_ether_hdr eth_hdr;
    struct rte_arp_hdr hdr;
};

struct route_gw {
    uint16_t id;
    uint16_t used;
    uint32_t gw;
};

struct route6_gw {
    uint16_t id;
    uint16_t used;
    uint8_t gw[16];
};

#define MAX_DISPATCHER_NUM 4
#define MAX_FWD_NUM 16
struct lcore_conf {
    uint8_t lcore;
    uint8_t rx_queue;
    uint8_t tx_queue;
    uint8_t id;
    uint16_t flag;
    struct ipv4_hashtbl *ipv4_hash;
    ogs_hash_t *ipv6_hash;
    /* IPv4 framed routes trie */
    struct upf_route_trie_node *ipv4_framed_routes;
    /* IPv6 framed routes trie */
    struct upf_route_trie_node *ipv6_framed_routes;
    
    struct rte_lpm *lpm;
    struct route_gw *rt_gw;
    struct rte_lpm6 *lpm6;
    struct route6_gw *rt6_gw;
    struct timer_wheel *twl;
    struct rte_ring *p2f_ring;
    struct rte_ring *f2f_ring;
    union {
        struct rte_ring *d2p_ring;
        struct rte_ring *f2p_ring;
    };
    struct rte_ring *d2f_ring[MAX_FWD_NUM]; //dpt to forward
    struct mbuf_table tx_mbufs[2];
    struct rte_ip_frag_tbl *frag_tbl;
    struct rte_ip_frag_death_row death_row;
    struct arp_hashtbl *arp_tbl;
    struct nd_hashtbl *nd_tbl;

    struct lcore_statistic lstat;
} __attribute__ ((aligned(CACHE_LINE_SIZE)));

typedef struct lcore_conf lcore_conf_t;

struct dpdk_upf_s {
    struct rte_mempool *mpool;
    struct rte_mempool *direct_pool;
    struct rte_mempool *indirect_pool;

    uint64_t sec_diff;
    uint64_t sys_up_sec;
    uint64_t sys_up_ms;
    uint64_t ticks_per_sec;
    uint64_t ticks_per_ms;

    uint64_t tx_offloads[2];
    uint8_t enable;
    uint8_t auto_send_garp;
    char *total_lcore_list;
    uint16_t dpt_num; //dispatcher threads
    uint16_t fwd_num; //forward threads
    uint16_t stopFlag;
    
    uint16_t pfcp_lcore;
    uint16_t dpt_lcore[MAX_DISPATCHER_NUM];
    uint16_t fwd_lcore[MAX_FWD_NUM];

    //uint32_t n3_addr;
    //uint64_t  n3_addr6[2];

    struct {
        uint32_t ipv4;
        uint32_t mask;
        uint32_t gw;
        uint16_t mask_bits;
        uint16_t mask6_bits;
        uint64_t ipv6[2];
        uint64_t mask6[2];
        uint64_t gw6[2];
    } n3_addr;

    struct {
        uint32_t ipv4;
        uint32_t mask;
        uint32_t gw;
        uint16_t mask_bits;
        uint16_t mask6_bits;
        uint64_t ipv6[2];
        uint64_t mask6[2];
        uint64_t gw6[2];
    } n6_addr;

    char startup_cfg[256];

    struct rte_ether_addr mac[2];
    struct arp_request arp_req[2];

    struct rte_ring *p2f_ring[MAX_FWD_NUM];  //pfcp to forward
    struct rte_ring *f2p_ring[MAX_FWD_NUM];
    struct rte_ring *d2p_ring[MAX_DISPATCHER_NUM];  //distatcher to pfcp

    struct lcore_conf lconf[RTE_MAX_LCORE];

} __attribute__ ((aligned(RTE_CACHE_LINE_SIZE)));


void print_stat(void);
int route_add(struct lcore_conf *lconf, uint32_t ip, uint32_t mask, uint32_t gw);
int route6_add(struct lcore_conf *lconf, uint8_t *ip, uint32_t mask, uint8_t *gw);
uint32_t route_find(struct lcore_conf *lconf, uint32_t ip);
uint8_t *route6_find(struct lcore_conf *lconf, uint8_t *ip);

void set_highest_priority(void);
struct dpdk_upf_s *upf_dpdk_context(void);
int upf_dpdk_context_parse_config(void);
int upf_dpdk_open(void);
int upf_dpdk_init(void);
void start_dpdk_threads(void);
int upf_dpdk_close(void);
int dpt_main_loop(void *arg);
int fwd_main_loop(void *arg);

arp_node_t *arp_find(struct lcore_conf *lconf, uint32_t ip, uint16_t port);
nd_node_t *nd_find(struct lcore_conf *lconf, void *ip, uint16_t port);
arp_node_t *arp_find_vxlan(struct lcore_conf *lconf, uint32_t ip, uint16_t port);

bool is_all_veth_up(void);
void check_link_status(void);
void generateMask6(uint16_t mask6_bits, uint64_t* mask6);
static inline void
dump_data(char *buf, int len)
{
    printf("%p ------------------------------------------------- %x\n", buf, len);
    int i = 0;
    for (i = 0; i < len; i++) {
        printf("%02x ", (uint8_t)buf[i]);
        if (!((i+1)%16)) {
            printf("\n");
        }
    }
    printf("\n");
}

extern struct dpdk_upf_s dkuf;

static inline void
send_garp(void)
{
    if (!dkuf.auto_send_garp) {
        return ;
    }

    struct rte_mbuf *n3_garp = dkuf_alloc_arp_request(0, dkuf.n3_addr.ipv4);
    int ret = rte_eth_tx_burst(0, dkuf.fwd_num, &n3_garp, 1);
    if (unlikely(ret < 1)) {
        ogs_error("port 0 send garp failed\n");
        rte_pktmbuf_free(n3_garp);
    }

    struct rte_mbuf *n6_garp = dkuf_alloc_arp_request(1, dkuf.n6_addr.ipv4);
    ret = rte_eth_tx_burst(1, dkuf.fwd_num, &n6_garp, 1);
    if (unlikely(ret < 1)) {
        ogs_error("port 1 send garp failed\n");
        rte_pktmbuf_free(n6_garp);
    }
}

static inline int
port_send_burst(struct lcore_conf *lconf, uint8_t portid)
{
    int cnt = lconf->tx_mbufs[portid].cnt;
    if (cnt == 0) {
        return 0;
    }
    struct rte_mbuf **m_table = lconf->tx_mbufs[portid].m_table;

#if 1
    int nb = rte_eth_tx_prepare(portid, lconf->tx_queue, m_table, cnt);
    if (nb != cnt) {
        ogs_error("lconf %p port %d, mbuf %d %p invalid, %s\n", lconf, portid, nb, m_table[nb], rte_strerror(rte_errno));
        rte_pktmbuf_free(m_table[nb]);
        if (nb != (cnt - 1)) {
            m_table[nb] = m_table[cnt - 1];
        }
        lconf->tx_mbufs[portid].cnt--;
        return port_send_burst(lconf, portid);
    }
#endif
    int ret = 0;
    ret = rte_eth_tx_burst(portid, lconf->tx_queue, m_table, cnt);
    if (unlikely(ret < cnt)) {
        ogs_error("lcore %d burst send %d packets failed, cnt %u\n", lconf->lcore, cnt - ret, cnt);
        lconf->lstat.tx_dropped[portid] += (cnt - ret);
        do {
            rte_pktmbuf_free(m_table[ret]);
        } while (++ret < cnt);
    }
    lconf->tx_mbufs[portid].cnt = 0;
    lconf->lstat.tx[portid] += ret;

    return 0;
}

static inline void
send_burst(struct lcore_conf *lconf)
{
    port_send_burst(lconf, 0);
    port_send_burst(lconf, 1);
}

static inline int
send_single_packet(struct lcore_conf *lconf, uint8_t portid, struct rte_mbuf *m)
{
    lconf->tx_mbufs[portid].m_table[lconf->tx_mbufs[portid].cnt++] = m;
    //printf("%s %d\n", __func__, lconf->tx_mbufs[portid].cnt);

    if (lconf->tx_mbufs[portid].cnt >= MAX_PKT_BURST) {
        port_send_burst(lconf, portid);
    }

    return 0;
}

extern void ip_fragmentation(lcore_conf_t *lconf, struct rte_mbuf *m, uint32_t mtu, uint16_t tx_port, uint8_t is_ipv4);

static inline bool
mbuf_need_frag(struct rte_mbuf *m)
{
    if (LIKELY(m->pkt_len <= (IPV4_MTU_DEFAULT + L2_HDR_LEN))) {
        return false;
    }
    if (!m->port || m->data_len > (IPV4_MTU_DEFAULT + L2_HDR_LEN)) {
        return true;
    }
  
    return false;
}

static inline void 
send_packet(struct lcore_conf *lconf, uint8_t portid, struct rte_mbuf *m, uint8_t is_ipv4)
{
    if (UNLIKELY(mbuf_need_frag(m))) {
        ip_fragmentation(lconf, m, IPV4_MTU_DEFAULT, portid, is_ipv4);
    } else {
        send_single_packet(lconf, portid, m);
    }
}


#endif  /* __UPF_DPDK_H__ */
