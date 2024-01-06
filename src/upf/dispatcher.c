#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/if_ether.h>
#include <sys/prctl.h>
#include "dpdk-common.h"
#include "upf-dpdk.h"
#include <arpa/inet.h>

#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_icmp.h>
#include <rte_ip_frag.h>
#include "dpdk-arp-nd.h"
#include "ctrl-path.h"
#include "context.h"
#include "ogs-gtp.h"


struct rte_mbuf *
ipv4_reassemble(lcore_conf_t *lconf, struct rte_mbuf *m,
        struct rte_ether_hdr **eth_hdr_pp,
        struct rte_ipv4_hdr **ip_hdr_pp);
struct rte_mbuf *
ipv4_reassemble(lcore_conf_t *lconf, struct rte_mbuf *m,
        struct rte_ether_hdr **eth_hdr_pp,
        struct rte_ipv4_hdr **ip_hdr_pp)
{
    struct rte_ipv4_hdr *ip_hdr = *ip_hdr_pp;
    struct rte_ip_frag_tbl *tbl;
    struct rte_ip_frag_death_row *dr;

    struct rte_mbuf *mo;

    tbl = lconf->frag_tbl;
    dr = &lconf->death_row;

    mo = rte_ipv4_frag_reassemble_packet(tbl, dr, m, rte_rdtsc(), ip_hdr);
    if (mo == NULL) {
        /* no packet to send out. */
        return NULL;
    }

    /* we have our packet reassembled. */
    if (mo != m) {
        m = mo;
        *eth_hdr_pp = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
        *ip_hdr_pp = (struct rte_ipv4_hdr *)(*eth_hdr_pp + 1);
    }

    return m;
}

struct rte_mbuf *
ipv6_reassemble(lcore_conf_t *lconf, struct rte_mbuf *m,
        struct ipv6_extension_fragment *frag_hdr,
        struct rte_ether_hdr **eth_hdr_pp,
        struct rte_ipv6_hdr **ip_hdr_pp);
struct rte_mbuf *ipv6_reassemble(lcore_conf_t *lconf, struct rte_mbuf *m,
        struct ipv6_extension_fragment *frag_hdr,
        struct rte_ether_hdr **eth_hdr_pp,
        struct rte_ipv6_hdr **ip_hdr_pp)
{
    struct rte_ether_hdr *eth_hdr = *eth_hdr_pp;
    struct rte_ipv6_hdr *ip_hdr = *ip_hdr_pp;
    struct rte_ip_frag_tbl *tbl;
    struct rte_ip_frag_death_row *dr;

    struct rte_mbuf *mo;

    tbl = lconf->frag_tbl;
    dr = &lconf->death_row;

    /* prepare mbuf: setup l2_len/l3_len. */
    m->l2_len = sizeof(*eth_hdr);
    m->l3_len = sizeof(*ip_hdr) + sizeof(*frag_hdr);

     mo = rte_ipv6_frag_reassemble_packet(tbl, dr, m, rte_rdtsc(), ip_hdr, frag_hdr);
    if (mo == NULL) {
        return NULL;
    }

    if (mo != m) {
        m = mo;
        *eth_hdr_pp = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
        *ip_hdr_pp = (struct rte_ipv6_hdr *)(eth_hdr + 1);
    }

    return m;
}

