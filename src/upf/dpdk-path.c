#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include <upf-dpdk.h>
#include <dpdk-path.h>
#include <ctrl-path.h>
#include <dpdk-common.h>
#include <dpdk-arp-nd.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <rte_ip.h>
#include <rte_udp.h>
#include <rte_tcp.h>
#include <rte_icmp.h>


extern int get_ipv6_proto(struct rte_ipv6_hdr *hdr, uint16_t *off);

uint8_t ue_addr_match(uint32_t ip)
{
    ogs_pfcp_subnet_t *subnet = NULL;

    ogs_list_for_each(&ogs_pfcp_self()->subnet_list, subnet) {
        if (subnet->family != AF_INET) {
            continue;
        }
        if (subnet->sub.sub[0] == (ip & subnet->sub.mask[0])) {
            return 1;
        }
    }

    return 0;
}

uint8_t ue_addr6_match(void *ip)
{
    uint32_t *p = (uint32_t *)ip;
    ogs_pfcp_subnet_t *subnet = NULL;

    ogs_list_for_each(&ogs_pfcp_self()->subnet_list, subnet) {
        if (subnet->family != AF_INET6) {
            continue;
        }
        if (subnet->sub.sub[0] == (p[0] & subnet->sub.mask[0]) &&
                subnet->sub.sub[1] == (p[1] & subnet->sub.mask[1]) &&
                subnet->sub.sub[2] == (p[2] & subnet->sub.mask[2]) &&
                subnet->sub.sub[3] == (p[3] & subnet->sub.mask[3])) {
            return 1;
        }
    }

    return 0;
}

void ip_fragmentation(lcore_conf_t *lconf, struct rte_mbuf *m, uint32_t mtu, uint16_t tx_port, uint8_t is_ipv4)
{
    struct rte_ether_hdr *origin_eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    struct rte_mbuf *new_m;
    struct rte_mbuf *frag_m[MAX_PACKET_FRAG];
    uint16_t cnt;
    int new_mbufs;
    struct rte_mempool *direct_pool = dkuf.direct_pool;
    struct rte_mempool *indirect_pool = dkuf.indirect_pool;

    cnt = lconf->tx_mbufs[tx_port].cnt;
    ogs_debug("ip fragmentation %d\n", m->pkt_len);

    rte_pktmbuf_adj(m, (uint16_t)sizeof(struct rte_ether_hdr));

    struct rte_ether_hdr *eth_hdr = NULL;
    struct rte_ipv4_hdr *ipv4_h = NULL;

    if (is_ipv4) {
        ipv4_h = rte_pktmbuf_mtod(m, struct rte_ipv4_hdr *);
        ipv4_h->fragment_offset &= ~htons(RTE_IPV4_HDR_DF_FLAG);
        new_mbufs = rte_ipv4_fragment_packet(m, frag_m,
                MAX_PACKET_FRAG, mtu, direct_pool, indirect_pool);
    } else {
        new_mbufs = rte_ipv6_fragment_packet(m, frag_m,
                MAX_PACKET_FRAG, mtu, direct_pool, indirect_pool);
    }
    if (unlikely(new_mbufs < 0)) {
        ogs_error("ip fragmentation failed %d, m %p data_len %u pkt_len %u\n",
                new_mbufs, m, m->data_len, m->pkt_len);
        rte_pktmbuf_free(m);
        return ;
    }

    ogs_debug("pkt splits to %d fragments.\n", new_mbufs);

    int i = 0;
    for (i = 0; i < new_mbufs; i++) {
        new_m = frag_m[i];
        eth_hdr = (struct rte_ether_hdr *)rte_pktmbuf_prepend(new_m, sizeof(struct rte_ether_hdr));
        rte_memcpy(eth_hdr, origin_eth_h, sizeof(struct rte_ether_hdr));
        new_m->l2_len = sizeof(struct rte_ether_hdr);

        if (is_ipv4) {
            ipv4_h = (struct rte_ipv4_hdr *)(eth_hdr + 1);
            new_m->ol_flags &= (~(PKT_TX_IPV4 | PKT_TX_IP_CKSUM));
            ipv4_h->hdr_checksum = 0;
            if (dkuf.tx_offloads[tx_port] & DEV_TX_OFFLOAD_IPV4_CKSUM) {
                new_m->l3_len = (ipv4_h->version_ihl & 0x0f) * 4;
                new_m->ol_flags |= (PKT_TX_IPV4 | PKT_TX_IP_CKSUM);
            } else {
                new_m->ol_flags &= (~(PKT_TX_IPV4 | PKT_TX_IP_CKSUM));
                ipv4_h->hdr_checksum = rte_ipv4_cksum(ipv4_h);
            }
        }
        lconf->tx_mbufs[tx_port].m_table[cnt + i] = new_m;
    }

    lconf->tx_mbufs[tx_port].cnt += new_mbufs;
    if (likely(lconf->tx_mbufs[tx_port].cnt >= MAX_PKT_BURST)) {
        port_send_burst(lconf, tx_port);
    }

    rte_pktmbuf_free(m);

    return;
}

struct rte_mbuf *gtp_handle_echo_req(struct rte_mbuf *m, ogs_gtp2_header_t *gtph)
{
    struct packet *pkt = (struct packet *)(m->buf_addr);

    struct rte_ether_hdr *eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    struct rte_ipv4_hdr *ipv4_h = (struct rte_ipv4_hdr *)((char *)eth_h + pkt->l2_len);
    struct rte_udp_hdr *udp_h = (struct rte_udp_hdr *)((char *)ipv4_h + pkt->l3_len);
    uint16_t length = 0;
    int idx = 0;

    if ((gtph->flags >> 5) != 1) {
        return NULL;
    }

    gtph->flags = (1 << 5); /* set version */
    gtph->flags |= (1 << 4); /* set PT */
    gtph->type = OGS_GTPU_MSGTYPE_ECHO_RSP;
    gtph->length = 0;
    gtph->teid = 0;
    idx = 8;

    if (gtph->flags & (OGS_GTPU_FLAGS_PN | OGS_GTPU_FLAGS_S)) {
        length += 4;
        if ((gtph->flags & OGS_GTPU_FLAGS_S) == 0) {
            *((uint8_t *)gtph + idx) = 0;
            *((uint8_t *)gtph + idx + 1) = 0;
        }
        idx += 2;
        if ((gtph->flags & OGS_GTPU_FLAGS_PN) == 0) {
            *((uint8_t *)gtph + idx) = 0;
        }
        idx++;
        *((uint8_t *)gtph + idx) = 0; /* next-extension header */
        idx++;
    }

    length += 2;
    *((uint8_t *)gtph + idx) = 14; idx++; /* type */
    *((uint8_t *)gtph + idx) = 0; idx++; /* restart counter */

    gtph->length = htobe16(length);

    SWAP(udp_h->src_port, udp_h->dst_port);
    int l4_len = length + sizeof(*gtph) + 8;
    udp_h->dgram_len = rte_cpu_to_be_16(l4_len);
    udp_h->dgram_cksum = 0;   /* No UDP checksum. */

    SWAP(ipv4_h->src_addr, ipv4_h->dst_addr);
    ipv4_h->total_length = rte_cpu_to_be_16(pkt->l3_len + l4_len);
    ipv4_h->hdr_checksum = 0;
    ipv4_h->hdr_checksum = rte_ipv4_cksum(ipv4_h);

