#include "upf-dpdk.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/resource.h>
#include <arpa/inet.h>
#include "context.h"

struct dpdk_upf_s dkuf;


#define JUMBO_FRAME_MAX_SIZE    0x2600
#define DEFAULT_RSS_VALUE (ETH_RSS_IP | ETH_RSS_IPV6)

static struct rte_eth_conf port_conf = {
    .rxmode = {
        .mq_mode = ETH_MQ_RX_RSS,
        .max_rx_pkt_len = JUMBO_FRAME_MAX_SIZE,
        .split_hdr_size = 0,
        .offloads = 0,
    },
    .txmode = {
        .mq_mode = ETH_MQ_TX_NONE,
        .offloads = (DEV_TX_OFFLOAD_IPV4_CKSUM |
                DEV_TX_OFFLOAD_MULTI_SEGS),
    }, 
    .rx_adv_conf = {
        .rss_conf = {
            .rss_key = NULL,
            .rss_hf = DEFAULT_RSS_VALUE,
        },
    },
};

void print_stat(void)
{
    uint16_t i = 0;
    uint16_t lcore_id = 0;
    struct lcore_statistic *lstat = NULL;
    struct lcore_statistic lstat_tot;

    memset(&lstat_tot, 0, sizeof(struct lcore_statistic));

    for (i = 0; i < dkuf.dpt_num; i++) {
        lcore_id = dkuf.dpt_lcore[i];
        lstat = &dkuf.lconf[lcore_id].lstat;
        lstat_tot.rx[0] += lstat->rx[0];
        lstat_tot.rx[1] += lstat->rx[1];
        lstat_tot.rx_dropped[0] += lstat->rx_dropped[0];
        lstat_tot.rx_dropped[1] += lstat->rx_dropped[1];
        lstat_tot.enqueue += lstat->enqueue;
        lstat_tot.enqueue_failed += lstat->enqueue_failed;
    }
    for (i = 0; i < dkuf.fwd_num; i++) {
        lcore_id = dkuf.fwd_lcore[i];
        lstat = &dkuf.lconf[lcore_id].lstat;
        lstat_tot.tx[0] += lstat->tx[0];
        lstat_tot.tx[1] += lstat->tx[1];
        lstat_tot.tx_dropped[0] += lstat->tx_dropped[0];
        lstat_tot.tx_dropped[1] += lstat->tx_dropped[1];
        lstat_tot.rx_event += lstat->rx_event;
        lstat_tot.dequeue += lstat->dequeue;
        lstat_tot.f2f_enqueue += lstat->f2f_enqueue;
        lstat_tot.f2f_dequeue += lstat->f2f_dequeue;
        lstat_tot.sess_report[0] += lstat->sess_report[0];
        lstat_tot.sess_report[1] += lstat->sess_report[1];
        lstat_tot.sess_match[0] += lstat->sess_match[0];
        lstat_tot.sess_match[1] += lstat->sess_match[1];
        lstat_tot.sess_unmatch[0] += lstat->sess_unmatch[0];
        lstat_tot.sess_unmatch[1] += lstat->sess_unmatch[1];
    }

    lstat = &dkuf.lconf[dkuf.pfcp_lcore].lstat;
    if (lstat_tot.rx[0] == lstat->rx[0] && lstat_tot.rx[1] == lstat->rx[1]) {
        //fprintf(stderr, "no pkt recv\n");
        return ;
    }

    printf("%lu =============================================================\n", dkuf.sys_up_sec);
    printf("total rx %lu enqueue %lu enq_failed %lu dequeue %lu\n",
            (lstat_tot.rx[0] - lstat->rx[0]) + (lstat_tot.rx[1] - lstat->rx[1]),
            lstat_tot.enqueue - lstat->enqueue, lstat_tot.enqueue_failed - lstat->enqueue_failed,
            lstat_tot.dequeue - lstat->dequeue);
    printf("port 0 rx %lu rx_drop %lu tx %lu tx_drop %lu\n",
            lstat_tot.rx[0] - lstat->rx[0], lstat_tot.rx_dropped[0] - lstat->rx_dropped[0],
            lstat_tot.tx[0] - lstat->tx[0], lstat_tot.tx_dropped[0] - lstat->tx_dropped[0]);
    printf("port 0 report %lu unmatch %lu match %lu\n",
            lstat_tot.sess_report[0] - lstat->sess_report[0],
            lstat_tot.sess_unmatch[0] - lstat->sess_unmatch[0],
            lstat_tot.sess_match[0] - lstat->sess_match[0]);
    printf("port 1 rx %lu rx_drop %lu tx %lu tx_drop %lu\n",
            lstat_tot.rx[1] - lstat->rx[1], lstat_tot.rx_dropped[1] - lstat->rx_dropped[1],
            lstat_tot.tx[1] - lstat->tx[1], lstat_tot.tx_dropped[1] - lstat->tx_dropped[1]);
    printf("port 1 report %lu unmatch %lu match %lu\n",
            lstat_tot.sess_report[1] - lstat->sess_report[1],
            lstat_tot.sess_unmatch[1] - lstat->sess_unmatch[1],
            lstat_tot.sess_match[1] - lstat->sess_match[1]);

    memcpy(lstat, &lstat_tot, sizeof(struct lcore_statistic));

    uint16_t port = 0;
    uint16_t nb_ports = rte_eth_dev_count_avail();
    struct rte_eth_stats stats;
    for (port = 0; port < nb_ports; port++) {
        rte_eth_stats_get(port, &stats);

        printf("port %2u rcv pkts %16lu miss %16lu ierror %16lu oerror %16lu nombuf %16lu\n",
                port, stats.ipackets, stats.imissed, stats.ierrors, stats.oerrors, stats.rx_nombuf);
    }
    printf("mbuf pool avail cnt mpool %u direct %u indirect %u\n",
            rte_mempool_avail_count(dkuf.mpool),
            rte_mempool_avail_count(dkuf.direct_pool),
            rte_mempool_avail_count(dkuf.indirect_pool));
    //printf("================================================================end\n");
    return;
}