int get_ipv6_proto(struct rte_ipv6_hdr *hdr, uint16_t *off);
int get_ipv6_proto(struct rte_ipv6_hdr *hdr, uint16_t *off)
{
    struct ext_hdr {
        uint8_t next_hdr;
        uint8_t len;
    };
    struct ext_hdr *xh;
    uint32_t i;
    uint8_t proto = hdr->proto;
    uint8_t *head = (uint8_t *)hdr;
    *off = 40;

#define MAX_EXT_HDRS 5
    for (i = 0; i < MAX_EXT_HDRS; i++) {
        switch (proto) {
            case IPPROTO_HOPOPTS:
            case IPPROTO_ROUTING:
            case IPPROTO_DSTOPTS:
                xh = (struct ext_hdr *)(head + *off);
                if (xh == NULL) {
                    return 0;
                }
                *off += (xh->len + 1) * 8;
                proto = xh->next_hdr;
                break;
            case IPPROTO_FRAGMENT:
                xh = (struct ext_hdr *)(head + *off);
                if (xh == NULL) {
                    return 0;
                }
                *off += 8;
                proto = xh->next_hdr;
                break; /* this is always the last ext hdr */
            case IPPROTO_NONE:
                break;
            default:
                return proto;
        }
    }
    return 0;
}

static int match_n3_addr(char *l3_head, uint8_t v4)
{
    ogs_socknode_t *node = NULL;

    ogs_list_for_each(&ogs_gtp_self()->gtpu_list, node) {
        if (v4 && node->addr->sin.sin_family == AF_INET) {
            if (ipv4_cmp(&node->addr->sin.sin_addr, l3_head + 16)) {
                return 0;
            }
        } else if (!v4 && node->addr->sin6.sin6_family == AF_INET6) {
            if (ipv6_cmp(&node->addr->sin6.sin6_addr, l3_head + 16)) {
                return 0;
            }
        }
    }

    return -1;
}

static int match_self_addr(uint32_t tip, uint16_t portid)
{
    ogs_socknode_t *node = NULL;

    if (portid) {
        if (ipv4_cmp(&dkuf.n6_addr.ipv4, &tip)) {
            return 0;
        }
    } else {
        ogs_list_for_each(&ogs_gtp_self()->gtpu_list, node) {
            if (node->addr->sin.sin_family == AF_INET) {
                if (ipv4_cmp(&node->addr->sin.sin_addr, &tip)) {
                    return 0;
                }
            }
        }
    }

    return -1;
}

static int match_self_addr6(void *tip, uint16_t portid)
{
    ogs_socknode_t *node = NULL;

    if (portid) {
        if (ipv6_cmp(dkuf.n6_addr.ipv6, tip)) {
            return 0;
        }
    } else {
        ogs_list_for_each(&ogs_gtp_self()->gtpu_list, node) {
            if (node->addr->sin6.sin6_family == AF_INET6) {
                if (ipv6_cmp(&node->addr->sin6.sin6_addr, tip)) {
                    return 0;
                }
            }
        }
    }

    return -1;
}

int gtpu_hdr_len(ogs_gtp2_header_t *gtp_h, int remain_len);
int gtpu_hdr_len(ogs_gtp2_header_t *gtp_h, int remain_len)
{
    uint8_t *ext_h = NULL;
    uint16_t len = 0;
    uint16_t ext_len = 0;

    len = OGS_GTPV1U_HEADER_LEN;
    if (remain_len < len) {
        return -1;
    }

    if (gtp_h->version != OGS_GTP2_VERSION_1) {
        ogs_error("[DROP] Invalid GTPU version [%d]", gtp_h->version);
        return -1;
    }

    if (gtp_h->flags & OGS_GTPU_FLAGS_E) {
        len += OGS_GTPV1U_EXTENSION_HEADER_LEN;
        if (remain_len < len) {
            return -1;
        }

        while (*(ext_h = (((uint8_t *)gtp_h) + len - 1))) {
            ext_len = (*(++ext_h)) * 4;
            if (!ext_len) {
                return -1;
            }
            len += ext_len;
            if (remain_len < len) {
                return -1;
            }
        }
    } else if (gtp_h->flags & (OGS_GTPU_FLAGS_S|OGS_GTPU_FLAGS_PN)) {
        len += 4;
        if (remain_len < len) {
            return -1;
        }
    }

    return len;
}

static inline int
dpt_decode_icmp6(struct lcore_conf *lconf,
        struct rte_mbuf *m, struct packet *pkt,
        struct mbuf_table *free, struct rte_ipv6_hdr *ipv6_h);