    mac_swap(&eth_h->s_addr, &eth_h->d_addr);
    m->data_len = m->pkt_len = pkt->l2_len + pkt->l3_len + l4_len;

    return m;
}

upf_sess_t *local_sess_find_by_ue_ip(struct lcore_conf *lconf, char *l3_head, uint8_t dst)
{
    upf_sess_t *sess = NULL;

    uint8_t off = 0;

    struct ip *ip_h = NULL;

    ip_h = (struct ip *)l3_head;
    if (ip_h->ip_v == 4) {
        off = dst ? 16 : 12;
        sess = ipv4_sess_find(lconf->ipv4_hash, *(uint32_t *)(l3_head + off));
        if (!sess || !sess->ipv4) {
            struct upf_route_trie_node *trie = lconf->ipv4_framed_routes;
            uint32_t addr = *(uint32_t *)(l3_head + off);
            const int nbits = sizeof(addr) << 3;
            int i;
            for (i =  0; i <= nbits; i++) {
                int bit = nbits - i - 1;

                if (!trie)
                    break;
                if (trie->sess){
                    sess = trie->sess;
                    ogs_info("framed_routes find sess, ip:%s", ip2str(addr));   
                }
                if (i == nbits)
                    break;

                if ((1 << bit) & be32toh(addr))
                    trie = trie->right;
                else
                    trie = trie->left;
            }
            
            if (!sess || !sess->ipv4){
                return NULL;
            }
        }
        ogs_debug("PAA IPv4:%s", ip2str(sess->ipv4->addr[0]));
    } else if (ip_h->ip_v == 6) {
        off = dst ? 24 : 8;
        sess = (upf_sess_t *)ogs_hash_get(lconf->ipv6_hash,
                l3_head + off, OGS_IPV6_DEFAULT_PREFIX_LEN >> 3);
        if (!sess || !sess->ipv6) {
            return NULL;
        }
        ogs_debug("PAA IPv6:%s", ip62str(sess->ipv6->addr));
    } else {
        ogs_error("Invalid packet [IP version:%d]", ip_h->ip_v);
    }

    return sess;
}