void check_link_status(void)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
    uint16_t portid;
    uint8_t count, all_ports_up, print_flag = 0;
    struct rte_eth_link link;

    printf("\nChecking link status\n");

    for (count = 0; count <= MAX_CHECK_TIME; count++) {
        all_ports_up = 1;
        RTE_ETH_FOREACH_DEV(portid) {
            memset(&link, 0, sizeof(link));
            rte_eth_link_get_nowait(portid, &link);

            if (print_flag == 1) {
                if (link.link_status) {
                    printf("Port%d Link Up - speed %uMbps - %s\n", portid, link.link_speed,
                            (link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
                            ("full-duplex") : ("half-duplex"));
                } else {
                    printf("Port %d Link Down\n", portid);
                }
                continue;
            }
            /* clear all_ports_up flag if any link down */
            if (link.link_status == ETH_LINK_DOWN) {
                all_ports_up = 0;
                break;
            }
        }
        /* after finally printing all link status, get out */
        if (print_flag == 1)
            break;

        if (all_ports_up == 0) {
            printf(".");
            fflush(stdout);
            rte_delay_ms(CHECK_INTERVAL);
        }

        /* set the print_flag if all ports up or timeout */
        if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
            print_flag = 1;
            printf("done\n");
        }
    }
}

void set_highest_priority(void)
{
    int nzero = sysconf(_SC_NZERO);
    printf("nzero %d\n", nzero);
    if (nice(nzero) < 0) {
        printf("set nice failed\n");
    }

    if (setpriority(PRIO_PROCESS, 0, -20) != 0) {
        printf("set pri failed\n");
    }
}

struct dpdk_upf_s *upf_dpdk_context(void)
{
    return &dkuf;
}

static int setup_ip_frag_tbl(void)
{
    uint32_t max_flow_num = DEFAULT_FLOW_NUM;
    uint32_t max_flow_ttl = DEFAULT_FLOW_TTL;
    int socket;
    uint64_t frag_cycles;
    struct lcore_conf *lconf = NULL;

    frag_cycles = (rte_get_tsc_hz() + MS_PER_S - 1) / MS_PER_S * max_flow_ttl;

    uint32_t i = 0;
    for (i = 0; i < RTE_MAX_LCORE; i++) {
        lconf = &dkuf.lconf[i];
        if (lconf->flag != DISPATCHER_THREAD) {
            continue;
        }
        socket = rte_lcore_to_socket_id(i);
        if (socket == SOCKET_ID_ANY) socket = 0;

        lconf->frag_tbl = rte_ip_frag_table_create(max_flow_num,
                IP_FRAG_TBL_BUCKET_ENTRIES,
                max_flow_num, frag_cycles,
                socket);
        if (!lconf->frag_tbl) {
            rte_exit(EXIT_FAILURE, "lcore %u ip_frag_tbl_create(%u) failed\n", i, max_flow_num);
        }
    }
    return OGS_OK;
} 

static void clear_ip_frag_tbl(void)
{
    struct lcore_conf *lconf = NULL;

    uint32_t i = 0;
    for (i = 0; i < RTE_MAX_LCORE; i++) {
        lconf = &dkuf.lconf[i];
        if (lconf->flag != DISPATCHER_THREAD) {
            continue;
        }
        if (lconf->frag_tbl) {
            rte_ip_frag_table_destroy(lconf->frag_tbl);
        }
    }
} 

