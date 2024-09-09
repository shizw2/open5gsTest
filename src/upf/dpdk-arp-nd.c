#include "core/abts.h"

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
#include <dpdk-common.h>
#include "dpdk-arp-nd.h"
#include <upf-dpdk.h>
#include <netinet/icmp6.h>
#include <ogs-gtp.h>
#include "dpdk-path.h"

struct rte_mbuf *
dkuf_alloc_arp_request(uint16_t portid, uint32_t dip)
{
    uint32_t sip = portid ? dkuf.n6_addr.ipv4 : dkuf.n3_addr.ipv4;
    struct rte_mempool *mp = dkuf.mpool;

    struct rte_mbuf *m;
    uint16_t pkt_len = sizeof(struct rte_ether_hdr) + sizeof(struct rte_arp_hdr);

    m = rte_pktmbuf_alloc(mp);
    if (!m) {
        return 0;
    }

    memcpy((char *)m->buf_addr + m->data_off, &dkuf.arp_req[portid], pkt_len);
    struct rte_ether_hdr *eth_h;
    struct rte_arp_hdr *arp_h;
     
    eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    arp_h = (struct rte_arp_hdr *)(eth_h + 1);
    arp_h->arp_data.arp_sip = sip;
    arp_h->arp_data.arp_tip = dip;
    //printf("%d,%d,set arp sip %s target ip %s\n",sip,dip,ip2str(sip), ip2str(dip));

    m->pkt_len = pkt_len;
    m->data_len = m->pkt_len;

    return m;
}

struct rte_mbuf *
dkuf_alloc_vxlan_arp_request(uint16_t portid, uint32_t sip,uint32_t dip)
{
    struct rte_mempool *mp = dkuf.mpool;

    struct rte_mbuf *m;
    uint16_t pkt_len = sizeof(struct rte_ether_hdr) + sizeof(struct rte_arp_hdr);

    m = rte_pktmbuf_alloc(mp);
    if (!m) {
        return 0;
    }

    memcpy((char *)m->buf_addr + m->data_off, &dkuf.arp_req[portid], pkt_len);
    struct rte_ether_hdr *eth_h;
    struct rte_arp_hdr *arp_h;
     
    eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    arp_h = (struct rte_arp_hdr *)(eth_h + 1);
    arp_h->arp_data.arp_sip = sip;
    arp_h->arp_data.arp_tip = dip;
    //printf("%d,%d,set arp sip %s target ip %s\n",sip,dip,ip2str(sip), ip2str(dip));

    m->pkt_len = pkt_len;
    m->data_len = m->pkt_len;

    return m;
}

struct rte_mbuf *
dkuf_alloc_ns(uint16_t portid, void *dip);
struct rte_mbuf *
dkuf_alloc_ns(uint16_t portid, void *dip)
{
    void *sip = portid ? dkuf.n6_addr.ipv6 : dkuf.n3_addr.ipv6;
    struct rte_mempool *mp = dkuf.mpool;

    struct rte_mbuf *m = rte_pktmbuf_alloc(mp);
    if (!m) {
        return 0;
    }

    char *ptr = (char *)m->buf_addr + m->data_off;

    struct rte_ether_hdr *eth_h = (struct rte_ether_hdr *)ptr;
    struct rte_ipv6_hdr *ipv6_h = (struct rte_ipv6_hdr *)(eth_h + 1);
    struct nd_msg *na = (struct nd_msg *)((char *)ipv6_h + 40);
     
    rte_eth_macaddr_get(portid, &eth_h->s_addr);
    ogs_info("port:%d, mac:%s.",portid,mac2str(&eth_h->s_addr));
    eth_h->ether_type = BE_ETH_P_IPV6;
     
    ipv6_h->vtc_flow = htonl(0x60000000);
    ipv6_h->payload_len = htons(32);
    ipv6_h->proto = IPPROTO_ICMPV6;
    ipv6_h->hop_limits = 255;
    memcpy(ipv6_h->src_addr, sip, 16);
    char multi_addr[16] = {0xFF, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0xFF, 0, 0, 0};
    memcpy(ipv6_h->dst_addr, multi_addr, 13);
    memcpy((char *)ipv6_h->dst_addr + 13, (char *)dip + 13, 3);

    *(uint16_t *)&eth_h->d_addr = 0x3333;
    *(uint32_t *)((char *)&eth_h->d_addr + 2) = *((uint32_t *)ipv6_h->dst_addr + 3);
    mac_print(&eth_h->d_addr);

    memset(na, 0, sizeof(*na) + 8);
    memcpy(&na->target, dip, 16);
    na->opt[0] = ND_OPT_SOURCE_LINKADDR;
    na->opt[1] = 1;
    mac_copy(&eth_h->s_addr, (struct ether_addr *)(na->opt + 2));
    na->icmph.icmp6_type = ND_NEIGHBOR_SOLICIT;
    na->icmph.icmp6_code = 0;

    m->pkt_len = 14 + 40 + 32;
    m->data_len = m->pkt_len;

    na->icmph.icmp6_cksum = ogs_in_cksum((uint16_t *)ipv6_h, 72);

    return m;
}