int add_vxlan_header(upf_sess_t *sess, struct rte_mbuf *m)
{
    // 计算新的封装所需的总长度
    uint16_t new_encap_len = sizeof(struct new_eth_header);

    char *ptr = rte_pktmbuf_mtod(m, char *);
    struct packet *pkt = (struct packet *)(m->buf_addr);

    // // 更新mbuf数据指针和长度以容纳新的封装
    // char *pkt = rte_pktmbuf_prepend(m, new_encap_len);
    // if (ptr == NULL) {
    //     // 处理错误：无法扩展mbuf
    //     return -1;
    // }

    // 填充新的以太网头部
    struct new_eth_header *new_encap = (struct new_eth_header *)(ptr + pkt->l2_len - new_encap_len);

    // 填充新的以太网MAC头部
    mac_copy((struct rte_ether_addr *)&dkuf.mac[0], &new_encap->ether.s_addr);

    arp_node_t *arp = NULL;
    //查询interface的MAC
    struct lcore_conf *lconf = &dkuf.lconf[rte_lcore_id()];
    arp = arp_find(lconf, sess->remote_vxlan_interface, 0);//TODO:ip地址

    if (arp->flag == ARP_ND_SEND) {
        if (arp->pkt_list_cnt < MAX_PKT_BURST) {
            pkt->next = arp->pkt_list;
            arp->pkt_list = pkt;
            arp->pkt_list_cnt++;
            return 0;
        } else {
            lconf->lstat.tx_dropped[0]++;
            return -1;
        }
    }
    mac_copy(arp->mac, &new_encap->ether.d_addr);

    new_encap->ether.ether_type = BE_ETH_P_IP; 

    
    // 填充VXLAN头部
    new_encap->vxlan.flags = 0x08; // 8位标志位，设置I和F标志为0
    new_encap->vxlan.rsvd[0] = 0;
    new_encap->vxlan.rsvd[1] = 0;
    new_encap->vxlan.rsvd[2] = 0;
    new_encap->vxlan.vni = rte_cpu_to_be_32(100 <<8);

   
    // 填充新的UDP头部
    new_encap->udp.src_port = htons(4789); // 新的源UDP端口
    new_encap->udp.dst_port = htons(4789); // 新的目的UDP端口
    new_encap->udp.dgram_len = rte_cpu_to_be_16(m->pkt_len - pkt->l2_len + new_encap_len - IP_HDR_LEN);
    new_encap->udp.dgram_cksum = 0; // VXLAN封装中UDP校验和通常为0

    // 填充新的IP头部
    new_encap->ip.version_ihl = 0x45; // IPv4版本4，IHL为5*4字节
    new_encap->ip.type_of_service = 0;
    new_encap->ip.packet_id = 0; // 可以设置为0或使用其他逻辑生成
    new_encap->ip.fragment_offset = 0;
    new_encap->ip.time_to_live = IPDEFTTL;
    new_encap->ip.next_proto_id = IPPROTO_UDP;
    new_encap->ip.src_addr = sess->ipv4->subnet->gw.sub[0]; // TODO 新的源IP地址
    new_encap->ip.dst_addr = sess->ipv4->addr; // TODO 新的目的IP地址
    new_encap->ip.total_length = rte_cpu_to_be_16(m->pkt_len - pkt->l2_len + new_encap_len);

    // 计算IP校验和
    new_encap->ip.hdr_checksum = rte_ipv4_cksum(&new_encap->ip);

    return new_encap_len;
}
int support_vxlan = 1;
int gtp_send_user_plane(
        ogs_gtp_node_t *gnode, ogs_gtp2_header_t *gtp_hdesc,
        ogs_gtp2_extension_header_t *ext_hdesc, struct rte_mbuf *m)
{
    ogs_gtp2_header_t *gtp_h = NULL;
    ogs_gtp2_extension_header_t *ext_h = NULL;
    uint8_t flags;
    uint8_t gtp_hlen = 0;
    int i = 0;


    flags = gtp_hdesc->flags;
    flags |= OGS_GTPU_FLAGS_V | OGS_GTPU_FLAGS_PT;
    if (ext_hdesc->array[0].type && ext_hdesc->array[0].len)
        flags |= OGS_GTPU_FLAGS_E;

    /* Define GTP Header Size */
    if (flags & OGS_GTPU_FLAGS_E) {

        gtp_hlen = OGS_GTPV1U_HEADER_LEN+OGS_GTPV1U_EXTENSION_HEADER_LEN;

        i = 0;
        while(ext_hdesc->array[i].len) {
            gtp_hlen += (ext_hdesc->array[i].len*4);
            i++;
        }

    } else if (flags & (OGS_GTPU_FLAGS_S|OGS_GTPU_FLAGS_PN))
        gtp_hlen = OGS_GTPV1U_HEADER_LEN+OGS_GTPV1U_EXTENSION_HEADER_LEN;
    else
        gtp_hlen = OGS_GTPV1U_HEADER_LEN;

    char *ptr = rte_pktmbuf_mtod(m, char *);
    struct packet *pkt = (struct packet *)(m->buf_addr);

    gtp_h = (ogs_gtp2_header_t *)(ptr + pkt->l2_len - gtp_hlen - pkt->vxlan_len);
    memset(gtp_h, 0, gtp_hlen);

    gtp_h->flags = flags;
    gtp_h->type = gtp_hdesc->type;
    gtp_h->teid = htobe32(gtp_hdesc->teid);
    gtp_h->length = htobe16(m->pkt_len - pkt->l2_len + gtp_hlen - OGS_GTPV1U_HEADER_LEN + pkt->vxlan_len);

    /*if (gtp_h->flags & OGS_GTPU_FLAGS_E) {
        ext_h = (ogs_gtp2_extension_header_t *)((char *)gtp_h + OGS_GTPV1U_HEADER_LEN);
        ext_h->type = OGS_GTP2_EXTENSION_HEADER_TYPE_PDU_SESSION_CONTAINER;
        ext_h->len = 1;
        ext_h->pdu_type = ext_hdesc->pdu_type;
        ext_h->qos_flow_identifier = ext_hdesc->qos_flow_identifier;
        ext_h->next_type = OGS_GTP2_EXTENSION_HEADER_TYPE_NO_MORE_EXTENSION_HEADERS;
    }*/

    /* Fill Extention Header */
    if (gtp_h->flags & OGS_GTPU_FLAGS_E) {
        uint8_t *ext_h = (uint8_t *)((char*)gtp_h + OGS_GTPV1U_HEADER_LEN + OGS_GTPV1U_EXTENSION_HEADER_LEN);
        ogs_assert(ext_h);

        /* Copy Header Type */
        *(ext_h-1) = ext_hdesc->array[0].type;

        i = 0;
        while (i < OGS_GTP2_NUM_OF_EXTENSION_HEADER &&
                (ext_h - (uint8_t *)gtp_h) < gtp_hlen) {
            int len = ext_hdesc->array[i].len*4;

            /* Copy Header Content */
            memcpy(ext_h, &ext_hdesc->array[i].len, len-1);

            /* Check if Next Header is Available */
            if (ext_hdesc->array[i+1].len)
                ext_h[len-1] = ext_hdesc->array[i+1].type;
            else
                ext_h[len-1] =
                    OGS_GTP2_EXTENSION_HEADER_TYPE_NO_MORE_EXTENSION_HEADERS;

            ext_h += len;
            i++;
        }
    }

    struct rte_udp_hdr *udp_h = (struct rte_udp_hdr *)((char *)gtp_h - UDP_HDR_LEN);

    udp_h->src_port = htons(OGS_GTPV1_U_UDP_PORT);
    udp_h->dst_port = htons(OGS_GTPV1_U_UDP_PORT);
    int l4_len = m->pkt_len - pkt->l2_len + gtp_hlen + UDP_HDR_LEN + pkt->vxlan_len;

    udp_h->dgram_len = rte_cpu_to_be_16(l4_len);
    udp_h->dgram_cksum = 0;

    struct rte_ether_hdr *eth_h = NULL;
    struct rte_ipv4_hdr *ipv4_h = NULL;
    struct rte_ipv6_hdr *ipv6_h = NULL;
    uint8_t is_ipv4 = 0;

    if (UNLIKELY(gnode->addr.ogs_sa_family == AF_INET6)) {
        udp_h->dgram_cksum = rte_ipv6_udptcp_cksum(ipv6_h, udp_h); //v6 must set udp cksum;
        ipv6_h = (struct rte_ipv6_hdr *)((char *)udp_h - IP6_HDR_LEN);
        eth_h = (struct rte_ether_hdr *)((char *)ipv6_h - L2_HDR_LEN);
        pkt->l3_len = IP6_HDR_LEN;
        ipv6_h->vtc_flow = htonl(0x60000000);
        ipv6_h->payload_len = rte_cpu_to_be_16(l4_len);
        ipv6_h->proto = IPPROTO_UDP;
        ipv6_h->hop_limits = IPDEFTTL - 1;
        memcpy(ipv6_h->src_addr, dkuf.n3_addr.ipv6, 16);
        memcpy(ipv6_h->dst_addr, &gnode->addr.sin6.sin6_addr, 16);
        eth_h->ether_type = BE_ETH_P_IPV6;
    } else {
        //  outer l3 id
        static uint16_t pkt_id = 100;

        ipv4_h = (struct rte_ipv4_hdr *)((char *)udp_h - IP_HDR_LEN);
        eth_h = (struct rte_ether_hdr *)((char *)ipv4_h - L2_HDR_LEN);
        pkt->l3_len = IP_HDR_LEN;

        ipv4_h->version_ihl = IP_VHL_DEF;
        ipv4_h->type_of_service = 0;
        ipv4_h->packet_id = pkt_id++;
        ipv4_h->fragment_offset = 0; /* [15,14,13] bits for flags, [12 - 0] bits for offset */
        ipv4_h->time_to_live = IPDEFTTL - 1;
        ipv4_h->next_proto_id = IPPROTO_UDP;
        ipv4_h->src_addr = dkuf.n3_addr.ipv4;
        ipv4_h->dst_addr = gnode->addr.sin.sin_addr.s_addr;
        ipv4_h->total_length = rte_cpu_to_be_16(IP_HDR_LEN + l4_len);
        ipv4_h->hdr_checksum = 0;
        if (dkuf.tx_offloads[0] & DEV_TX_OFFLOAD_IPV4_CKSUM) {
            m->l3_len = IP_HDR_LEN;  //set l2_len & used m->l2_len/l3_len later;
            m->ol_flags |= (PKT_TX_IPV4 | PKT_TX_IP_CKSUM);
        } else {
            m->ol_flags &= (~(PKT_TX_IPV4 | PKT_TX_IP_CKSUM));
            ipv4_h->hdr_checksum = rte_ipv4_cksum(ipv4_h);
        }
        eth_h->ether_type = BE_ETH_P_IP;
        is_ipv4 = 1;
    }

    mac_copy((struct rte_ether_addr *)&dkuf.mac[0], &eth_h->s_addr);

    int increase = pkt->l3_len + UDP_HDR_LEN + gtp_hlen + pkt->vxlan_len;
    m->data_len += increase;
    m->pkt_len += increase;
    m->data_off -= increase;

    struct lcore_conf *lconf = &dkuf.lconf[rte_lcore_id()];
    if (LIKELY(is_ipv4)) {
        //arp_node_t *arp = arp_find(lconf, ipv4_h->dst_addr, 0);
        arp_node_t *arp = NULL;
        //如果目的IP跟N3在同一网段,则直接查询目的IP的MAC,否则查询GW的MAC
        ogs_debug("dstaddr:%s, n3addr:%s, gw:%s,mask:%s", ip2str(ipv4_h->dst_addr),ip2str(dkuf.n3_addr.ipv4),ip2str(dkuf.n3_addr.gw),ip2str(dkuf.n3_addr.mask));
        if ((ipv4_h->dst_addr&dkuf.n3_addr.mask ) == (dkuf.n3_addr.ipv4 & dkuf.n3_addr.mask)){
            arp = arp_find(lconf, ipv4_h->dst_addr, 0);
        }else{
            arp = arp_find(lconf, dkuf.n3_addr.gw, 0);
        }

        if (arp->flag == ARP_ND_SEND) {
            if (arp->pkt_list_cnt < MAX_PKT_BURST) {
                pkt->next = arp->pkt_list;
                arp->pkt_list = pkt;
                arp->pkt_list_cnt++;
                return 0;
            } else {
                lconf->lstat.tx_dropped[0]++;
                return -1;
            }
        }
        mac_copy(arp->mac, &eth_h->d_addr);
    } else {
        //nd_node_t *nd = nd_find(lconf, ipv6_h->dst_addr, 0);
        nd_node_t *nd = NULL;
        if ((ipv6_h->dst_addr[0]&dkuf.n3_addr.mask6[0] ) == (dkuf.n3_addr.ipv6[0] & dkuf.n3_addr.mask6[0]) && 
            (ipv6_h->dst_addr[1]&dkuf.n3_addr.mask6[1] ) == (dkuf.n3_addr.ipv6[1] & dkuf.n3_addr.mask6[1])){
            nd = nd_find(lconf, ipv6_h->dst_addr, 0);
        }else{
            nd = nd_find(lconf, dkuf.n3_addr.gw6, 0);
        }
        if (nd->flag == ARP_ND_SEND) {
            if (nd->pkt_list_cnt < MAX_PKT_BURST) {
                pkt->next = nd->pkt_list;
                nd->pkt_list = pkt;
                nd->pkt_list_cnt++;
                return 0;
            } else {
                lconf->lstat.tx_dropped[0]++;
                return -1;
            }
        }
        mac_copy(nd->mac, &eth_h->d_addr);
    }

    send_packet(lconf, 0, m, is_ipv4);

    return 0;
}