unsigned int ip_hash_func(const char *ip, int *klen);
unsigned int ip_hash_func(const char *ip, int *klen)
{
    uint32_t *ptr = (uint32_t *)ip;
    uint32_t hash = 0;

    if (*klen == 4) {
        hash = *ptr;
    } else if (*klen == 16) {
        hash = ptr[0] ^ ptr[1] ^ ptr[2] ^ ptr[3];
    }

    return hash;
}

static int ring_init(void)
{
    uint16_t i, j;
    char name[128];
    int qsize = 4096; //set by cnf later ,default 4096

    for (i = 0; i < dkuf.dpt_num; i++) {
        sprintf(name, "RING_DPT_2_PFCP_%d%c", i, '\0');
        dkuf.d2p_ring[i] = rte_ring_create(name, qsize, 0,
                RING_F_SP_ENQ | RING_F_SC_DEQ);
        if (dkuf.d2p_ring[i] == NULL) {
            fprintf(stderr, "rte_ring_create %s fails\n", name);
            return -1;
        }
    }
    for (i = 0; i < dkuf.fwd_num; i++) {
        sprintf(name, "RING_PFCP_2_FWD_%d%c", i, '\0');
        dkuf.p2f_ring[i] = rte_ring_create(name, qsize, 0,
                RING_F_SP_ENQ | RING_F_SC_DEQ);
        if (dkuf.p2f_ring[i] == NULL) {
            fprintf(stderr, "rte_ring_create %s fails\n", name);
            return -1;
        }
        sprintf(name, "RING_FWD_2_PFCP_%d%c", i, '\0');
        dkuf.f2p_ring[i] = rte_ring_create(name, qsize, 0,
                RING_F_SP_ENQ | RING_F_SC_DEQ);
        if (dkuf.f2p_ring[i] == NULL) {
            fprintf(stderr, "rte_ring_create %s fails\n", name);
            return -1;
        }
    }
    uint16_t lcore_id = 0;
    dkuf.lconf[dkuf.pfcp_lcore].flag = PFCP_THREAD;

    for (i = 0; i < dkuf.dpt_num; i++) {
        lcore_id = dkuf.dpt_lcore[i];
        dkuf.lconf[lcore_id].flag = DISPATCHER_THREAD;
        dkuf.lconf[lcore_id].d2p_ring = dkuf.d2p_ring[i];
        dkuf.lconf[lcore_id].rx_queue = i;
        dkuf.lconf[lcore_id].lcore = lcore_id;
        for (j = 0; j < dkuf.fwd_num; j++) {
            sprintf(name, "RING_DISPATCH%d_2_FWD%d%c", i, j, '\0');
            dkuf.lconf[lcore_id].d2f_ring[j] = rte_ring_create(name, qsize, 0,
                    RING_F_SP_ENQ | RING_F_SC_DEQ);
            if (dkuf.lconf[lcore_id].d2f_ring[j] == NULL) {
                fprintf(stderr, "rte_ring_create %s fails\n", name);
                return -1;
            }
        }
    }

    for (i = 0; i < dkuf.fwd_num; i++) {
        lcore_id = dkuf.fwd_lcore[i];
        dkuf.lconf[lcore_id].flag = FORWARD_THREAD;
        dkuf.lconf[lcore_id].p2f_ring = dkuf.p2f_ring[i];
        dkuf.lconf[lcore_id].f2p_ring = dkuf.f2p_ring[i];
        dkuf.lconf[lcore_id].tx_queue = i;
        dkuf.lconf[lcore_id].lcore = lcore_id;
        for (j = 0; j < dkuf.dpt_num; j++) {
            dkuf.lconf[lcore_id].d2f_ring[j] = dkuf.lconf[dkuf.dpt_lcore[j]].d2f_ring[i];
        }
        sprintf(name, "RING_FWD_2_FWD_%d%c", i, '\0');
        dkuf.lconf[lcore_id].f2f_ring = rte_ring_create(name, qsize >> 2, 0,
                RING_F_SC_DEQ);
        if (dkuf.lconf[lcore_id].f2f_ring == NULL) {
            fprintf(stderr, "rte_ring_create %s fails\n", name);
            return -1;
        }
    }

    return OGS_OK;
}

static void clear_ring(void)
{
    uint32_t i = 0;

    for (i = 0; i < dkuf.dpt_num; i++) {
        rte_ring_free(dkuf.d2p_ring[i]);
    }
    for (i = 0; i < dkuf.fwd_num; i++) {
        rte_ring_free(dkuf.p2f_ring[i]);
        rte_ring_free(dkuf.f2p_ring[i]);
        rte_ring_free(dkuf.lconf[dkuf.fwd_lcore[i]].f2f_ring);
    }
}

int get_free_rt_gw_id(struct lcore_conf *lconf);
int get_free_rt_gw_id(struct lcore_conf *lconf)
{
    int i = 0;
    for (i = 0; i < UPF_LPM_MAX_RULES; i++) {
        if (!lconf->rt_gw[i].used) {
            lconf->rt_gw[i].used = 1;
            return i;
        }
    }
    return -1;
}