static inline int
dpt_decode_icmp6(struct lcore_conf *lconf,
        struct rte_mbuf *m, struct packet *pkt,
        struct mbuf_table *free, struct rte_ipv6_hdr *ipv6_h)
{
    int i = 0;
    struct rte_mbuf *new_m = NULL;

    if (match_self_addr6(ipv6_h->dst_addr, m->port) < 0) {
        free->m_table[free->cnt++] = m;
        return -1;
    }

    struct nd_msg *na = (struct nd_msg *)((char *)ipv6_h + pkt->l3_len);
    uint8_t type = na->icmph.icmp6_type;

    if (type == ICMP6_ECHO_REQUEST) {
        pkt->pkt_type = PKT_TYPE_PING6;
        return (*(uint32_t *)(ipv6_h->src_addr + 12) % dkuf.fwd_num);
    } else if (type == ND_NEIGHBOR_SOLICIT) {
        pkt->pkt_type = PKT_TYPE_ND;
        return (*(uint32_t *)(ipv6_h->src_addr + 12) % dkuf.fwd_num);
    } else if(type == ND_NEIGHBOR_ADVERT) {
        pkt->pkt_type = PKT_TYPE_ND;
        for (i = 0; i < dkuf.fwd_num; i++) {
            if (i != dkuf.fwd_num - 1) {
                new_m = packet_meta(packet_clone(pkt));
            } else {
                new_m = m;
            }
            if (-ENOBUFS == rte_ring_sp_enqueue(lconf->d2f_ring[i], new_m)) {
                rte_pktmbuf_free(new_m);
            }
        }
        return -1;
    } else {
        free->m_table[free->cnt++] = m;
        return -1;
    }
}

static inline int
dpt_decode_icmp(struct rte_mbuf *m, struct packet *pkt,
        struct mbuf_table *free, struct rte_ipv4_hdr *ipv4_h);
static inline int
dpt_decode_icmp(struct rte_mbuf *m, struct packet *pkt,
        struct mbuf_table *free, struct rte_ipv4_hdr *ipv4_h)
{
    struct rte_icmp_hdr *icmp_h = (struct rte_icmp_hdr *)((char *)ipv4_h + pkt->l3_len);

    // TODO: Test for N6 ping UE, will be removed when NAT implemented.
    if (m->port && match_self_addr(ipv4_h->dst_addr, m->port) != OGS_OK) {
        pkt->pkt_type = PKT_TYPE_IP_N6;
        return ntohl(ipv4_h->dst_addr) % dkuf.fwd_num;
    }

    // handle real imcp to N3 & N6 port
    // TODO: N3 & N6 might send icmp request to gnb or GW
    if (icmp_h->icmp_type != RTE_IP_ICMP_ECHO_REQUEST) {
        free->m_table[free->cnt++] = m;
        return -1;
    }

    if (match_self_addr(ipv4_h->dst_addr, m->port) < 0) {
        free->m_table[free->cnt++] = m;
        return -1;
    }

    pkt->pkt_type = PKT_TYPE_PING;
    return ntohl(ipv4_h->src_addr) % dkuf.fwd_num;
}

static inline int
dpt_decode_arp(struct lcore_conf *lconf,
        struct rte_mbuf *m, struct packet *pkt,
        struct mbuf_table *free, struct rte_arp_hdr *arp_h);
static inline int
dpt_decode_arp(struct lcore_conf *lconf,
        struct rte_mbuf *m, struct packet *pkt,
        struct mbuf_table *free, struct rte_arp_hdr *arp_h)
{
    int i = 0;
    struct rte_mbuf *new_m = NULL;

    if (match_self_addr(arp_h->arp_data.arp_tip, m->port) < 0) {
        free->m_table[free->cnt++] = m;
        return -1;
    }   