int pfcp_send_g_pdu(ogs_pfcp_pdr_t *pdr, struct rte_mbuf *m)
{
    ogs_gtp_node_t *gnode = NULL;
    ogs_pfcp_far_t *far = NULL;

    ogs_gtp2_header_t gtp_hdesc;
    ogs_gtp2_extension_header_t ext_hdesc;

    far = pdr->far;
    if (!far || !far->gnode) {
        ogs_error("No FAR");
        return -1;
    }

    if (far->dst_if == OGS_PFCP_INTERFACE_UNKNOWN) {
        ogs_error("No Destination Interface");
        return -1;
    }

    gnode = far->gnode;

    memset(&gtp_hdesc, 0, sizeof(gtp_hdesc));
    memset(&ext_hdesc, 0, sizeof(ext_hdesc));

    gtp_hdesc.type = OGS_GTPU_MSGTYPE_GPDU;
    gtp_hdesc.teid = far->outer_header_creation.teid;
    if (pdr->qer && pdr->qer->qfi) {
        //ext_hdesc.qos_flow_identifier = pdr->qer->qfi;
        
        ext_hdesc.array[0].type =
            OGS_GTP2_EXTENSION_HEADER_TYPE_PDU_SESSION_CONTAINER;
        ext_hdesc.array[0].len = 1;
        ext_hdesc.array[0].pdu_type = OGS_GTP2_EXTENSION_HEADER_PDU_TYPE_DL_PDU_SESSION_INFORMATION;
        ext_hdesc.array[0].qos_flow_identifier = pdr->qer->qfi;
    }

    return gtp_send_user_plane(gnode, &gtp_hdesc, &ext_hdesc, m);
}

int pfcp_up_handle_pdr(ogs_pfcp_pdr_t *pdr, struct rte_mbuf *m, uint8_t *downlink_data_report)
{
    ogs_pfcp_far_t *far = NULL;

    far = pdr->far;

    if (far->gnode) {
        if (far->apply_action & OGS_PFCP_APPLY_ACTION_FORW) {
            return pfcp_send_g_pdu(pdr, m);
        } else if (!(far->apply_action & OGS_PFCP_APPLY_ACTION_BUFF)) {
            return -1;
        }
    }

    // TODO: BUG on fwd_flush_buffered_packet() did not flush bufferd pkts, because old far was not OGS_PFCP_APPLY_ACTION_FORW
    if (far->num_of_buffered_packet == 0) {
        /* Only the first time a packet is buffered, it reports downlink notifications. */
        *downlink_data_report = 1;
    }
    if (far->num_of_buffered_packet < OGS_MAX_NUM_OF_PACKET_BUFFER) {
        far->buffered_packet[far->num_of_buffered_packet++] = (ogs_pkbuf_t *)m;
    } else {
        ogs_error("No enough buffer space, free pkt\n");
        return -1;
    }

    return 0;
}

ogs_pfcp_rule_t *pfcp_pdr_rule_find_by_packet(ogs_pfcp_pdr_t *pdr, char *l3_head)
{
    struct ip *ip_h =  NULL;
    struct ip6_hdr *ip6_h =  NULL;
    uint32_t *src_addr = NULL;
    uint32_t *dst_addr = NULL;
    uint16_t sport = 0;
    uint16_t dport = 0;
    int addr_len = 0;
    uint8_t proto = 0;
    uint16_t ip_hlen = 0;
    ogs_pfcp_rule_t *rule = NULL;

    ip_h = (struct ip *)l3_head;
    if (ip_h->ip_v == 4) {
        ip6_h = NULL;

        proto = ip_h->ip_p;
        ip_hlen = (ip_h->ip_hl)*4;

        src_addr = &ip_h->ip_src.s_addr;
        dst_addr = &ip_h->ip_dst.s_addr;
        addr_len = OGS_IPV4_LEN;
    } else if (ip_h->ip_v == 6) {
        ip_h = NULL;
        ip6_h = (struct ip6_hdr *)l3_head;

        proto = get_ipv6_proto((struct rte_ipv6_hdr *)ip6_h, &ip_hlen);

        src_addr = (uint32_t *)ip6_h->ip6_src.s6_addr;
        dst_addr = (uint32_t *)ip6_h->ip6_dst.s6_addr;
        addr_len = OGS_IPV6_LEN;
    } else {
        ogs_error("Invalid packet IP version:%d", ip_h->ip_v);
        return NULL;
    }

    ogs_list_for_each(&pdr->rule_list, rule) {
        int k;
        uint32_t src_mask[4];
        uint32_t dst_mask[4];
        ogs_ipfw_rule_t *ipfw = NULL;

        ipfw = &rule->ipfw;
        ogs_assert(ipfw);

        ogs_debug("PROTO:%d SRC:%s\n", proto, ip6_h ? ip62str(src_addr) : ip2str(src_addr[0]));
        ogs_debug("HLEN:%d  DST:%s\n", ip_hlen, ip6_h ? ip62str(dst_addr) : ip2str(dst_addr[0]));
        ogs_debug("PROTO:%d SRC:%d-%d DST:%d-%d", ipfw->proto, ipfw->port.src.low,
                ipfw->port.src.high, ipfw->port.dst.low, ipfw->port.dst.high);
        ogs_debug("SRC:%s/%s\n", ip6_h ? ip62str(ipfw->ip.src.addr) : ip2str(ipfw->ip.src.addr[0]),
                ip6_h ? ip62str2(ipfw->ip.src.mask) : ip2str2(ipfw->ip.src.mask[0]));
        ogs_debug("DST:%s/%s\n", ip6_h ? ip62str(ipfw->ip.dst.addr) : ip2str(ipfw->ip.dst.addr[0]),
                ip6_h ? ip62str2(ipfw->ip.dst.mask) : ip2str2(ipfw->ip.dst.mask[0]));

        for (k = 0; k < 4; k++) {
            src_mask[k] = src_addr[k] & ipfw->ip.src.mask[k];
            dst_mask[k] = dst_addr[k] & ipfw->ip.dst.mask[k];
        }

        if (memcmp(src_mask, ipfw->ip.src.addr, addr_len) == 0 &&
                memcmp(dst_mask, ipfw->ip.dst.addr, addr_len) == 0) {
            if (ipfw->proto == 0) { /* IP */
                /* No need to match port */
                return rule;
            }
            if (ipfw->proto != proto) {
                continue;
            }
            if (ipfw->proto != IPPROTO_TCP && ipfw->proto != IPPROTO_UDP) {
                /* No need to match port */
                return rule;
            }
            sport = be16toh(*(uint16_t *)((char *)l3_head + ip_hlen));
            if (ipfw->port.src.low && sport < ipfw->port.src.low) {
                continue;
            }
            if (ipfw->port.src.high && sport > ipfw->port.src.high) {
                continue;
            }
            dport = be16toh(*(uint16_t *)((char *)l3_head + ip_hlen + 2));
            if (ipfw->port.dst.low && dport < ipfw->port.dst.low) {
                continue;
            }
            if (ipfw->port.dst.high && dport > ipfw->port.dst.high) {
                continue;
            }
            /* Matched */
            return rule;
        }
    }

    return NULL;
}