int get_free_rt6_gw_id(struct lcore_conf *lconf);
int get_free_rt6_gw_id(struct lcore_conf *lconf)
{
    int i = 0;
    for (i = 0; i < UPF_LPM_MAX_RULES; i++) {
        if (!lconf->rt6_gw[i].used) {
            lconf->rt6_gw[i].used = 1;
            return i;
        }
    }
    return -1;
}

int route_add(struct lcore_conf *lconf, uint32_t ip, uint32_t mask, uint32_t gw)
{
    int id = get_free_rt_gw_id(lconf);
    if (id < 0) {
        return -1;
    }
    lconf->rt_gw[id].gw = gw;
    rte_lpm_add(lconf->lpm, ip, mask, id);

    return id;
}

int route6_add(struct lcore_conf *lconf, uint8_t *ip, uint32_t mask, uint8_t *gw)
{
    int id = get_free_rt6_gw_id(lconf);
    if (id < 0) {
        return -1;
    }
    memcpy(lconf->rt6_gw[id].gw, gw, 16);
    rte_lpm6_add(lconf->lpm6, ip, mask, id);

    return id;
}

uint32_t route_find(struct lcore_conf *lconf, uint32_t ip)
{
    uint32_t nexthop = 0;
    if (rte_lpm_lookup(lconf->lpm, ip, &nexthop) < 0) {
        return 0;
    }
    return lconf->rt_gw[nexthop].gw;
}

uint8_t *route6_find(struct lcore_conf *lconf, uint8_t *ip)
{
    uint32_t nexthop = 0;
    if (rte_lpm6_lookup(lconf->lpm6, ip, &nexthop) < 0) {
        return 0;
    }
    return lconf->rt6_gw[nexthop].gw;
}

static int fwd_tbl_init(void)
{
    uint16_t i;
    uint16_t lcore_id;
    struct lcore_conf *lconf = NULL;
    struct rte_lpm_config config = {UPF_LPM_MAX_RULES, UPF_NUM_TBL8S, 0};
    struct rte_lpm6_config config6 = {UPF_LPM6_MAX_RULES, UPF_NUM_TBL8S, 0};
    struct timer_wheel_args twprm = {DKUF_TICKS_SIZE, DKUF_NB_TIMERS};

    char name[128];

    for (i = 0; i < dkuf.fwd_num; i++) {
        lcore_id = dkuf.fwd_lcore[i];
        lconf = &dkuf.lconf[lcore_id];

        lconf->ipv4_hash = ipv4_hash_create(1024);
        lconf->ipv6_hash = ogs_hash_make_custom(ip_hash_func);

        sprintf(name, "LPM_%d_CORE_%d%c", i, lcore_id, '\0');
        lconf->lpm = rte_lpm_create(name, 0, &config);
        lconf->rt_gw = calloc(config.max_rules, sizeof(struct route_gw));

        sprintf(name, "LPM6_%d_CORE_%d%c", i, lcore_id, '\0');
        lconf->lpm6 = rte_lpm6_create(name, 0, &config6);
        lconf->rt6_gw = calloc(config6.max_rules, sizeof(struct route6_gw));

        lconf->arp_tbl = arp_hash_create(ARP_TABLE_NUM);
        lconf->nd_tbl = nd_hash_create(ND_TABLE_NUM);

        lconf->twl = tw_timer_create(&twprm, dkuf.sys_up_ms);
        //flow table init;
    }

    return OGS_OK;
}

static void clear_fwd_tbl(void)
{
    uint16_t i;
    uint16_t lcore_id;
    struct lcore_conf *lconf = NULL;

    for (i = 0; i < dkuf.fwd_num; i++) {
        lcore_id = dkuf.fwd_lcore[i];
        lconf = &dkuf.lconf[lcore_id];
        ipv4_hash_destroy(lconf->ipv4_hash);
        ogs_hash_destroy(lconf->ipv6_hash);

        rte_lpm_free(lconf->lpm);
        rte_lpm6_free(lconf->lpm6);
        free(lconf->rt_gw);
        free(lconf->rt6_gw);

        arp_hash_destroy(lconf->arp_tbl);
        nd_hash_destroy(lconf->nd_tbl);

        tw_timer_free(lconf->twl);
    }
}