    pkt->pkt_type = PKT_TYPE_ARP;
    if (arp_h->arp_opcode == htons(RTE_ARP_OP_REQUEST)) {
        return ntohl(arp_h->arp_data.arp_sip) % dkuf.fwd_num;
    } else if (arp_h->arp_opcode == htons(RTE_ARP_OP_REPLY)) {
        for (i = 0; i < dkuf.fwd_num; i++) {
            if (i != dkuf.fwd_num - 1) {
                new_m = packet_meta(packet_clone(pkt));
            } else {
                new_m = m;
            }
            if (-ENOBUFS == rte_ring_sp_enqueue(lconf->d2f_ring[i], new_m)) {
                rte_pktmbuf_free(new_m);
            }
        }
        return -1;
    } else {
        free->m_table[free->cnt++] = m;
        return -1;
    }
}

static int dpt_handle_gtp_err_ind(struct lcore_conf *lconf, char *data, int remain_len)
{
#define MIN_ERR_IND_LEN 12
    if (remain_len < MIN_ERR_IND_LEN) {
        return -1;
    }
    upf_dpdk_event_t *event = dpdk_malloc(sizeof(upf_dpdk_event_t));
    if (!event) {
        ogs_error("Error allocate memory.");
        return -1;
    }
    char *pkt_data = dpdk_malloc(remain_len);
    if (!pkt_data) {
        ogs_error("Error allocate memory.");
        dpdk_free(event);
        return -1;
    }

    event->event_type = UPF_DPDK_SESS_REPORT;
    event->subtype = SESS_REPORT_ERR_IND;
    //maybe transfer rte_mbuf to pkbuf;
    memcpy(pkt_data, data, remain_len);
    event->event_body = (void *)pkt_data;
    if (-ENOBUFS == rte_ring_sp_enqueue(lconf->d2p_ring, event)) {
        lconf->lstat.enqueue_failed++;
        dpdk_free(pkt_data);
        dpdk_free(event);
        return -1;
    }

    return 0;
}