int fwd_handle_gtp_session_report(struct rte_ring *r, ogs_pfcp_pdr_t *pdr, uint32_t sess_index)
{
    upf_dpdk_event_t *event = dpdk_malloc(sizeof(upf_dpdk_event_t));
    upf_dpdk_event_report_t *report = dpdk_malloc(sizeof(upf_dpdk_event_report_t));
    if (!event || !report) {
        ogs_error("Error allocate memory.");
        return -1;
    }
    event->event_type = UPF_DPDK_SESS_REPORT;
    event->subtype = SESS_REPORT_NORMAL;
    report->pdr_id = pdr->id;
    if (pdr->qer && pdr->qer->qfi) {
        report->qfi = pdr->qer->qfi; /* for 5GC */
    }
    report->sess_index = sess_index;
    event->event_body = report;
    if (-ENOBUFS == rte_ring_sp_enqueue(r, event)) {
        dpdk_free(event);
        dpdk_free(report);
        return -1;
    }
    ogs_debug("%s: sess %d pdr %d qfi %d\n", __func__, sess_index, report->pdr_id, report->qfi);

    return 0;
}

ogs_pfcp_pdr_t *n3_pdr_find_by_local_sess(upf_sess_t *sess, ogs_gtp2_header_t *gtp_h, char *ip_h, uint8_t tunnel_len)
{
    uint8_t qfi = 0;
    int teid = be32toh(gtp_h->teid);
    ogs_gtp2_extension_header_t ext_hdesc;
    ogs_gtp2_header_desc_t header_desc_s;
    ogs_gtp2_header_desc_t *header_desc = &header_desc_s;
    uint8_t *ext_h = NULL;
    int i;
    uint16_t len = 0;

    if (LIKELY(gtp_h->flags & OGS_GTPU_FLAGS_E)) {
        /*临时做，确保这是第一个扩展头，否则会有问题*/
        /*
             * TS29.281
             * 5.2.1 General format of the GTP-U Extension Header
             * Figure 5.2.1-3: Definition of Extension Header Type
             *
             * Note 4 : For a GTP-PDU with several Extension Headers, the PDU
             *          Session Container should be the first Extension Header
        */
        
        /*
        ogs_gtp2_extension_header_t *ext_header =
            (ogs_gtp2_extension_header_t *)((char *)gtp_h + OGS_GTPV1U_HEADER_LEN);
        if (ext_header->array[0].type == OGS_GTP2_EXTENSION_HEADER_TYPE_PDU_SESSION_CONTAINER &&
                ext_header->array[0].pdu_type == OGS_GTP2_EXTENSION_HEADER_PDU_TYPE_UL_PDU_SESSION_INFORMATION) {
            ogs_debug("   QFI [0x%x]", ext_header->array[0].qos_flow_identifier);
            qfi = ext_header->array[0].qos_flow_identifier;
        }*/       

        if (header_desc) {
            memset(header_desc, 0, sizeof(*header_desc));

            header_desc->flags = gtp_h->flags;
            header_desc->type = gtp_h->type;
            header_desc->teid = be32toh(gtp_h->teid);
        }

        len = OGS_GTPV1U_HEADER_LEN;
        if (tunnel_len < len) {
            ogs_error("the length of the packet is insufficient[%d:%d]",
                    tunnel_len, len);
            return NULL;
        }

        if (gtp_h->flags & OGS_GTPU_FLAGS_E) {

            len += OGS_GTPV1U_EXTENSION_HEADER_LEN;
            if (tunnel_len < len) {
                ogs_error("the length of the packet is insufficient[%d:%d]",
                        tunnel_len, len);
                return NULL;
            }

            /*
             * TS29.281
             * 5.2.1 General format of the GTP-U Extension Header
             *
             * If no such Header follows,
             * then the value of the Next Extension Header Type shall be 0. */

            i = 0;
            while (*(ext_h = (((uint8_t *)gtp_h) + len - 1)) &&
                    i < OGS_GTP2_NUM_OF_EXTENSION_HEADER) {
            /*
             * The length of the Extension header shall be defined
             * in a variable length of 4 octets, i.e. m+1 = n*4 octets,
             * where n is a positive integer.
             */
                len += (*(++ext_h)) * 4;
                if (*ext_h == 0) {
                    ogs_error("No length in the Extension header");
                    return NULL;
                }

                if (((*ext_h) * 4) > OGS_GTP2_MAX_EXTENSION_HEADER_LEN) {
                    ogs_error("Overflow length : %d", (*ext_h));
                    return NULL;
                }

                if (tunnel_len < len) {
                    ogs_error("the length of the packet is insufficient[%d:%d]",
                            tunnel_len, len);
                    return -1;
                }

                if (!header_desc) /* Skip to extract header content */
                    continue;

                /* Copy Header Content */
                memcpy(&ext_hdesc.array[i], ext_h-1, (*ext_h) * 4);

                switch (ext_hdesc.array[i].type) {
                case OGS_GTP2_EXTENSION_HEADER_TYPE_PDU_SESSION_CONTAINER:
                    header_desc->pdu_type = ext_hdesc.array[i].pdu_type;
                    if (ext_hdesc.array[i].pdu_type ==
                        OGS_GTP2_EXTENSION_HEADER_PDU_TYPE_UL_PDU_SESSION_INFORMATION) {
                            header_desc->qos_flow_identifier =
                                ext_hdesc.array[i].qos_flow_identifier;
                            ogs_trace("   QFI [0x%x]",
                                    header_desc->qos_flow_identifier);
                    }
                    break;
                case OGS_GTP2_EXTENSION_HEADER_TYPE_UDP_PORT:
                    header_desc->udp.presence = true;
                    header_desc->udp.port = be16toh(ext_hdesc.array[i].udp_port);

                    ogs_trace("   UDP Port [%d]", header_desc->udp.port);
                    break;
                case OGS_GTP2_EXTENSION_HEADER_TYPE_PDCP_NUMBER:
                    header_desc->pdcp_number_presence = true;
                    header_desc->pdcp_number =
                        be16toh(ext_hdesc.array[i].pdcp_number);

                    ogs_trace("   PDCP Number [%d]", header_desc->pdcp_number);
                    break;
                default:
                    break;
                }

                i++;
            }

            if (i >= OGS_GTP2_NUM_OF_EXTENSION_HEADER) {
                ogs_error("The number of extension headers is limited to [%d]", i);
                return -1;
            }

        } else if (gtp_h->flags & (OGS_GTPU_FLAGS_S|OGS_GTPU_FLAGS_PN)) {
            /*
             * If and only if one or more of these three flags are set,
             * the fields Sequence Number, N-PDU and Extension Header
             * shall be present. The sender shall set all the bits of
             * the unused fields to zero. The receiver shall not evaluate
             * the unused fields.
             * For example, if only the E flag is set to 1, then
             * the N-PDU Number and Sequence Number fields shall also be present,
             * but will not have meaningful values and shall not be evaluated.
             */
            len += 4;
        }

    }

    ogs_pfcp_pdr_t *pdr = NULL;

    ogs_list_for_each(&sess->pfcp.pdr_list, pdr) {
        if (pdr->src_if != OGS_PFCP_INTERFACE_ACCESS &&
                pdr->src_if != OGS_PFCP_INTERFACE_CP_FUNCTION) {
            continue;
        }
        if (teid != pdr->f_teid.teid) {
            continue;
        }
        if (header_desc->qos_flow_identifier && pdr->qfi != header_desc->qos_flow_identifier) {
            continue;
        }
        if (ogs_list_first(&pdr->rule_list) &&
                pfcp_pdr_rule_find_by_packet(pdr, (char *)ip_h) == NULL) {
            continue;
        }
        break;
    }

    return pdr;
}