static int mem_init(void)
{
    uint32_t nb_mbuf = 0;
    uint32_t nb_frag_mbuf = 0;
    char s[64];

    /* exceed (rx ring + tx ring + d2f_ring + f2f_ring + flush buffer + pkt_list); */
    nb_mbuf = (RTE_TX_DESC_DEFAULT *  dkuf.dpt_num) << 6;
    /* exceed tx ring; */
    nb_frag_mbuf = (RTE_TX_DESC_DEFAULT *  dkuf.dpt_num) << 2;

    snprintf(s, sizeof(s), "mbuf_pool");
    dkuf.mpool = rte_pktmbuf_pool_create(s, nb_mbuf,
            MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, 0);
    if (dkuf.mpool == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");
    }

    snprintf(s, sizeof(s), "pool_direct");
    dkuf.direct_pool = rte_pktmbuf_pool_create(s, nb_frag_mbuf, 32,
            0, RTE_MBUF_DEFAULT_BUF_SIZE, 0);
    if (dkuf.direct_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot create direct mempool\n");
    }

    snprintf(s, sizeof(s), "pool_indirect");
    dkuf.indirect_pool = rte_pktmbuf_pool_create(s, nb_frag_mbuf, 32,
            0, RTE_MBUF_DEFAULT_BUF_SIZE, 0);
    if (dkuf.indirect_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot create indirect mempool\n");
    }

    return OGS_OK;
}

static uint32_t set_n3_addr(void)
{
    ogs_socknode_t *node = NULL;

    ogs_list_for_each(&ogs_gtp_self()->gtpu_list, node) {
        if (node->addr->sin.sin_family == AF_INET) {
            dkuf.n3_addr.ipv4 = *(uint32_t *)&node->addr->sin.sin_addr;
        } else if (node->addr->sin6.sin6_family == AF_INET6) {
            memcpy(dkuf.n3_addr.ipv6, &node->addr->sin6.sin6_addr, 16);
        }
    }

    return 0;
}

void arp_req_init(uint16_t portid);
void arp_req_init(uint16_t portid)
{
    struct arp_request *arp_req = &dkuf.arp_req[portid];
    struct rte_ether_hdr *eth_h = &arp_req->eth_hdr;
    struct rte_arp_hdr *arp_h = (struct rte_arp_hdr *)(eth_h + 1);
     
    rte_eth_macaddr_get(portid, &eth_h->s_addr);
    ogs_info("port:%d, mac:%s.",portid,mac2str(&eth_h->s_addr));

    memset(&eth_h->d_addr, 0xFF, RTE_ETHER_ADDR_LEN);
    eth_h->ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_ARP);

    arp_h->arp_hardware = rte_cpu_to_be_16(RTE_ARP_HRD_ETHER);
    arp_h->arp_protocol = rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4);
    arp_h->arp_hlen = RTE_ETHER_ADDR_LEN;
    arp_h->arp_plen = sizeof(uint32_t);
    arp_h->arp_opcode = rte_cpu_to_be_16(RTE_ARP_OP_REQUEST);

    mac_copy(&eth_h->s_addr, &dkuf.mac[portid]);
    mac_copy(&eth_h->s_addr, &arp_h->arp_data.arp_sha);
    memset(&arp_h->arp_data.arp_tha, 0xFF, RTE_ETHER_ADDR_LEN);
    arp_h->arp_data.arp_sip = 0;
    arp_h->arp_data.arp_tip = 0;
}

static void reset_port_conf(struct rte_eth_conf *pconf, struct rte_eth_dev_info *dev_info)
{
    if ((pconf->rx_adv_conf.rss_conf.rss_hf & dev_info->flow_type_rss_offloads)
            != pconf->rx_adv_conf.rss_conf.rss_hf) {
        pconf->rx_adv_conf.rss_conf.rss_hf &= dev_info->flow_type_rss_offloads;
        printf("reset rss value 0x%lx\n", pconf->rx_adv_conf.rss_conf.rss_hf);
    }
    if ((pconf->rxmode.offloads & dev_info->rx_offload_capa) != pconf->rxmode.offloads) {
        pconf->rxmode.offloads &= dev_info->rx_offload_capa;
        printf("reset rx offload 0x%lx\n", pconf->rxmode.offloads);
    }
    if ((pconf->txmode.offloads & dev_info->tx_offload_capa) != pconf->txmode.offloads) {
        pconf->txmode.offloads &= dev_info->tx_offload_capa;
        printf("reset tx offload 0x%lx\n", pconf->txmode.offloads);
    }
    if (dev_info->max_rx_pktlen < pconf->rxmode.max_rx_pkt_len) {
        printf("reset max rx_pkt_len %d\n", dev_info->max_rx_pktlen);
        pconf->rxmode.max_rx_pkt_len = dev_info->max_rx_pktlen;
    }
}