struct arp_hashtbl *arp_hash_create(uint32_t size)
{
    struct arp_hashtbl *h;
    uint32_t i, hsize;

    hsize = align32pow2(size);
    if (hsize < sizeof(size_t)) {
        hsize = sizeof(size_t);
    }

    h = malloc(sizeof(*h) + sizeof(*(h->htable)) * hsize);
    if (h == NULL) {
        printf("alloc hash table memory failed\n");
        return NULL;
    }

    h->num = 0;
    h->size = hsize;
    h->bitmask = h->size - 1;
    h->htable = (void *)((char *)h + sizeof(*h));
    if (!h->htable) {
        free(h);
        return NULL;
    }

    for (i = 0; i < h->size; i++) {
        h->htable[i] = NULL;
    }

    return h;
}

void arp_hash_destroy(struct arp_hashtbl *h)
{
    uint32_t i;
    arp_node_t *cur, *next;
    struct packet *next_pkt, *pkt;

    assert(h && h->htable);

    for (i = 0; i < h->size; i++) {
        cur = h->htable[i];
        while (cur) {
            next = cur->next;
            if (cur && cur->pkt_list) {
                pkt = (struct packet *)cur->pkt_list;
                while (pkt) {
                    next_pkt = pkt->next;
                    rte_pktmbuf_free(packet_meta(pkt));
                    pkt->next = NULL;
                    pkt = next_pkt;
                }
            }
            free(cur);
            cur = next;
        }
        h->htable[i] = NULL;
    }

    h->htable = NULL;

    free(h);

    return;
}

arp_node_t *arp_create(struct arp_hashtbl *arp_tbl, uint32_t ip, uint16_t port)
{
    arp_node_t *arp = calloc(1, sizeof(arp_node_t));
    arp->ip = ip;
    arp->port = port;
    arp->flag = ARP_ND_INIT;
    arp->up_sec = dkuf.sys_up_sec;

    arp_hash_insert(arp_tbl, arp);

    return arp;
}

arp_node_t *arp_find(struct lcore_conf *lconf, uint32_t ip, uint16_t port)
{
    arp_node_t *arp = arp_hash_find(lconf->arp_tbl, ip);

    if (!arp) {
        arp = arp_create(lconf->arp_tbl, ip, port);
        ogs_debug("create arp %s\n", ip2str(ip));
    }
    if (arp->flag == ARP_ND_INIT) {
        struct rte_mbuf *arp_m = dkuf_alloc_arp_request(port, ip);
        send_single_packet(lconf, port, arp_m);
        arp->flag = ARP_ND_SEND;
        arp_timer_start(lconf->twl, arp, 2);
    } else if (arp->flag == ARP_ND_SEND) {
        ogs_debug("arp waiting reply, %s\n", ip2str(ip));
    }

    return arp;
}