void handle_gpdu_prepare(struct rte_mbuf *m)
{
    struct packet *pkt = (struct packet *)(m->buf_addr);

    int decrease = pkt->l2_len + pkt->l3_len + pkt->l4_len + pkt->tunnel_len + pkt->vxlan_len - sizeof(struct rte_ether_hdr);
    m->data_off += decrease;
    m->data_len -= decrease;
    m->pkt_len -= decrease;
    struct rte_ether_hdr *eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

    pkt->l2_len = sizeof(struct rte_ether_hdr);
    struct ip *ip_h = rte_pktmbuf_mtod_offset(m, struct ip *, pkt->l2_len);
    if (UNLIKELY(ip_h->ip_v == 6)) {
        pkt->is_ipv4 = 0;
        get_ipv6_proto((struct rte_ipv6_hdr *)ip_h, &pkt->l3_len);
        eth_h->ether_type = BE_ETH_P_IPV6;
    } else {
        pkt->is_ipv4 = 1;
        pkt->l3_len = (((struct rte_ipv4_hdr *)ip_h)->version_ihl & 0x0f) * 4;
        eth_h->ether_type = BE_ETH_P_IP;
    }
    pkt->l4_len = 0;
    pkt->tunnel_len = 0;
}

int process_dst_if_interface_core(struct lcore_conf *lconf, struct rte_mbuf *m, char *in_l3_head)
{
    uint32_t ring = 0;
    arp_node_t *arp = NULL;
    nd_node_t *nd = NULL;
    struct rte_ether_hdr *eth_h = NULL;
    struct packet *pkt = (struct packet *)(m->buf_addr);
    uint8_t is_ipv4 = 0;

    struct ip *ip_h = (struct ip *)in_l3_head;
    if (ip_h->ip_v == 4) {
        is_ipv4 = 1;
        struct rte_ipv4_hdr *in_ipv4_h = (struct rte_ipv4_hdr *)ip_h;
        if (ue_addr_match(in_ipv4_h->dst_addr)) { //ue to ue, send to owner fwd;
            pkt->pkt_type = PKT_TYPE_IP_N6;
            ring = ntohl(in_ipv4_h->dst_addr) % dkuf.fwd_num;
            if (-ENOBUFS == rte_ring_sp_enqueue(dkuf.lconf[dkuf.fwd_lcore[ring]].f2f_ring, m)) {
                ogs_error("%s, to %s enqueue f2fring failed\n", __func__, ip2str(in_ipv4_h->dst_addr));
                return -1;
            }
            lconf->lstat.f2f_enqueue++;
            return 0;
        }

        //route find;
        eth_h = (struct rte_ether_hdr *)((char *)ip_h - sizeof(*eth_h));
        mac_copy((struct rte_ether_addr *)&dkuf.mac[1], &eth_h->s_addr);

        //如果目的IP跟N6在同一网段，则直接查询目的IP的MAC，否则查询GW的MAC
        ogs_debug("dstaddr:%s, n6addr:%s, gw:%s,mask:%d", ip2str(in_ipv4_h->dst_addr),ip2str(dkuf.n6_addr.ipv4),ip2str(dkuf.n6_addr.gw),dkuf.n6_addr.mask);
        if ((in_ipv4_h->dst_addr&dkuf.n6_addr.mask ) == (dkuf.n6_addr.ipv4 & dkuf.n6_addr.mask)){
            arp = arp_find(lconf, in_ipv4_h->dst_addr, 1);
        }else{
            ogs_info("find gw's mac");
            arp = arp_find(lconf, dkuf.n6_addr.gw, 1);
        }
        
        if (arp->flag == ARP_ND_SEND) {
            if (arp->pkt_list_cnt < MAX_PKT_BURST) {
                pkt->next = arp->pkt_list;
                arp->pkt_list = pkt;
                arp->pkt_list_cnt++;
                return 0;
            } else {
                lconf->lstat.tx_dropped[1]++;
                return -1;
            }
        }
        mac_copy((struct rte_ether_addr *)arp->mac, &eth_h->d_addr);
    } else if (ip_h->ip_v == 6) {
        struct rte_ipv6_hdr *in_ipv6_h = (struct rte_ipv6_hdr *)ip_h;
        if (ue_addr6_match(in_ipv6_h->dst_addr)) { //ue to ue, send to owner fwd;
            pkt->pkt_type = PKT_TYPE_IP_N6;
            ring = *(uint32_t *)(&in_ipv6_h->dst_addr[12]) % dkuf.fwd_num;
            if (-ENOBUFS == rte_ring_sp_enqueue(dkuf.lconf[dkuf.fwd_lcore[ring]].f2f_ring, m)) {
                ogs_error("%s, to %s enqueue f2fring failed\n", __func__, ip62str(in_ipv6_h->dst_addr));
                return -1;
            }
            lconf->lstat.f2f_enqueue++;
            return 0;
        }

        eth_h = (struct rte_ether_hdr *)((char *)ip_h - sizeof(*eth_h));
        mac_copy((struct rte_ether_addr *)&dkuf.mac[1], &eth_h->s_addr);
        //nd = nd_find(lconf, in_ipv6_h->dst_addr, 1);
        if ((in_ipv6_h->dst_addr[0]&dkuf.n6_addr.mask6[0] ) == (dkuf.n6_addr.ipv6[0] & dkuf.n6_addr.mask6[0]) && 
            (in_ipv6_h->dst_addr[1]&dkuf.n6_addr.mask6[1] ) == (dkuf.n6_addr.ipv6[1] & dkuf.n6_addr.mask6[1])){
            nd = nd_find(lconf, in_ipv6_h->dst_addr, 1);
        }else{
            nd = nd_find(lconf, dkuf.n6_addr.gw6, 1);
        }
        if (nd->flag == ARP_ND_SEND) {
            if (nd->pkt_list_cnt < MAX_PKT_BURST) {
                pkt->next = nd->pkt_list;
                nd->pkt_list = pkt;
                nd->pkt_list_cnt++;
                return 0;
            } else {
                lconf->lstat.tx_dropped[1]++;
                return -1;
            }
        }
        mac_copy((struct rte_ether_addr *)nd->mac, &eth_h->d_addr);
    } else {
        return -1;
    }

    send_packet(lconf, 1, m, is_ipv4);

    return 0;
}