static int port_init(uint16_t portid)
{
    uint16_t nb_rxd = RTE_RX_DESC_DEFAULT;
    uint16_t nb_txd = RTE_TX_DESC_DEFAULT;

    struct rte_eth_dev_info dev_info;
    struct rte_eth_txconf *txconf = &dev_info.default_txconf;
    struct rte_eth_rxconf *rxconf = &dev_info.default_rxconf;
    struct rte_eth_conf local_port_conf = port_conf;

    rte_eth_dev_info_get(portid, &dev_info);
    reset_port_conf(&local_port_conf, &dev_info);
    uint16_t tx_queues = dkuf.auto_send_garp ? (dkuf.fwd_num + 1) : dkuf.fwd_num;

    printf("init port %d queue rx %d tx %d\n", portid, dkuf.dpt_num, tx_queues);
    int ret = rte_eth_dev_configure(portid, dkuf.dpt_num, tx_queues, &local_port_conf);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "Cannot configure device: err %d, port %u\n", ret, portid);
    }

    ret = rte_eth_dev_adjust_nb_rx_tx_desc(portid, &nb_rxd, &nb_txd);
    if (ret != 0) {
        rte_exit(EXIT_FAILURE, "Cannot adjust rx tx desc: err %d, port %u\n", ret, portid);
    }
    arp_req_init(portid);

    rxconf->offloads = local_port_conf.rxmode.offloads;
    uint16_t j = 0;
    for (j = 0; j < dkuf.dpt_num; j++) {
        printf("rte_eth_rx_queue_setup: port %u, queue %u\n", portid, j);
        ret = rte_eth_rx_queue_setup(portid, j, nb_rxd, 0, rxconf, dkuf.mpool);
        if (ret < 0) {
            rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup: err %d, port %u, queue %u\n", ret, portid, j);
        }
    } 
    txconf->offloads = local_port_conf.txmode.offloads;
    dkuf.tx_offloads[portid] = txconf->offloads;
    for (j = 0; j < tx_queues; j++) {
        printf("rte_eth_tx_queue_setup: port %u, queue %u\n", portid, j);
        ret = rte_eth_tx_queue_setup(portid, j, nb_txd, 0, txconf);
        if (ret < 0) {
            rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup: err %d, port %u, queue %u\n", ret, portid, j);
        }
    }

    ret = rte_eth_dev_start(portid);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "rte_eth_dev_start:err %d, port %u\n", ret, portid);
    }
    printf("init port %d, done!\n", portid);
    rte_eth_promiscuous_enable(portid);

    return OGS_OK;
}

int construct_lcore_list(void);
int construct_lcore_list(void)
{
    char buf[4096];
    int offset = 0;
    int n = 0;
    int i = 0;

    n = snprintf(buf + offset, 4096, "%d", dkuf.pfcp_lcore );
    offset += n;

    for (i = 0; i < dkuf.dpt_num; i++) {
        n = snprintf(buf + offset, 4096 - offset, ",%d", dkuf.dpt_lcore[i]);
        offset += n;
    }
    for (i = 0; i < dkuf.fwd_num; i++) {
        n = snprintf(buf + offset, 4096 - offset, ",%d", dkuf.fwd_lcore[i]);
        offset += n;
    }

    dkuf.total_lcore_list = strdup(buf);

    return OGS_OK;
}

int eal_init(void);
int eal_init(void)
{
    construct_lcore_list();

    char *argv[] = {
        (char *)"",
        (char *)"-l",
        dkuf.total_lcore_list,
    };
    const int argc = 3;

    int ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "Invalid EAL parameters\n");
    }

    return OGS_OK;
}

static void parse_lcores(const char *str, uint16_t *array, uint16_t *num, uint16_t max)
{
    char *p1, *p2, *p3;
    uint32_t start, end;

    printf("get lcores: ");
    p1 = (char *)str;
    do {
        p2 = strchr(p1, ',');
        if (p2) {
            *p2 = '\0';
        }
        if ((p3 = strchr(p1, '-'))) {
            *p3 = '\0';
            start = atoi(p1);
            end = atoi(p3 + 1);
            while (start <= end) {
                array[(*num)++] = start++;
                printf("%d ", array[(*num) - 1]);
                if (*num == max) {
                    break;
                }
            }
        } else {
            array[(*num)++] = atoi(p1);
            printf("%d ", array[(*num) - 1]);
            if (*num == max) {
                break;
            }
        }
        p1 = p2 ? (p2 + 1) : NULL;
    } while (p1);
    printf("\nlcore num: %d\n", *num);
}

static int auto_set_lcore(void)
{
    uint16_t nb_lcores = rte_lcore_count();
    //todo: get numa 0 core list;
    printf("\nb_lcores num: %d\n", nb_lcores);
    if (nb_lcores < 3) {
         return OGS_ERROR;
    } else if (nb_lcores < 7) {
        dkuf.pfcp_lcore = 0;
        dkuf.dpt_num = 1; 
        dkuf.dpt_lcore[0] = 1; 
        dkuf.fwd_num = 1; 
        dkuf.fwd_lcore[0] = 2;
    } else {
        dkuf.pfcp_lcore = 0;
        dkuf.dpt_num = 2; 
        dkuf.dpt_lcore[0] = 1; 
        dkuf.dpt_lcore[1] = 2; 
        dkuf.fwd_num = 4; 
        dkuf.fwd_lcore[0] = 3;
        dkuf.fwd_lcore[1] = 4;
        dkuf.fwd_lcore[2] = 5;
        dkuf.fwd_lcore[3] = 6;
    }
    return OGS_OK;
}