arp_node_t *arp_find_vxlan(struct lcore_conf *lconf, uint32_t ip, uint16_t port)
{
    arp_node_t *arp = arp_hash_find(lconf->arp_tbl, ip);

    if (!arp) {
        arp = arp_create(lconf->arp_tbl, ip, port);
        ogs_info("create vxlan arp %s\n", ip2str(ip));
    }
    if (arp->flag == ARP_ND_INIT) {
        //struct rte_mbuf *arp_m = dkuf_alloc_arp_request(port, ip);
        //send_single_packet(lconf, port, arp_m);//这里不能直接发送,需要封装vxlan头+gtp头后才能发送
        arp->flag = ARP_ND_VXLAN_SEND;
        arp_timer_start(lconf->twl, arp, 2);
    } else if (arp->flag == ARP_ND_VXLAN_SEND) {
        ogs_info("vxlan arp waiting reply, %s\n", ip2str(ip));
    }

    return arp;
}

void arp_delete(struct arp_hashtbl *arp_tbl, arp_node_t *arp)
{
    struct packet *next_pkt, *pkt;

    arp_hash_remove(arp_tbl, arp);
    if (arp && arp->pkt_list) {
        pkt = (struct packet *)arp->pkt_list;
        while (pkt) {
            next_pkt = pkt->next;
            rte_pktmbuf_free(packet_meta(pkt));
            pkt->next = NULL;
            pkt = next_pkt;
        }
    }

    free(arp);
}

void arp_timeout(void *arg)
{
    arp_node_t *arp = (arp_node_t *)arg;
    struct rte_mbuf *arp_m;
    struct lcore_conf *lconf = &dkuf.lconf[rte_lcore_id()];
    struct packet *pkt = NULL;
    ogs_debug("arp %p flag %d\n", arp, arp->flag);

    switch (arp->flag) {
    case ARP_ND_INIT:
    case ARP_ND_SEND:
        arp_m = dkuf_alloc_arp_request(arp->port, arp->ip);
        send_single_packet(lconf, arp->port, arp_m);
        arp_timer_start(lconf->twl, arp, ARP_ND_SEND_TIMEOUT);
        arp->flag = ARP_ND_TIMEOUT2;
        return ;
        
    case ARP_ND_OK:
        if ((dkuf.sys_up_sec - arp->up_sec) < (ARP_ND_AGEOUT - 2)) {
            ogs_info("arp %s isn't timeout\n", ip2str(arp->ip));
            arp_timer_start(lconf->twl, arp, ARP_ND_AGEOUT);
            return ;
        }
        ogs_info("arp %s timeout\n", ip2str(arp->ip));
        arp_m = dkuf_alloc_arp_request(arp->port, arp->ip);
        send_single_packet(lconf, arp->port, arp_m);
        arp_timer_start(lconf->twl, arp, ARP_ND_SEND_TIMEOUT1);
        arp->flag = ARP_ND_TIMEOUT1;
        return ;
    case ARP_ND_VXLAN_OK:
        //vxlan mac暂不进行超时重发处理。mac数不多。另外,如果mac变化,对方会主动发送arp。问题不大。
        if ((dkuf.sys_up_sec - arp->up_sec) < (10/*ARP_ND_AGEOUT*/ - 2)) {
            ogs_info("vxlan arp %s isn't timeout\n", ip2str(arp->ip));
            arp_timer_start(lconf->twl, arp, 10/*ARP_ND_AGEOUT*/);
            return ;
        }
        ogs_info("vxlan arp %s timeout\n", ip2str(arp->ip));
        arp_m = make_vxlan_arp_request(arp->ip,arp->remote_interface_ip,arp->local_tunnel_ip,arp->remote_tunnel_ip);
        
        pkt = (struct packet *)(arp_m->buf_addr);
        pkt->pkt_type = PKT_TYPE_ARP_VXLAN;
        ogs_info("make_vxlan_arp_request, pkt->l2_len:%d",pkt->l2_len);
        pkt->l2_len = 14;
        if (-ENOBUFS == rte_ring_sp_enqueue(dkuf.lconf[dkuf.fwd_lcore[0]].f2f_ring, arp_m)) {
            ogs_error("%s, to %s enqueue f2fring failed\n", __func__, ip2str(arp->ip));
            return ;
        }
        
        arp_timer_start(lconf->twl, arp, ARP_ND_SEND_TIMEOUT1);
        arp->flag = ARP_ND_VXLAN_TIMEOUT1;
        return ;
    case ARP_ND_TIMEOUT1:
        arp_m = dkuf_alloc_arp_request(arp->port, arp->ip);
        send_single_packet(lconf, arp->port, arp_m);
        arp_timer_start(lconf->twl, arp, ARP_ND_SEND_TIMEOUT2);
        arp->flag = ARP_ND_TIMEOUT2;
        return ;

    case ARP_ND_TIMEOUT2:
        arp_delete(lconf->arp_tbl, arp);
        return ;
    case ARP_ND_VXLAN_SEND:
        ogs_info("delete vxlan arp,%s",ip2str(arp->ip));
        arp_delete(lconf->arp_tbl, arp);
        return ;
    case ARP_ND_VXLAN_TIMEOUT1:
        arp_m = make_vxlan_arp_request(arp->ip,arp->remote_interface_ip,arp->local_tunnel_ip,arp->remote_tunnel_ip);
        
        pkt = (struct packet *)(arp_m->buf_addr);
        pkt->pkt_type = PKT_TYPE_ARP_VXLAN;
        if (-ENOBUFS == rte_ring_sp_enqueue(dkuf.lconf[dkuf.fwd_lcore[0]].f2f_ring, arp_m)) {
            ogs_error("%s, to %s enqueue f2fring failed\n", __func__, ip2str(arp->ip));
            return ;
        }
        arp_timer_start(lconf->twl, arp, ARP_ND_SEND_TIMEOUT2);
        arp->flag = ARP_ND_VXLAN_TIMEOUT2;
        return ;

    case ARP_ND_VXLAN_TIMEOUT2:
        arp_delete(lconf->arp_tbl, arp);
        return ;
    }

    return;
}