int process_dst_if_interface_access(struct lcore_conf *lconf, struct rte_mbuf *m, ogs_pfcp_pdr_t *pdr)
{
    uint8_t downlink_data_report = 0;
    if (pfcp_up_handle_pdr(pdr, m, &downlink_data_report) < 0) {
        ogs_error("dst if access up handler failed\n");
        return -1;
    }
    if (downlink_data_report) {
        ogs_error("Indirect Data Fowarding Buffered");
        lconf->lstat.sess_report[m->port]++;
        fwd_handle_gtp_session_report(lconf->f2p_ring, pdr, *(UPF_SESS(pdr->sess))->upf_n4_seid_node);
    }

    return 0;
}

int process_dst_if_interface_cp_func(struct lcore_conf *lconf, struct rte_mbuf *m, ogs_pfcp_pdr_t *pdr)
{
    ogs_pfcp_far_t *far = pdr->far;
    if (!far || !far->gnode) {
        ogs_error("No Outer Header Creation in FAR");
        return -1;
    }
    if ((far->apply_action & OGS_PFCP_APPLY_ACTION_FORW) == 0) {
        ogs_error("Not supported Apply Action [0x%x]", far->apply_action);
        return -1;
    }
    if (pfcp_send_g_pdu(pdr, m) < 0) {
        ogs_error("dst if access up handler failed\n");
        return -1;
    }

    return 0;
}

int upf_gtp_handle_multicast(struct rte_mbuf *m)
{
    struct ip *ip_h =  NULL;
    struct ip6_hdr *ip6_h =  NULL;
    char *l3_head = rte_pktmbuf_mtod(m, char *);
    uint8_t downlink_data_report = 0;

    ip_h = (struct ip *)l3_head;
    if (ip_h->ip_v == 6) {
        struct in6_addr ip6_dst;
        ip6_h = (struct ip6_hdr *)l3_head;
        memcpy(&ip6_dst, &ip6_h->ip6_dst, sizeof(struct in6_addr));
        if (IN6_IS_ADDR_MULTICAST(&ip6_dst) == 0) {
            return -1;
        }
        upf_sess_t *sess = NULL;
        ogs_list_for_each(&upf_self()->sess_list, sess) {
            if (!sess->ipv6) {
                continue;
            }
            /* PDN IPv6 is avaiable */
            ogs_pfcp_pdr_t *pdr = NULL;
            ogs_list_for_each(&sess->pfcp.pdr_list, pdr) {
                if (pdr->src_if == OGS_PFCP_INTERFACE_CORE) {
                    return pfcp_up_handle_pdr(pdr, m, &downlink_data_report);
                }
            }

            return 0;
        }
    }

    return -1;
}

ogs_pfcp_pdr_t *n6_pdr_find_by_local_sess(upf_sess_t *sess, char *l3_head)
{
    ogs_pfcp_pdr_t *pdr = NULL;
    ogs_pfcp_pdr_t *fallback_pdr = NULL;
    ogs_pfcp_far_t *far = NULL;

    ogs_list_for_each(&sess->pfcp.pdr_list, pdr) {
        far = pdr->far;
        ogs_assert(far);
        /* Check if PDR is Downlink */
        if (pdr->src_if != OGS_PFCP_INTERFACE_CORE)
            continue;

        /* Save the Fallback PDR : Lowest precedence downlink PDR */
        fallback_pdr = pdr;

        /* Check if FAR is Downlink */
        if (far->dst_if != OGS_PFCP_INTERFACE_ACCESS)
            continue;

        /* Check if Outer header creation */
        if (far->outer_header_creation.ip4 == 0 &&
                far->outer_header_creation.ip6 == 0 &&
                far->outer_header_creation.udp4 == 0 &&
                far->outer_header_creation.udp6 == 0 &&
                far->outer_header_creation.gtpu4 == 0 &&
                far->outer_header_creation.gtpu6 == 0)
            continue;

        /* Check if Rule List in PDR */
        if (ogs_list_first(&pdr->rule_list) &&
                pfcp_pdr_rule_find_by_packet(pdr, l3_head) == NULL)
            continue;

        break;
    }

    if (!pdr) {
        pdr = fallback_pdr;
    }

    return pdr;
}

void fwd_flush_buffered_packet(upf_sess_t *sess)
{
    ogs_pfcp_pdr_t *pdr = NULL;
    ogs_pfcp_far_t *far = NULL;
    int i;

    ogs_list_for_each(&sess->pfcp.pdr_list, pdr) {
        if (pdr->src_if != OGS_PFCP_INTERFACE_CORE) { /* Downlink */
            continue;
        }
        far = pdr->far;
        if (!far || !far->num_of_buffered_packet) {
            continue;
        }
        if (far->gnode && (far->apply_action & OGS_PFCP_APPLY_ACTION_FORW)) {
            for (i = 0; i < far->num_of_buffered_packet; i++) {
                if (pfcp_send_g_pdu(pdr, (struct rte_mbuf *)far->buffered_packet[i]) < 0) {
                    rte_pktmbuf_free((struct rte_mbuf *)far->buffered_packet[i]);
                }
            }
        } else {
            for (i = 0; i < far->num_of_buffered_packet; i++) {
                rte_pktmbuf_free((struct rte_mbuf *)far->buffered_packet[i]);
            }
        }
        far->num_of_buffered_packet = 0;
    }
}