static int upf_dpdk_context_prepare(void)
{
    memset(&dkuf, 0, sizeof(dkuf));

    return OGS_OK;
}

static int upf_dpdk_context_validation(void)
{
    if (!dkuf.dpt_num || !dkuf.fwd_num) {
        ogs_error("lcore not set, dpt %d fwd %d, auto set it.\n", dkuf.dpt_num, dkuf.fwd_num);
        return auto_set_lcore();
    }

    return OGS_OK;
}

/* upf:
 *    dpdk:
 *        pfcp_lcore: 0
 *        dpt_lcore: 1
 *        fwd_lcore: 2,3
 *        n6_addr: 1.2.3.4/24
 *        n6_gw: 1.2.3.1
 *        n6_addr6: 2001::2/64
 *        n6_gw6: 2001::1
 */
int upf_dpdk_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    rv = upf_dpdk_context_prepare();
    if (rv != OGS_OK) return rv;
    
    printf("upf_dpdk_context_parse_config.\r\n");

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        if (strcmp(root_key, "upf")) {
            continue;
        }
        printf("upf_dpdk_context_parse_config,has upf.\r\n");
        ogs_yaml_iter_t upf_iter;
        ogs_yaml_iter_recurse(&root_iter, &upf_iter);
        while (ogs_yaml_iter_next(&upf_iter)) {
            if (strcmp(ogs_yaml_iter_key(&upf_iter), "dpdk")) {
                continue;
            }
            
            printf("upf_dpdk_context_parse_config,has dpdk.\r\n");
            dkuf.enable = 1;
            ogs_yaml_iter_t upf_array, dpdk_iter;
            ogs_yaml_iter_recurse(&upf_iter, &upf_array);
            do {
                if (ogs_yaml_iter_type(&upf_array) == YAML_MAPPING_NODE) {
                    memcpy(&dpdk_iter, &upf_array, sizeof(ogs_yaml_iter_t));
                } else if (ogs_yaml_iter_type(&upf_array) == YAML_SEQUENCE_NODE) {
                    if (!ogs_yaml_iter_next(&upf_array))
                        break;
                    ogs_yaml_iter_recurse(&upf_array, &dpdk_iter);
                } else if (ogs_yaml_iter_type(&upf_array) == YAML_SCALAR_NODE) {
                    break;
                } else {
                    ogs_assert_if_reached();
                }

                char *ipstr = NULL;

                while (ogs_yaml_iter_next(&dpdk_iter)) {
                    const char *dpdk_key = ogs_yaml_iter_key(&dpdk_iter);
                    ogs_assert(dpdk_key);
                    if (!strcmp(dpdk_key, "pfcp_lcore")) {
                        const char *v = ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            printf("get pfcp lcore %s\n", v);
                            dkuf.pfcp_lcore = atoi(v);
                        }
                    } else if (!strcmp(dpdk_key, "dpt_lcore")) {
                        const char *v = ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            printf("get dpt lcore %s\n", v);
                            parse_lcores(v, dkuf.dpt_lcore, &dkuf.dpt_num, MAX_DISPATCHER_NUM);
                        }
                    } else if (!strcmp(dpdk_key, "fwd_lcore")) {
                        const char *v = ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            printf("get fwd lcore %s\n", v);
                            parse_lcores(v, dkuf.fwd_lcore, &dkuf.fwd_num, MAX_FWD_NUM);
                        }
                    } else if (!strcmp(dpdk_key, "n6_addr")) {
                        char *v = (char *)ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            ipstr = (char *)strsep(&v, "/");
                            dkuf.n6_addr.ipv4 = inet_addr(ipstr);
                            if (v) {
                                dkuf.n6_addr.mask_bits = atoi(v);
                            } else {
                                dkuf.n6_addr.mask_bits = 32;
                            }

                            dkuf.n6_addr.mask = 0xFFFFFFFF;
                            ogs_info("mask:%s\n",ip2str(dkuf.n6_addr.mask));
                            dkuf.n6_addr.mask <<= (32 - dkuf.n6_addr.mask_bits);
                            dkuf.n6_addr.mask = ntohl(dkuf.n6_addr.mask);
                            ogs_info("get n6 address %s/%d\n", ip2str(dkuf.n6_addr.ipv4), dkuf.n6_addr.mask_bits);
                            ogs_info("get n6 mask:%s\n",ip2str(dkuf.n6_addr.mask));
                        }
                    } else if (!strcmp(dpdk_key, "n3_default_gw")) {
                        char *v = (char *)ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            dkuf.n3_addr.gw = inet_addr(v);
                            ogs_info("get n3 gateway %s\n", ip2str(dkuf.n3_addr.gw));
                        }
                    } else if (!strcmp(dpdk_key, "n3_default_gw6")) {
                        char *v = (char *)ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            inet_pton(AF_INET6, v, dkuf.n3_addr.gw6);
                            ogs_info("get n3 gateway6 %s\n", v);
                        }
                    } else if (!strcmp(dpdk_key, "n6_default_gw")) {
                        char *v = (char *)ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            dkuf.n6_addr.gw = inet_addr(v);
                            ogs_info("get n6 gateway %s\n", ip2str(dkuf.n6_addr.gw));
                        }
                    } else if (!strcmp(dpdk_key, "n6_addr6")) {
                        char *v = (char *)ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            ipstr = (char *)strsep(&v, "/");
                            inet_pton(AF_INET6, ipstr, dkuf.n6_addr.ipv6);
                            if (v) {
                                dkuf.n6_addr.mask6_bits = atoi(v);
                            } else {
                                dkuf.n6_addr.mask6_bits = 128;
                            }
                            ogs_info("get n6 address %s/%d\n", ipstr, dkuf.n6_addr.mask6_bits);
                        }
                    } else if (!strcmp(dpdk_key, "n6_default_gw6")) {
                        char *v = (char *)ogs_yaml_iter_value(&dpdk_iter);
                        if (v) {
                            inet_pton(AF_INET6, v, dkuf.n6_addr.gw6);
                            ogs_info("get n6 gateway6 %s\n", v);
                        }
                    } else {
                        ogs_warn("unknown key `%s`", dpdk_key);
                    }
                }
            } while (ogs_yaml_iter_type(&upf_array) == YAML_SEQUENCE_NODE);
        }
    }

    rv = upf_dpdk_context_validation();
    if (rv != OGS_OK) {
        return rv;
    }

    return OGS_OK;
}