void arp_timer_start(struct timer_wheel *tw, arp_node_t *arp, uint32_t expire)
{
    uint32_t ms = expire * MS_PER_S;
    if (!tw_timer_start(tw, arp_timeout, arp, ms)) {
        ogs_error("start arp timer error\n");
    }
    ogs_debug("%p, expire %u sec\n", arp, expire);
}

struct nd_hashtbl *nd_hash_create(uint32_t size)
{
    struct nd_hashtbl *h;
    uint32_t i, hsize;

    hsize = align32pow2(size);
    if (hsize < sizeof(size_t)) {
        hsize = sizeof(size_t);
    }

    h = malloc(sizeof(*h) + sizeof(*(h->htable)) * hsize);
    if (h == NULL) {
        printf("alloc hash table memory failed\n");
        return NULL;
    }

    h->num = 0;
    h->size = hsize;
    h->bitmask = h->size - 1;
    h->htable = (void *)((char *)h + sizeof(*h));
    if (!h->htable) {
        free(h);
        return NULL;
    }

    for (i = 0; i < h->size; i++) {
        h->htable[i] = NULL;
    }

    return h;
}

void
nd_hash_destroy(struct nd_hashtbl *h)
{
    uint32_t i;
    nd_node_t *cur, *next;
    struct packet *next_pkt, *pkt;

    assert(h && h->htable);

    for (i = 0; i < h->size; i++) {
        cur = h->htable[i];
        while (cur) {
            next = cur->next;
            if (cur && cur->pkt_list) {
                pkt = (struct packet *)cur->pkt_list;
                while (pkt) {
                    next_pkt = pkt->next;
                    rte_pktmbuf_free(packet_meta(pkt));
                    pkt->next = NULL;
                    pkt = next_pkt;
                }
            }
            free(cur);
            cur = next;
        }
        h->htable[i] = NULL;
    }

    h->htable = NULL;

    free(h);

    return;
}
nd_node_t *nd_create(struct nd_hashtbl *nd_tbl, void *ip6, uint16_t port)
{
    nd_node_t *nd = calloc(1, sizeof(nd_node_t));

    memcpy(nd->ip6, ip6, 16);
    nd->port = port;
    nd->flag = ARP_ND_INIT;
    nd->up_sec = dkuf.sys_up_sec;

    nd_hash_insert(nd_tbl, nd);

    return nd;
}