static int dpt_decap_pak(struct lcore_conf *lconf, struct rte_mbuf **mp, struct mbuf_table *free, uint16_t portid);
static int dpt_decap_pak(struct lcore_conf *lconf, struct rte_mbuf **mp, struct mbuf_table *free, uint16_t portid)
{
    struct rte_mbuf *m;
    m = *mp;
    int remain_len = 0;
    int len = m->data_len;
    uint16_t eth_type;
    struct packet *pkt = (struct packet *)(m->buf_addr);
    char *buf = rte_pktmbuf_mtod(m, char *);
    char *ptr = buf;
    int ring = -1;

    struct rte_ether_hdr *eth_h = (struct rte_ether_hdr *)ptr;

    if (m->port) {
        //n6 mac || multicast mac
        if (!(mac_cmp(ptr, (char *)&dkuf.mac[1]) || (ptr[0] & 0x01))) {
            free->m_table[free->cnt++] = m;
            return -1;
        }
    }

    pkt->pkt_type = PKT_TYPE_INIT;
    pkt->next = NULL;
    ptr = buf + MAC_ADDR_LEN;
    remain_len = len - MAC_ADDR_LEN;

    //using offload later;
    eth_type = *(uint16_t *)ptr;
    while (eth_type == BE_ETH_P_8021Q) {
        ptr += VLAN_HDR_LEN;
        remain_len -= VLAN_HDR_LEN;
        eth_type = *(uint16_t *)ptr;
    }
    ptr += ETHER_PROTO_LEN;
    remain_len -= ETHER_PROTO_LEN;
    pkt->l2_len = ptr - buf;
    m->l2_len = pkt->l2_len;

    uint16_t proto = 0;
    char *l3_head = ptr;
    struct rte_ipv4_hdr *ipv4_h;
    struct ipv6_extension_fragment *frag_hdr;
    struct rte_ipv6_hdr *ipv6_h;

    if (eth_type == BE_ETH_P_IP) {
        pkt->is_ipv4 = 1;
        ipv4_h = (struct rte_ipv4_hdr *)l3_head;
        pkt->l3_len = (ipv4_h->version_ihl & 0x0f) * 4;
        m->l3_len = pkt->l3_len;
        proto = ipv4_h->next_proto_id;

        if (!m->port && rte_ipv4_frag_pkt_is_fragmented(ipv4_h)) { //N6 do not reassemble
            m = ipv4_reassemble(lconf, m, &eth_h, &ipv4_h);
            if (!m) {
                return -1;
            }
            *mp = m;
            l3_head = (char *)ipv4_h;
            ptr = l3_head;
            pkt = (struct packet *)(m->buf_addr);
            remain_len = m->data_len - (ptr - rte_pktmbuf_mtod(m, char *));
        }
        if (IPPROTO_ICMP == proto) {
            return dpt_decode_icmp(m, pkt, free, ipv4_h);
        }
    } else if (eth_type == BE_ETH_P_IPV6) {
        pkt->is_ipv4 = 0;
        ipv6_h = (struct rte_ipv6_hdr *)l3_head;
        proto = get_ipv6_proto(ipv6_h, &pkt->l3_len);
        m->l3_len = pkt->l3_len;

        if (!m->port) { //N6 do not reassemble
            frag_hdr = rte_ipv6_frag_get_ipv6_fragment_header(ipv6_h);
            if (frag_hdr) {
                m = ipv6_reassemble(lconf, m, frag_hdr, &eth_h, &ipv6_h);
                if (!m) {
                    return -1;
                }
                *mp = m;
                pkt = (struct packet *)(m->buf_addr);
                l3_head = (char *)ipv6_h;
                ptr = l3_head;
                proto = get_ipv6_proto(ipv6_h, &pkt->l3_len);
                remain_len = m->data_len - (ptr - rte_pktmbuf_mtod(m, char *));
            }
        }
#define IPPROTO_ICMP6   58
        if (proto == IPPROTO_ICMP6) {
            return dpt_decode_icmp6(lconf, m, pkt, free, ipv6_h);
        }
    } else if (eth_type == BE_ETH_P_ARP) {
        return dpt_decode_arp(lconf, m, pkt, free, (struct rte_arp_hdr *)l3_head);
    } else {
        uint8_t is_lldp = 0;
        if (is_lldp) {
            //process later;
        } else {
            free->m_table[free->cnt++] = m;
            return -1;
        }
    }

    if (remain_len < pkt->l3_len) {
        free->m_table[free->cnt++] = m;
        return -1;
    }

    ptr += pkt->l3_len;
    remain_len -= pkt->l3_len;

    if (!portid) {
        if (((struct udphdr *)ptr)->dest != ntohs(OGS_GTPV1_U_UDP_PORT)) {
            free->m_table[free->cnt++] = m;
            return -1;
        }
        if (match_n3_addr(l3_head, pkt->is_ipv4) < 0) {
            free->m_table[free->cnt++] = m;
            return -1;
        }
        //add peer_list ?
        pkt->l4_len = UDP_HDR_LEN;
        ptr += pkt->l4_len;
        remain_len -= pkt->l4_len;

        ogs_gtp2_header_t *gtp_h = (ogs_gtp2_header_t *)ptr;
        int gtph_len = gtpu_hdr_len(gtp_h, remain_len);
        if (gtph_len < 0) {
            free->m_table[free->cnt++] = m;
            return -1;
        }
        pkt->tunnel_len = gtph_len;
        remain_len -= pkt->tunnel_len;
        ptr += pkt->tunnel_len;
        if (gtp_h->type == OGS_GTPU_MSGTYPE_ERR_IND) {
            if (dpt_handle_gtp_err_ind(lconf, ptr, remain_len) == 0) {
                lconf->lstat.sess_report[portid]++;
            }
            free->m_table[free->cnt++] = m;
            return -1;
        } else if (gtp_h->type == OGS_GTPU_MSGTYPE_GPDU) {
            uint8_t ip_version = (*ptr & 0xf0) >> 4;
            if (ip_version != 4 && ip_version != 6) {
                ogs_error("inner header ip version %d error\n", ip_version);
                free->m_table[free->cnt++] = m;
                return -1;
            } else if ((ip_version == 4 && remain_len < 20) || (ip_version == 6 && remain_len < 40)) {
                ogs_error("inner ip header len %d error\n", remain_len);
                free->m_table[free->cnt++] = m;
                return -1;
            }
        }
        pkt->pkt_type = PKT_TYPE_IP_N3;
        if (pkt->is_ipv4) {
            ring = ntohl((*(uint32_t *)(ptr + 12 + portid * 4))) % dkuf.fwd_num;
        } else {
            //ipv6 addr only used last 4 bytes;
            ring = (*(uint32_t *)(ptr + 20 + portid * 16)) % dkuf.fwd_num;
        }
    } else {
        pkt->pkt_type = PKT_TYPE_IP_N6;
        if (pkt->is_ipv4) {
            ring = ntohl((*(uint32_t *)(l3_head + 12 + portid * 4))) % dkuf.fwd_num;
        } else {
            ring = (*(uint32_t *)(l3_head + 20 + portid * 16)) % dkuf.fwd_num;
        }
    }

    return ring;
}