static void sys_time_init(void)
{
    dkuf.ticks_per_sec = rte_get_tsc_hz();
    dkuf.ticks_per_ms = dkuf.ticks_per_sec / 1000;

    dkuf.sys_up_ms = rte_rdtsc() / dkuf.ticks_per_ms;
    dkuf.sys_up_sec = dkuf.sys_up_ms >> 10;
    dkuf.sec_diff = time(0) - dkuf.sys_up_sec;
    printf("ticks_per_sec %lu per_ms %lu up %lu ms %lu diff %lu\n",
            dkuf.ticks_per_sec, dkuf.ticks_per_ms,
            dkuf.sys_up_sec, dkuf.sys_up_ms, dkuf.sec_diff);
}

int upf_dpdk_init(void)
{
    eal_init();
    sys_time_init();

    uint16_t nb_ports = rte_eth_dev_count_avail();
    if (nb_ports != 2) {
        rte_exit(EXIT_FAILURE, "Ethernet ports %d err, must be 2.\n", nb_ports);
    }

    ring_init();
    mem_init();
    fwd_tbl_init();

    setup_ip_frag_tbl();
    set_n3_addr();
    dkuf.auto_send_garp = 1;

    uint16_t portid = 0;
    for (portid = 0; portid < nb_ports; portid++) {
        port_init(portid);
    }
    ogs_info("upf_dpdk_init sucess.");
    return OGS_OK;
}

int upf_dpdk_run(void);
int upf_dpdk_run(void)
{
    int i = 0;
    uint16_t lcore_id = 0;
    dkuf.stopFlag = 0;
    for (i = 0; i < dkuf.fwd_num; i++) {
        lcore_id = dkuf.fwd_lcore[i];
        rte_eal_remote_launch(fwd_main_loop, NULL, lcore_id);
    }
    for (i = 0; i < dkuf.dpt_num; i++) {
        lcore_id = dkuf.dpt_lcore[i];
        rte_eal_remote_launch(dpt_main_loop, NULL, lcore_id);
    }

    return OGS_OK;
}

int upf_dpdk_open(void)
{
    upf_dpdk_init();
    upf_dpdk_run();

    return OGS_OK;
}

int upf_dpdk_close(void)
{
    uint16_t portid;
    dkuf.stopFlag = 1;

    printf("\nWaiting for lcores to finish...\n");
	rte_eal_mp_wait_lcore();

    clear_ring();
    clear_fwd_tbl();
    clear_ip_frag_tbl();

    for (portid = 0; portid < 2; portid++) {
        printf("Closing port %d...", portid);
        rte_eth_dev_stop(portid);
        rte_eth_dev_close(portid);
        printf("port %d Done.", portid);
    }       

    free(dkuf.total_lcore_list);
    return OGS_OK;
} 