nd_node_t *nd_find(struct lcore_conf *lconf, void *ip6, uint16_t port)
{
    uint32_t hash = ipv6_addr_hash((uint32_t *)ip6);
    nd_node_t *nd = nd_hash_find(lconf->nd_tbl, (uint64_t *)ip6, hash);
    if (!nd) {
        nd = nd_create(lconf->nd_tbl, ip6, port);
        ogs_debug("create nd %s\n", ip62str(ip6));
    }
    if (nd->flag == ARP_ND_INIT) {
        struct rte_mbuf *nd_m = dkuf_alloc_ns(port, nd->ip6);
        send_single_packet(lconf, port, nd_m);
        nd->flag = ARP_ND_SEND;
        nd_timer_start(lconf->twl, nd, 2);
    } else if (nd->flag == ARP_ND_SEND) {
        ogs_debug("nd waiting reply, %s\n", ip62str(ip6));
    }

    return nd;
}

void nd_delete(struct nd_hashtbl *nd_tbl, nd_node_t *nd)
{
    struct packet *next_pkt, *pkt;

    nd_hash_remove(nd_tbl, nd);
    if (nd && nd->pkt_list) {
        pkt = (struct packet *)nd->pkt_list;
        while (pkt) {
            next_pkt = pkt->next;
            rte_pktmbuf_free(packet_meta(pkt));
            pkt->next = NULL;
            pkt = next_pkt;
        }
    }

    free(nd);
}

void nd_timeout(void *arg)
{
    nd_node_t *nd = (nd_node_t *)arg;
    struct rte_mbuf *nd_m;
    struct lcore_conf *lconf = &dkuf.lconf[rte_lcore_id()];
    ogs_debug("nd %p flag %d\n", nd, nd->flag);

    switch (nd->flag) {
    case ARP_ND_INIT:
    case ARP_ND_SEND:
        nd_m = dkuf_alloc_ns(nd->port, nd->ip6);
        send_single_packet(lconf, nd->port, nd_m);
        nd_timer_start(lconf->twl, nd, ARP_ND_SEND_TIMEOUT);
        nd->flag = ARP_ND_TIMEOUT2;
        return ;
        
    case ARP_ND_OK:
        if ((dkuf.sys_up_sec - nd->up_sec) < (ARP_ND_AGEOUT - 2)) {
            ogs_debug("nd %s isn't timeout\n", ip62str(nd->ip6));
            nd_timer_start(lconf->twl, nd, ARP_ND_AGEOUT);
            return ;
        }
        nd_m = dkuf_alloc_ns(nd->port, nd->ip6);
        send_single_packet(lconf, nd->port, nd_m);
        nd_timer_start(lconf->twl, nd, ARP_ND_SEND_TIMEOUT1);
        nd->flag = ARP_ND_TIMEOUT1;
        return ;

    case ARP_ND_TIMEOUT1:
        nd_m = dkuf_alloc_ns(nd->port, nd->ip6);
        send_single_packet(lconf, nd->port, nd_m);
        nd_timer_start(lconf->twl, nd, ARP_ND_SEND_TIMEOUT2);
        nd->flag = ARP_ND_TIMEOUT2;
        return ;

    case ARP_ND_TIMEOUT2:
        nd_delete(lconf->nd_tbl, nd);
        return ;
    }

    return;
}

void nd_timer_start(struct timer_wheel *tw, nd_node_t *nd, uint32_t expire)
{
    uint32_t ms = expire * MS_PER_S;
    if (!tw_timer_start(tw, nd_timeout, nd, ms)) {
        ogs_error("start nd timer error\n");
    }
    ogs_debug("%p, expire %u sec\n", nd, expire);
}