int send_packet_to_nbr(struct lcore_conf *lconf, struct rte_mbuf *m, uint32_t nbraddr)
{
    struct rte_ether_hdr *eth_h;
    struct packet *pkt = (struct packet *)(m->buf_addr);
    uint8_t is_ipv4 = 0;
	uint32_t netxhop = 0;
	arp_node_t *arp = NULL;
	nd_node_t *nd = NULL;
	
    eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

    char *l3_head = rte_pktmbuf_mtod_offset(m, char *, pkt->l2_len);
	
	struct ip *ip_h = (struct ip *)l3_head;
	if (ip_h->ip_v == 4) {
		is_ipv4 = 1;
		struct rte_ipv4_hdr *in_ipv4_h = (struct rte_ipv4_hdr *)ip_h;

		//mac_copy((struct rte_ether_addr *)&dkuf.mac[1], &eth_h->src_addr);
		mac_copy((struct rte_ether_addr *)&dkuf.mac[0], &eth_h->s_addr);
		//arp = arp_find(lconf, in_ipv4_h->dst_addr, 1);
        //TODO 搞清楚getnexthop是干嘛的
		/*netxhop = getnexthop(lconf, nbraddr);
		if(netxhop == 0)
		{
			lconf->lstat.tx_dropped[1]++;
			return -1;
		}*/
		arp = arp_find(lconf, in_ipv4_h->dst_addr, 0);
		//arp = arp_find(lconf, netxhop, 0);
		if (arp->flag == ARP_ND_SEND) {
			if (arp->pkt_list_cnt < MAX_PKT_BURST) {
				pkt->next = arp->pkt_list;
				arp->pkt_list = pkt;
				arp->pkt_list_cnt++;
				return 0;
			} else {
				lconf->lstat.tx_dropped[1]++;
				return -1;
			}
		}
		mac_copy((struct rte_ether_addr *)arp->mac, &eth_h->d_addr);
	} else if (ip_h->ip_v == 6) {
		#if 0
		struct rte_ipv6_hdr *in_ipv6_h = (struct rte_ipv6_hdr *)ip_h;

		eth_h = (struct rte_ether_hdr *)((char *)ip_h - sizeof(*eth_h));
		mac_copy((struct rte_ether_addr *)&dkuf.mac[0], &eth_h->s_addr);
		nd = nd_find(lconf, in_ipv6_h->dst_addr, 1);
		if (nd->flag == ARP_ND_SEND) {
			if (nd->pkt_list_cnt < MAX_PKT_BURST) {
				pkt->next = nd->pkt_list;
				nd->pkt_list = pkt;
				nd->pkt_list_cnt++;
				return 0;
			} else {
				lconf->lstat.tx_dropped[1]++;
				return -1;
			}
		}
		mac_copy((struct rte_ether_addr *)nd->mac, &eth_h->d_addr);
		#endif
	} else {
		return -1;
	}

	//send_packet(lconf, 1, m, is_ipv4);
	send_packet(lconf, 0, m, is_ipv4);

	return 0;
}

int send_packet_to_nbr_ipip(struct lcore_conf *lconf, struct rte_mbuf *m, uint32_t nbraddr)
{
    struct rte_ether_hdr *eth_h;
    struct packet *pkt = (struct packet *)(m->buf_addr);
    uint8_t is_ipv4 = 0;
	uint32_t netxhop = 0;
	arp_node_t *arp = NULL;
	nd_node_t *nd = NULL;
    struct rte_ipv4_hdr *ipv4_h = NULL;
	
    char *ptr = rte_pktmbuf_mtod(m, char *);
	ipv4_h = (struct rte_ipv4_hdr *)(ptr + pkt->l2_len - IP_HDR_LEN);
	
    int l4_len = m->pkt_len - pkt->l2_len + IP_HDR_LEN;
	
    eth_h = (struct rte_ether_hdr *)((char *)ipv4_h - L2_HDR_LEN);
    pkt->l3_len = IP_HDR_LEN;

	static uint16_t pkt_id = 100;
    ipv4_h->version_ihl = IP_VHL_DEF;
    ipv4_h->type_of_service = 0;
    ipv4_h->packet_id = pkt_id++;
    ipv4_h->fragment_offset = 0; /* [15,14,13] bits for flags, [12 - 0] bits for offset */
    ipv4_h->time_to_live = IPDEFTTL - 1;
    ipv4_h->next_proto_id = IPPROTO_IPIP;
    ipv4_h->src_addr = dkuf.n6_addr.ipv4;
    ipv4_h->dst_addr = nbraddr;
    ipv4_h->total_length = rte_cpu_to_be_16(IP_HDR_LEN + l4_len);
    ipv4_h->hdr_checksum = 0;
    if (dkuf.tx_offloads[0] & DEV_TX_OFFLOAD_IPV4_CKSUM) {
        m->l3_len = IP_HDR_LEN;  //set l2_len & used m->l2_len/l3_len later;
        m->ol_flags |= (PKT_TX_IPV4 | PKT_TX_IP_CKSUM);
    } else {
        m->ol_flags &= (~(PKT_TX_IPV4 | PKT_TX_IP_CKSUM));
        ipv4_h->hdr_checksum = rte_ipv4_cksum(ipv4_h);
    }
    eth_h->ether_type = BE_ETH_P_IP;
    is_ipv4 = 1;

	int increase = IP_HDR_LEN;
    m->data_len += increase;
    m->pkt_len += increase;
    m->data_off -= increase;
	
    char *l3_head = rte_pktmbuf_mtod_offset(m, char *, pkt->l2_len);
	
	struct ip *ip_h = (struct ip *)l3_head;
	if (ip_h->ip_v == 4) {
		is_ipv4 = 1;
		struct rte_ipv4_hdr *in_ipv4_h = (struct rte_ipv4_hdr *)ip_h;

		//mac_copy((struct rte_ether_addr *)&dkuf.mac[1], &eth_h->src_addr);
		mac_copy((struct rte_ether_addr *)&dkuf.mac[0], &eth_h->s_addr);
		//arp = arp_find(lconf, in_ipv4_h->dst_addr, 1);
#if 0   /* getnexthop 后续搞清楚再完善*/     
		netxhop = getnexthop(lconf, nbraddr);
		if(netxhop == 0)
		{   
			lconf->lstat.tx_dropped[1]++;
			return -1;
		}
		//arp = arp_find(lconf, in_ipv4_h->dst_addr, 0);
		arp = arp_find(lconf, netxhop, 0);
#endif   
        arp = arp_find(lconf, in_ipv4_h->dst_addr, 0);  
		if (arp->flag == ARP_ND_SEND) {
			if (arp->pkt_list_cnt < MAX_PKT_BURST) {
				pkt->next = arp->pkt_list;
				arp->pkt_list = pkt;
				arp->pkt_list_cnt++;
				return 0;
			} else {
				lconf->lstat.tx_dropped[1]++;
				return -1;
			}
		}
		mac_copy((struct rte_ether_addr *)arp->mac, &eth_h->d_addr);
	} else if (ip_h->ip_v == 6) {
		#if 0
		struct rte_ipv6_hdr *in_ipv6_h = (struct rte_ipv6_hdr *)ip_h;

		eth_h = (struct rte_ether_hdr *)((char *)ip_h - sizeof(*eth_h));
		mac_copy((struct rte_ether_addr *)&dkuf.mac[0], &eth_h->s_addr);
		nd = nd_find(lconf, in_ipv6_h->dst_addr, 1);
		if (nd->flag == ARP_ND_SEND) {
			if (nd->pkt_list_cnt < MAX_PKT_BURST) {
				pkt->next = nd->pkt_list;
				nd->pkt_list = pkt;
				nd->pkt_list_cnt++;
				return 0;
			} else {
				lconf->lstat.tx_dropped[1]++;
				return -1;
			}
		}
		mac_copy((struct rte_ether_addr *)nd->mac, &eth_h->d_addr);
		#endif
	} else {
		return -1;
	}

	//send_packet(lconf, 1, m, is_ipv4);
	send_packet(lconf, 0, m, is_ipv4);

	return 0;
}