void dpt_pkts(struct lcore_conf *lconf, struct rte_mbuf **pkts, uint16_t nb_rx, uint16_t portid);
void dpt_pkts(struct lcore_conf *lconf, struct rte_mbuf **pkts, uint16_t nb_rx, uint16_t portid)
{
    struct mbuf_table free = {0};

    int ring = 0; 
    int i = 0;
    struct rte_mbuf *m;

    for (i = 0; i < nb_rx; i++) {
        m = pkts[i];
        m->port = portid;
        ring = dpt_decap_pak(lconf, &m, &free, portid);
        if (ring < 0) {
            lconf->lstat.rx_dropped[portid]++;
            continue;
        }
        if (-ENOBUFS == rte_ring_sp_enqueue(lconf->d2f_ring[ring], m)) {
            lconf->lstat.enqueue_failed++;
            free.m_table[free.cnt++] = m;
        } else {
            lconf->lstat.enqueue++;
        }
    }

    while (free.cnt-- > 0) {
        rte_pktmbuf_free(free.m_table[free.cnt]);
    }
    rte_ip_frag_free_death_row(&lconf->death_row, PREFETCH_OFFSET);
}

int dpt_main_loop(void *arg) 
{
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    uint16_t nb_rx, queueid;
    struct lcore_conf *lconf;

    unsigned lcore_id = rte_lcore_id();
    lconf = &dkuf.lconf[lcore_id];
    queueid = lconf->rx_queue;

    char thread_name[32];
    sprintf(thread_name, "dpt_%u_rxq_%d%c", lcore_id, lconf->rx_queue, '\0');
    prctl(PR_SET_NAME, thread_name);
    set_highest_priority();

    printf("%s lcoreid %u queueid %u\n", __func__, lcore_id, queueid);

    while (!dkuf.stopFlag) {
        nb_rx = rte_eth_rx_burst(0, queueid, pkts_burst, MAX_PKT_BURST);
        if (nb_rx) {
            //printf("lcore %d port 0 queue %d recv cnt %d\n", lcore_id, queueid, nb_rx);
            lconf->lstat.rx[0] += nb_rx;
            dpt_pkts(lconf, pkts_burst, nb_rx, 0);
        }

        nb_rx = rte_eth_rx_burst(1, queueid, pkts_burst, MAX_PKT_BURST);
        if (nb_rx) {
            //printf("lcore %d port 1 queue %d recv cnt %d\n", lcore_id, queueid, nb_rx);
            lconf->lstat.rx[1] += nb_rx;
            dpt_pkts(lconf, pkts_burst, nb_rx, 1);
        }
    }

    return 0;
}

