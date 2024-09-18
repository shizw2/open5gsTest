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
#include <dpdk-path.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <rte_udp.h>
#include <rte_tcp.h>
#include <rte_icmp.h>

static int32_t handle_n6_pkt(struct lcore_conf *lconf, struct rte_mbuf *m);

int32_t handle_arp(struct lcore_conf *lconf, struct rte_mbuf *m);
int32_t handle_arp(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    struct rte_ether_hdr *eth_h;
    struct rte_arp_hdr *arp_h;

    eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    arp_h = (struct rte_arp_hdr *)(eth_h + 1);
    ogs_debug("arp %s --> %s\n", ip2str(arp_h->arp_data.arp_sip), ip2str2(arp_h->arp_data.arp_tip));

    struct rte_arp_ipv4 *arp_data = &arp_h->arp_data;
    uint32_t sip = arp_data->arp_sip ? arp_data->arp_sip : arp_data->arp_tip;
    arp_node_t *arp = arp_hash_find(lconf->arp_tbl, sip);
    if (!arp) {
        arp = arp_create(lconf->arp_tbl, sip, m->port);
    }

    ogs_debug("find arp, ip:%s, mac:%s, flag %d\n", ip2str(arp_data->arp_sip), mac2str((struct rte_ether_addr *)arp->mac), arp->flag);
    if (!mac_cmp((char *)arp->mac, (char *)&arp_h->arp_data.arp_sha)) {
        mac_copy(&arp_h->arp_data.arp_sha, (struct rte_ether_addr *)arp->mac);
        ogs_debug("update arp mac\n");
        //mac_print((struct rte_ether_addr *)arp->mac);
    }
    arp->up_sec = dkuf.sys_up_sec;
    arp->flag = ARP_ND_OK;

    if (arp_h->arp_opcode == htons(RTE_ARP_OP_REQUEST)) {
        ogs_debug("got arp request for port %d\n", m->port);
        mac_copy(&eth_h->s_addr, &eth_h->d_addr);
        mac_copy(&dkuf.mac[m->port], &eth_h->s_addr);
        arp_h->arp_opcode = rte_cpu_to_be_16(RTE_ARP_OP_REPLY);
        SWAP(arp_data->arp_sip, arp_data->arp_tip);

        mac_copy(&arp_data->arp_sha, &arp_data->arp_tha);
        mac_copy(&dkuf.mac[m->port], &arp_data->arp_sha);

        mac_print(&dkuf.arp_req[m->port].eth_hdr.s_addr);
        send_single_packet(lconf, m->port, m);
    } else if (arp_h->arp_opcode == htons(RTE_ARP_OP_REPLY)) {
        if (arp && arp->pkt_list) {
            struct rte_mbuf *fm;
            struct packet *next_pkt, *pkt;
            pkt = (struct packet *)arp->pkt_list;
            while (pkt) {
                next_pkt = pkt->next;
                fm = packet_meta(pkt);
                mac_copy(arp->mac, rte_pktmbuf_mtod(fm, char*));
                send_packet(lconf, arp->port, fm, 1);
                pkt->next = NULL;
                pkt = next_pkt;
            }
            arp->pkt_list_cnt = 0;
            arp->pkt_list = NULL;
        }
        ogs_debug("got arp reply for port %d\n", m->port);
        return -1;
    }

    return 0;
}

int32_t handle_nd(struct lcore_conf *lconf, struct rte_mbuf *m);
int32_t handle_nd(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    struct rte_ether_hdr *eth_h;
    struct rte_ipv6_hdr *ipv6_h;
    struct nd_msg *na;
    struct packet *pkt = (struct packet *)(m->buf_addr);

    eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    ipv6_h = (struct rte_ipv6_hdr *)(eth_h + 1);
    na = (struct nd_msg *)((char *)ipv6_h + pkt->l3_len);

    //dad handle

    uint32_t hash = ipv6_addr_hash((uint32_t *)&na->target);
    nd_node_t *nd = nd_hash_find(lconf->nd_tbl, (uint64_t *)&na->target, hash);
    if (!nd) {
        nd = nd_create(lconf->nd_tbl, &na->target, m->port);
    }

    //ogs_debug("find nd %s, flag %d\n", ipv6 + 128, nd->flag);
    if (!mac_cmp((char *)nd->mac, (char *)(na->opt + 2))) {
        mac_copy((struct rte_ether_addr *)(na->opt + 2), (struct rte_ether_addr *)nd->mac);
        ogs_debug("update nd mac\n");
        mac_print((struct rte_ether_addr *)nd->mac);
    }
    nd->up_sec = dkuf.sys_up_sec;
    nd->flag = ARP_ND_OK;

    uint8_t type = na->icmph.icmp6_type;
    if (type == ND_NEIGHBOR_SOLICIT) {
        ogs_debug("got ns for port %d\n", m->port);
        mac_copy(&eth_h->s_addr, &eth_h->d_addr); //modify multi addr
        mac_copy(&dkuf.mac[m->port], &eth_h->s_addr);

        na->icmph.icmp6_type = ND_NEIGHBOR_ADVERT;
        na->icmph.icmp6_data32[0] = 0;
        na->icmph.icmp6_data32[0] |= ND_NA_FLAG_SOLICITED;
        na->icmph.icmp6_data32[0] |= ND_NA_FLAG_OVERRIDE;

        mac_copy(&eth_h->s_addr, &na->opt[2]);
        na->opt[0] = ND_OPT_TARGET_LINKADDR;
        na->opt[1] = 1; //len >> 3
        memcpy(ipv6_h->dst_addr, ipv6_h->src_addr, 16);
        memcpy(ipv6_h->src_addr, &na->target, 16);

        na->icmph.icmp6_cksum = 0;
        na->icmph.icmp6_cksum = ogs_in_cksum((uint16_t *)ipv6_h, 40 + ntohs(ipv6_h->payload_len));

        mac_copy(&eth_h->s_addr, (struct rte_ether_addr *)(na->opt + 2));

        mac_print(&eth_h->s_addr);
        send_single_packet(lconf, m->port, m);
    } else if (type == ND_NEIGHBOR_ADVERT) {
        if (nd && nd->pkt_list) {
            struct rte_mbuf *fm;
            struct packet *next_pkt, *pkt;
            pkt = (struct packet *)nd->pkt_list;
            while (pkt) {
                next_pkt = pkt->next;
                fm = packet_meta(pkt);
                mac_copy(nd->mac, rte_pktmbuf_mtod(fm, char*));
                send_packet(lconf, nd->port, fm, 0);
                pkt->next = NULL;
                pkt = next_pkt;
            }
            nd->pkt_list_cnt = 0;
            nd->pkt_list = NULL;
        }
        ogs_debug("got na for port %d\n", m->port);
        return -1;
    }
    return 0;
}

int32_t handle_ping(struct lcore_conf *lconf, struct rte_mbuf *m);
int32_t handle_ping(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    struct rte_ether_hdr *eth_h;
    struct packet *pkt = (struct packet *)(m->buf_addr);

    eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    struct rte_ipv4_hdr *ipv4_h = (struct rte_ipv4_hdr *)((char *)eth_h + pkt->l2_len);
    struct rte_icmp_hdr *icmp_h = (struct rte_icmp_hdr *)((char *)ipv4_h + pkt->l3_len);

    uint16_t icmp_type = icmp_h->icmp_type;
    if (icmp_type != RTE_IP_ICMP_ECHO_REQUEST) {
        return -1;
    }

    ogs_debug("got icmp request for port %d\n", m->port);
    mac_swap(&eth_h->s_addr, &eth_h->d_addr);
    SWAP(ipv4_h->src_addr, ipv4_h->dst_addr);
    ipv4_h->hdr_checksum = 0;
    ipv4_h->hdr_checksum = rte_ipv4_cksum(ipv4_h);
    icmp_h->icmp_type = RTE_IP_ICMP_ECHO_REPLY;
    uint32_t cksum = ~icmp_h->icmp_cksum & 0xffff;
    cksum += ~htons(RTE_IP_ICMP_ECHO_REQUEST << 8) & 0xffff;
    cksum += htons(RTE_IP_ICMP_ECHO_REPLY << 8);
    cksum = (cksum & 0xffff) + (cksum >> 16);
    cksum = (cksum & 0xffff) + (cksum >> 16);
    icmp_h->icmp_cksum = ~cksum;

    send_single_packet(lconf, m->port, m);

    return 0;
}

int32_t handle_ping6(struct lcore_conf *lconf, struct rte_mbuf *m);
int32_t handle_ping6(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    struct rte_ether_hdr *eth_h;
    struct rte_ipv6_hdr *ipv6_h;
    struct icmp6_hdr *icmp6_h;
    struct packet *pkt = (struct packet *)(m->buf_addr);

    eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    ipv6_h = (struct rte_ipv6_hdr *)((char *)eth_h + pkt->l2_len);
    icmp6_h = (struct icmp6_hdr *)((char *)ipv6_h + pkt->l3_len);

    uint8_t icmp_type = icmp6_h->icmp6_type;
    if (icmp_type != ICMP6_ECHO_REQUEST) {
        return -1;
    }
    ogs_debug("got ping6 request for port %d\n", m->port);
    mac_swap(&eth_h->s_addr, &eth_h->d_addr);
    ipv6_swap(ipv6_h->src_addr, ipv6_h->dst_addr);
    icmp6_h->icmp6_type = ICMP6_ECHO_REPLY;
    uint8_t *p = (uint8_t *)&icmp6_h->icmp6_cksum;
    *p = (*p) - 1;

    send_single_packet(lconf, m->port, m);

    return 0;
}

static int32_t handle_n3_pkt(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    struct packet *pkt = (struct packet *)(m->buf_addr);
    struct rte_ether_hdr *eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);
    ogs_gtp2_header_t *gtp_h = (ogs_gtp2_header_t *)((char *)eth_h + pkt->l2_len + pkt->l3_len + pkt->l4_len);
    ogs_gtp2_header_desc_t header_desc;
    
    ogs_debug("[RECV] GPU-U Type [%d] from [%s] : TEID[0x%x]",
            gtp_h->type, ip_printf((char *)eth_h + pkt->l2_len, 0), be32toh(gtp_h->teid));

    if (LIKELY(gtp_h->type == OGS_GTPU_MSGTYPE_GPDU)) {
        char *in_l3_head = (char *)gtp_h + pkt->tunnel_len;

        upf_sess_t *sess = local_sess_find_by_ue_ip(lconf, in_l3_head, 0);
        if (!sess) {
            ogs_debug("%s, unmatch session by ip %s\n", __func__, ip_printf(in_l3_head, 0));
            lconf->lstat.sess_unmatch[0]++;
            return -1;
        }
        ogs_pfcp_pdr_t *pdr = NULL;
        ogs_pfcp_far_t *far = NULL;

        pdr = n3_pdr_find_by_local_sess(sess, gtp_h, in_l3_head, pkt->tunnel_len);
        if (!pdr || !pdr->sess) {
            ogs_error("%s, unfound pdr by local session, ip %s\n",
                    __func__, ip_printf(in_l3_head, 0));
            /* TODO : Send Error Indication */
            return -1;
        }
        lconf->lstat.sess_match[0]++;

        //如果sess支持vxlan,则进行vxlan报文的处理
        if (sess->support_vxlan_flag){
            struct vxlan_tunnel_header *vxlan_header;
            struct rte_ether_hdr *eth_h;
            struct rte_arp_hdr *arp_h;
            uint32_t vni;
            vxlan_header = rte_pktmbuf_mtod_offset(m, struct vxlan_tunnel_header *, pkt->l2_len + pkt->l3_len + pkt->l4_len + pkt->tunnel_len);
            vni = rte_be_to_cpu_32(vxlan_header->vxlan.vni)>>8;
            ogs_debug("vtep src_addr:%s, vtep dst_addr:%s, vni:%d",ip2str(vxlan_header->ip.src_addr), ip2str(vxlan_header->ip.dst_addr), vni);
            
            if (vni != sess->vni){
                ogs_error("%s, vni not match, vni in pkt:%d, vni in sess:%d\n",
                    __func__, vni, sess->vni);
                return -1;
            }

            eth_h = rte_pktmbuf_mtod_offset(m, struct rte_ether_hdr *, pkt->l2_len + pkt->l3_len + pkt->l4_len + pkt->tunnel_len + IP_HDR_LEN +UDP_HDR_LEN + VXLAN_HDR_LEN);
            if (eth_h->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_ARP)){
                arp_h = (struct rte_arp_hdr *)(eth_h + 1);
                struct rte_arp_ipv4 *arp_data = &arp_h->arp_data;
                    
                uint32_t sip = arp_data->arp_sip ? arp_data->arp_sip : arp_data->arp_tip;
                arp_node_t *arp = arp_hash_find(lconf->arp_tbl, sip);
                if (!arp) {
                    arp = arp_create(lconf->arp_tbl, sip, m->port);
                    ogs_info("not find vxlan arp, create a new one, ip:%s.\n", ip2str(sip));                    
                }
 
                if (!mac_cmp((char *)arp->mac, (char *)&arp_h->arp_data.arp_sha)) {
                    mac_copy(&arp_h->arp_data.arp_sha, (struct rte_ether_addr *)arp->mac);
                    ogs_debug("update arp mac\n");
                    //mac_print((struct rte_ether_addr *)arp->mac);
                }
                arp->up_sec = dkuf.sys_up_sec;
                arp->flag = ARP_ND_VXLAN_OK;
                    
                if (arp_h->arp_opcode == rte_cpu_to_be_16(RTE_ARP_OP_REQUEST)){
                    ogs_debug("test: it is an vxlan arp request, srcip:%s, desip:%s,s_addr:%s, d_addr:%s,pkt->vxlan_len:%d",ip2str(arp_data->arp_sip),ip2str(arp_data->arp_tip),mac2str(&eth_h->s_addr),mac2str(&eth_h->d_addr),pkt->vxlan_len);
                    pkt->vxlan_len = 0;            
                    handle_gpdu_prepare(m);
                    
                    eth_h->ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_ARP);
                    uint32_t ring = 0;
                    mac_copy(&eth_h->s_addr, &eth_h->d_addr);
                    mac_copy(&dkuf.mac[m->port], &eth_h->s_addr);
                    arp_h->arp_opcode = rte_cpu_to_be_16(RTE_ARP_OP_REPLY);
                    SWAP(arp_data->arp_sip, arp_data->arp_tip);

                    mac_copy(&arp_data->arp_sha, &arp_data->arp_tha);
                    mac_copy(&dkuf.mac[m->port], &arp_data->arp_sha);
             
                    
                    struct rte_ipv4_hdr *in_ipv4_h = (struct rte_ipv4_hdr *)in_l3_head;
                    
                    SWAP(in_ipv4_h->src_addr, in_ipv4_h->dst_addr);
                    in_ipv4_h->hdr_checksum = 0;
                    in_ipv4_h->hdr_checksum = rte_ipv4_cksum(in_ipv4_h);
                     
                    struct rte_udp_hdr *in_udp_h = (struct rte_udp_hdr *)((char *)in_l3_head + IP_HDR_LEN);
                    in_udp_h->src_port = htons(4789);
                    in_udp_h->dst_port = htons(4789);

                    in_udp_h->dgram_cksum = 0;
                    //如果不计算checksum或计算错误,则对端vxlan会处理错误
                    in_udp_h->dgram_cksum = rte_ipv4_udptcp_cksum(in_ipv4_h,in_udp_h);
                     
                    pdr = n6_pdr_find_by_local_sess(sess, in_l3_head);
                    if (!pdr) {
                        ogs_error("%s, unfound pdr by local session, ip %s\n", __func__, ip_printf(in_l3_head, 1));
                        if (ogs_global_conf()->parameter.multicast) {
                            return upf_gtp_handle_multicast(m);
                        }
                        return -1;
                    }

                    uint8_t downlink_data_report = 0;
                    if (pfcp_up_handle_pdr(pdr, m, &downlink_data_report) < 0) {
                        return -1;
                    }

                    if (downlink_data_report) {
                        ogs_assert(pdr->sess);
                        sess = UPF_SESS(pdr->sess);
                        ogs_assert(sess);

                        ogs_debug("%s, pkt first buffered, reports downlink notifications.\n", __func__);
                        lconf->lstat.sess_report[m->port]++;
                        fwd_handle_gtp_session_report(lconf->f2p_ring, pdr, *sess->upf_n4_seid_node);
                    }

                    return 0;
                }else if (arp_h->arp_opcode == htons(RTE_ARP_OP_REPLY)) {
                    ogs_debug("test: it is an vxlan arp reply, srcip:%s, desip:%s,s_addr:%s, d_addr:%s",ip2str(arp_data->arp_sip),ip2str(arp_data->arp_tip),mac2str(&eth_h->s_addr),mac2str(&eth_h->d_addr));
                   
                    if (arp && arp->pkt_list) {
                        struct rte_mbuf *fm;
                        struct packet *next_pkt, *pkt;
                        pkt = (struct packet *)arp->pkt_list;
                        while (pkt) {
                            next_pkt = pkt->next;
                            ogs_info("test:handle buffred n6 pkt.");
                            fm = packet_meta(pkt);
                            handle_n6_pkt(lconf, fm);                            
                            //mac_copy(arp->mac, rte_pktmbuf_mtod(fm, char*));
                            //send_packet(lconf, arp->port, fm, 1);
                            pkt->next = NULL;
                            pkt = next_pkt;
                        }
                        arp->pkt_list_cnt = 0;
                        arp->pkt_list = NULL;
                    }
                    return 0;
                }
            }else{
                //更新为实际的IP头
                in_l3_head = (char *)gtp_h + pkt->tunnel_len + IP_HDR_LEN +UDP_HDR_LEN + VXLAN_HDR_LEN + RTE_ETHER_HDR_LEN;
                struct rte_ipv4_hdr *in_ipv4_h = (struct rte_ipv4_hdr *)in_l3_head;
                ogs_debug("test:skip vxlan, ip in src_addr:%s,in dst_addr:%s,proto:%d",ip2str(in_ipv4_h->src_addr),ip2str(in_ipv4_h->dst_addr),in_ipv4_h->next_proto_id);
                pkt->vxlan_len = IP_HDR_LEN +UDP_HDR_LEN + VXLAN_HDR_LEN + RTE_ETHER_HDR_LEN;       
            }
        }

        far = pdr->far;
        ogs_assert(far);

        handle_gpdu_prepare(m);

        if (far->dst_if == OGS_PFCP_INTERFACE_CORE) {
            return process_dst_if_interface_core(lconf, m, in_l3_head);
        } else if (far->dst_if == OGS_PFCP_INTERFACE_ACCESS) {
            return process_dst_if_interface_access(lconf, m, pdr);
        } else if (far->dst_if == OGS_PFCP_INTERFACE_CP_FUNCTION) {
            return process_dst_if_interface_cp_func(lconf, m, pdr);
        } else {
            ogs_fatal("Not implemented : FAR-DST_IF[%d]", far->dst_if);
            return -1;
        }
    } else if (gtp_h->type == OGS_GTPU_MSGTYPE_ECHO_REQ) {
        if (gtp_handle_echo_req(m, gtp_h)) {
            send_single_packet(lconf, m->port, m);
        }
        return 0;
    } else if (gtp_h->type == OGS_GTPU_MSGTYPE_END_MARKER ||
            gtp_h->type == OGS_GTPU_MSGTYPE_ERR_IND) {
        /* Nothing */
        return -1;
    } else {
        //dispatcher send to pfcp, never reached;
        ogs_debug("[DROP] Invalid GTPU Type [%d]", gtp_h->type);
        return -1;
    }

    return 0;
}

static int32_t handle_n6_ipip_pkt(struct lcore_conf *lconf, struct rte_mbuf *m)
{
	struct packet *pkt = (struct packet *)(m->buf_addr);
    uint32_t ring = 0;
	
	int decrease = pkt->l2_len + pkt->l3_len - sizeof(struct rte_ether_hdr);
	m->data_off += decrease;
	m->data_len -= decrease;
	m->pkt_len -= decrease;
	struct rte_ether_hdr *eth_h = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

	pkt->l2_len = sizeof(struct rte_ether_hdr);
	struct ip *ip_h = rte_pktmbuf_mtod_offset(m, struct ip *, pkt->l2_len);
	struct rte_ipv4_hdr *in_ipv4_h = (struct rte_ipv4_hdr *)ip_h;
	if (UNLIKELY(ip_h->ip_v == 6)) {
		//pkt->is_ipv4 = 0;
		//get_ipv6_proto((struct rte_ipv6_hdr *)ip_h, &pkt->l3_len);
		//eth_h->ether_type = BE_ETH_P_IPV6;
	} else {
		pkt->is_ipv4 = 1;
		pkt->l3_len = (((struct rte_ipv4_hdr *)ip_h)->version_ihl & 0x0f) * 4;
		eth_h->ether_type = BE_ETH_P_IP;
	}
	pkt->l4_len = 0;
	pkt->tunnel_len = 0;

	pkt->pkt_type = PKT_TYPE_IP_N6;
    ring = ntohl(in_ipv4_h->dst_addr) % dkuf.fwd_num;
    if (-ENOBUFS == rte_ring_sp_enqueue(dkuf.lconf[dkuf.fwd_lcore[ring]].f2f_ring, m)) {
        ogs_error("%s, to %s enqueue f2fring failed\n", __func__, ip2str(in_ipv4_h->dst_addr));
        return -1;
    }
    lconf->lstat.f2f_enqueue++;
	return 0;
	
}


static int32_t handle_n6_pkt(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    struct packet *pkt = (struct packet *)(m->buf_addr);
    char *l3_head = rte_pktmbuf_mtod_offset(m, char *, pkt->l2_len);

    upf_sess_t *sess = NULL;
    ogs_pfcp_pdr_t *pdr = NULL;

    struct ip *ip_h = (struct ip *)l3_head;
    if(ip_h->ip_v == 4)
    {
        struct rte_ipv4_hdr *in_ipv4_h = (struct rte_ipv4_hdr *)ip_h;
        if(in_ipv4_h->next_proto_id == IPPROTO_IPIP)
        {
            handle_n6_ipip_pkt(lconf, m);
            return 0;
        }
    }
    sess = local_sess_find_by_ue_ip(lconf, l3_head, 1);
    if (!sess) {
        ogs_debug("%s, unmatch session by ip %s\n", __func__, ip_printf(l3_head, 1));
        lconf->lstat.sess_unmatch[1]++;
        return -1;
    }
    if(sess->bnbr)
    {
        //ogs_error("%s, destination ue %s is belong to nbr 0x%x\n", __func__, ip_printf(l3_head, 1), sess->nbraddr);
        //Ŀ��ue������������ע��
        if(send_packet_to_nbr_ipip(lconf, m, sess->nbraddr) < 0)
        {
            return -1;
			
        }
		return 0;
    }
    pdr = n6_pdr_find_by_local_sess(sess, l3_head);
    if (!pdr) {
        ogs_error("%s, unfound pdr by local session, ip %s\n", __func__, ip_printf(l3_head, 1));
        if (ogs_global_conf()->parameter.multicast) {
            return upf_gtp_handle_multicast(m);
        }
        return -1;
    }
    lconf->lstat.sess_match[1]++;



    //判断如果要支持vxlan隧道,则封装vxlan头
    //NEW: IP头20字节+UDP头8+vxlan头8+内层mac地址14 
    if (sess->support_vxlan_flag){
        int ret;
        ret = add_vxlan_header(sess, m);
        
        if (ret <= 0){
            m = make_vxlan_arp_request(sess->local_interface_address,sess->remote_interface_address,sess->ipv4->subnet->gw.sub[0],sess->ipv4->addr[0]);
        }
    }

    uint8_t downlink_data_report = 0;
    if (pfcp_up_handle_pdr(pdr, m, &downlink_data_report) < 0) {
        return -1;
    }

    if (downlink_data_report) {
        ogs_assert(pdr->sess);
        sess = UPF_SESS(pdr->sess);
        ogs_assert(sess);

        ogs_debug("%s, pkt first buffered, reports downlink notifications.\n", __func__);
        lconf->lstat.sess_report[m->port]++;
        fwd_handle_gtp_session_report(lconf->f2p_ring, pdr, *sess->upf_n4_seid_node);
    }

    return 0;
}

static int32_t handle_arp_vxlan(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    struct packet *pkt = (struct packet *)(m->buf_addr);
    char *l3_head = rte_pktmbuf_mtod_offset(m, char *, pkt->l2_len);

    upf_sess_t *sess = NULL;
    ogs_pfcp_pdr_t *pdr = NULL;
    ogs_info("handle_arp_vxlan, pkt->l2_len:%d",pkt->l2_len);
    sess = local_sess_find_by_ue_ip(lconf, l3_head, 0);
    if (!sess) {
        ogs_debug("%s, unmatch session by ip %s\n", __func__, ip_printf(l3_head, 1));
        lconf->lstat.sess_unmatch[1]++;
        return -1;
    }

    pdr = n6_pdr_find_by_local_sess(sess, l3_head);
    if (!pdr) {
        ogs_error("%s, unfound pdr by local session, ip %s\n", __func__, ip_printf(l3_head, 1));
        if (ogs_global_conf()->parameter.multicast) {
            return upf_gtp_handle_multicast(m);
        }
        return -1;
    }
    lconf->lstat.sess_match[1]++;

    uint8_t downlink_data_report = 0;
    if (pfcp_up_handle_pdr(pdr, m, &downlink_data_report) < 0) {
        return -1;
    }

    if (downlink_data_report) {
        ogs_assert(pdr->sess);
        sess = UPF_SESS(pdr->sess);
        ogs_assert(sess);

        ogs_debug("%s, pkt first buffered, reports downlink notifications.\n", __func__);
        lconf->lstat.sess_report[m->port]++;
        fwd_handle_gtp_session_report(lconf->f2p_ring, pdr, *sess->upf_n4_seid_node);
    }

    return 0;
}

static int handle_pkt(struct lcore_conf *lconf, struct rte_mbuf *m)
{
    int ret = 0;
    struct packet *pkt = (struct packet *)(m->buf_addr);

    ogs_debug("%s, pkt type %d\n", __func__, pkt->pkt_type);

    switch (pkt->pkt_type) {
        case PKT_TYPE_ARP:
            ret = handle_arp(lconf, m);
            break;
        case PKT_TYPE_ND:
            ret = handle_nd(lconf, m);
            break;
        case PKT_TYPE_PING:
            ret = handle_ping(lconf, m);
            break;
        case PKT_TYPE_PING6:
            ret = handle_ping6(lconf, m);
            break;
        case PKT_TYPE_IP_N3:
            ret = handle_n3_pkt(lconf, m);
            break;
        case PKT_TYPE_IP_N6:
            ret = handle_n6_pkt(lconf, m);
            break;
        case PKT_TYPE_ARP_VXLAN:
            ret = handle_arp_vxlan(lconf, m);
            break;
        default:
            return -1;
    }

    return ret;
}

static void handle_pkts(struct lcore_conf *lconf, struct rte_mbuf **pkts_burst, uint16_t nb_rx)
{
    int32_t j;

#define PREFETCH_OFFSET   3
    for (j = 0; j < PREFETCH_OFFSET && j < nb_rx; j++) {
        rte_prefetch0(rte_pktmbuf_mtod(pkts_burst[j], void *));
    }

    for (j = 0; j < (nb_rx - PREFETCH_OFFSET); j++) {
        rte_prefetch0(rte_pktmbuf_mtod(pkts_burst[j + PREFETCH_OFFSET], void *));
        if (handle_pkt(lconf, pkts_burst[j]) < 0) {
            rte_pktmbuf_free(pkts_burst[j]);
        }
    }

    for (; j < nb_rx; j++) {
        if (handle_pkt(lconf, pkts_burst[j]) < 0) {
            rte_pktmbuf_free(pkts_burst[j]);
        }
    }
}

static void free_local_framed_route_from_trie(struct lcore_conf *lconf, ogs_ipsubnet_t *route)
{
    const int chunk_size = sizeof(route->sub[0]) << 3;
    const int is_ipv4 = route->family == AF_INET;
    const int nbits = is_ipv4 ? chunk_size : OGS_IPV6_128_PREFIX_LEN;
    struct upf_route_trie_node **trie =
        is_ipv4 ? &lconf->ipv4_framed_routes : &lconf->ipv6_framed_routes;

    struct upf_route_trie_node **to_free_tries[OGS_IPV6_128_PREFIX_LEN + 1];
    int free_from = 0;
    int i = 0;

    for (i = 0; i <= nbits; i++) {
        int part = i / chunk_size;
        int bit = (nbits - i - 1) % chunk_size;

        if (!*trie)
            break;
        to_free_tries[i] = trie;

        if (i == nbits ||
            ((1 << bit) & be32toh(route->mask[part])) == 0) {
            (*trie)->sess = NULL;
            if ((*trie)->left || (*trie)->right)
                free_from = i + 1;
            i++;
            break;
        }

        if ((1 << bit) & be32toh(route->sub[part])) {
            if ((*trie)->left || (*trie)->sess)
                free_from = i + 1;
            trie = &(*trie)->right;
        } else {
            if ((*trie)->right || (*trie)->sess)
                free_from = i + 1;
            trie = &(*trie)->left;
        }
    }

    for (i = i - 1; i >= free_from; i--) {
        trie = to_free_tries[i];
        ogs_free(*trie);
        *trie = NULL;
    }
}

static void add_local_framed_route_to_trie(struct lcore_conf *lconf, ogs_ipsubnet_t *route, upf_sess_t *sess)
{
    const int chunk_size = sizeof(route->sub[0]) << 3;
    const int is_ipv4 = route->family == AF_INET;
    const int nbits = is_ipv4 ? chunk_size : OGS_IPV6_128_PREFIX_LEN;
    struct upf_route_trie_node **trie =
        is_ipv4 ? &lconf->ipv4_framed_routes : &lconf->ipv6_framed_routes;
    int i = 0;

    for (i = 0; i <= nbits; i++) {
        int part = i / chunk_size;
        int bit = (nbits - i - 1) % chunk_size;

        if (!*trie)
            *trie = ogs_calloc(1, sizeof(**trie));

        if (i == nbits ||
            ((1 << bit) & be32toh(route->mask[part])) == 0) {
            (*trie)->sess = sess;
            break;
        }

        if ((1 << bit) & be32toh(route->sub[part])) {
            trie = &(*trie)->right;
        } else {
            trie = &(*trie)->left;
        }
    }
}

static int ipv4_framed_routes_remove(struct lcore_conf *lconf, upf_sess_t *sess){
    int i = 0;
    
    for (i = 0; i < OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI; i++) {
        if (!sess->ipv4_framed_routes || !sess->ipv4_framed_routes[i].family)
            break;
        free_local_framed_route_from_trie(lconf, &sess->ipv4_framed_routes[i]);
    }
    
    return 0;
}

static int ipv4_framed_routes_add(struct lcore_conf *lconf, upf_sess_t *sess){
    int i = 0;
    
    ogs_assert(sess);

    for (i = 0; i < OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI; i++) {
        if (!sess->ipv4_framed_routes || !sess->ipv4_framed_routes[i].family)
            break;
        add_local_framed_route_to_trie(lconf, &sess->ipv4_framed_routes[i], sess);
    }

    return 0;
}

static int upf_local_sess_add(struct lcore_conf *lconf, upf_sess_t *sess)
{
    upf_sess_t *old = NULL;

    old = ipv4_sess_find(lconf->ipv4_hash, sess->ipv4->addr[0]);
    if (old) {
        ipv4_hash_remove(lconf->ipv4_hash, sess->ipv4->addr[0]);
        ipv4_framed_routes_remove(lconf, sess);
        fwd_flush_buffered_packet(old);
        free_upf_dpdk_sess(old);
    }

    ipv4_hash_insert(lconf->ipv4_hash, sess->ipv4->addr[0], sess);
    ipv4_framed_routes_add(lconf, sess);
    return 0;
}

static int upf_local_sess_update(struct lcore_conf *lconf, upf_sess_t *sess)
{
    upf_sess_t *old = NULL;

    old = ipv4_sess_find(lconf->ipv4_hash, sess->ipv4->addr[0]);
    if (old) {
        ipv4_hash_remove(lconf->ipv4_hash, sess->ipv4->addr[0]);
        ipv4_framed_routes_remove(lconf, sess);
        fwd_flush_buffered_packet(old);
        free_upf_dpdk_sess(old);
    }
    ipv4_hash_insert(lconf->ipv4_hash, sess->ipv4->addr[0], sess);
    ipv4_framed_routes_add(lconf, sess);
    return 0;
}

static int upf_local_sess_del(struct lcore_conf *lconf, void *body)
{
    uint32_t ip = (uint32_t)(uintptr_t)body;
    ogs_debug("forward local sess try to delete ipv4= "FORMAT_IPV4, FORMAT_IPV4_ARGS(ip));
    upf_sess_t *sess = NULL;

    sess = ipv4_sess_find(lconf->ipv4_hash, ip);
    if (sess) {
        fwd_flush_buffered_packet(sess);
        ipv4_hash_remove(lconf->ipv4_hash, sess->ipv4->addr[0]);
        ipv4_framed_routes_remove(lconf, sess);
        ogs_debug("forward local sess found and delete ipv4= "FORMAT_IPV4, FORMAT_IPV4_ARGS(ip));
        free_upf_dpdk_sess(sess);
    }

    return 0;
}




static int upf_local_nbr_handle(struct lcore_conf *lconf, upf_nbr_message_t *nbrmessage)
{
    upf_sess_t *old = NULL;
	upf_sess_t *new_sess = NULL;
	upf_sess_t *sess = NULL;
    uint16_t loop;
	//ogs_error("%s, optype %d\n", __func__, nbrmessage->optype);
	//ogs_error("%s, serveraddr 0x%x\n", __func__, nbrmessage->serveraddr);
	//ogs_error("%s, uenum %d\n", __func__, nbrmessage->uenum);
	//ogs_error("%s, coreid %d\n", __func__, lconf->lcore);
	//增加nbr ue
	if(nbrmessage->optype == 1)
	{
		for(loop = 0; loop < nbrmessage->uenum; loop++)
		{
		    ogs_error("loop %d, addr 0x%x\n", loop, nbrmessage->addr[loop]);
		    old = ipv4_sess_find(lconf->ipv4_hash, nbrmessage->addr[loop]);

			if (old)
			{
		        ipv4_hash_remove(lconf->ipv4_hash, nbrmessage->addr[loop]);
		        fwd_flush_buffered_packet(old);
		        free_upf_dpdk_sess(old);
		    }
			new_sess = dpdk_malloc(sizeof(upf_sess_t));
			if(!new_sess)
			{
			    ogs_error("dpdk_malloc failed\n");
			    continue;
			}
			new_sess->bnbr = 1;
			new_sess->nbraddr = nbrmessage->serveraddr;
			ogs_error("upf_local_nbr_handle ipaddr 0x%x\n", nbrmessage->addr[loop]);
			ipv4_hash_insert(lconf->ipv4_hash, nbrmessage->addr[loop], new_sess);
		}
	}
    else if(nbrmessage->optype == 2)
    {
        //删除ue
        for(loop = 0; loop < nbrmessage->uenum; loop++)
		{
		    old = ipv4_sess_find(lconf->ipv4_hash, nbrmessage->addr[loop]);

			if (old)
			{
			    if(old->nbraddr == nbrmessage->serveraddr)
			    {
			        ipv4_hash_remove(lconf->ipv4_hash, nbrmessage->addr[loop]);
			        fwd_flush_buffered_packet(old);
			        free_upf_dpdk_sess(old);
			    }
		    }
		}
    }
	else if(nbrmessage->optype == 3)
	{
	    //remote nbr server lost,delete all nbrserver ue
	    ipv4_hash_remove_nbrservaddr(lconf->ipv4_hash, nbrmessage->serveraddr);
	}
    return 0;
}


static int handle_event(struct lcore_conf *lconf, upf_dpdk_event_t *event)
{
    int ret = 0;

    ogs_debug("%s, event type %d\n", __func__, event->event_type);
    switch (event->event_type) {
        case UPF_DPDK_SESS_ESTAB:
            ret = upf_local_sess_add(lconf, event->event_body);
            break;
        case UPF_DPDK_SESS_MOD:
            ret = upf_local_sess_update(lconf, event->event_body);
            break;
        case UPF_DPDK_SESS_DEL:
            ret = upf_local_sess_del(lconf, event->event_body);
            break;
        case UPF_DPDK_NBR_MSG:
            upf_local_nbr_handle(lconf, event->event_body);
	    break;
        default:
            return -1;
    }

    return ret;
}

static void
handle_events(struct lcore_conf *lconf, upf_dpdk_event_t **events, uint16_t nb_rx)
{
    int32_t i;

    for (i = 0; i < nb_rx; i++) {
        if (handle_event(lconf, events[i]) < 0) {
            free_upf_dpdk_sess(events[i]->event_body);
        }
        dpdk_free(events[i]);
    }
}

int fwd_main_loop(void *arg)
{
    unsigned lcore_id = rte_lcore_id();
    struct lcore_conf *lconf = &dkuf.lconf[lcore_id];

    uint16_t queueid = lconf->tx_queue;
    uint64_t last_ms = dkuf.sys_up_ms;

    char thread_name[32];
    sprintf(thread_name, "forward%u_core_%d", lconf->tx_queue, lcore_id);
    prctl(PR_SET_NAME, thread_name);
    set_highest_priority();

    //ogs_info("%s lcoreid %u queueid %u\n", __func__, lcore_id, queueid);

    int i = 0;
    int tmp_size = 0;
    int dpt_num = dkuf.dpt_num;
    int dequeue_size = MIN(RX_PROCESS_SIZE / (dpt_num + 1), RX_BURST_SIZE);
    int16_t nb_rx = 0, nb_event = 0;
    struct rte_mbuf *bufs[RX_PROCESS_SIZE];
    upf_dpdk_event_t *events[RX_EVENT_SIZE];

    while (!dkuf.stopFlag) {
        if ((nb_rx < dequeue_size) || (dkuf.sys_up_ms - last_ms > 10)) {
            send_burst(lconf);
            last_ms = dkuf.sys_up_ms;
            tw_timer_trigger(lconf->twl, dkuf.sys_up_ms);
        }

        nb_event = rte_ring_dequeue_burst(lconf->p2f_ring, (void **)events, RX_EVENT_SIZE, NULL);
        if (nb_event > 0) {
            lconf->lstat.rx_event += nb_event;
            handle_events(lconf, events, nb_event);
        }

        nb_rx = 0;
        for (i = 0; i < dpt_num; i++) {
            tmp_size = rte_ring_dequeue_burst(lconf->d2f_ring[i], (void **)&bufs[nb_rx], dequeue_size, NULL);
            if (tmp_size > 0) {
                nb_rx += tmp_size;
            }
        }
        lconf->lstat.dequeue += nb_rx;
        tmp_size = rte_ring_dequeue_burst(lconf->f2f_ring, (void **)&bufs[nb_rx], dequeue_size, NULL);
        if (tmp_size > 0) {
            nb_rx += tmp_size;
            lconf->lstat.f2f_dequeue += tmp_size;
        }
        //ogs_info("lcore %d queue %d recv cnt %d\n", lcore_id, queueid, nb_rx);
        handle_pkts(lconf, bufs, nb_rx);
    }
    /* never reached */
    /* pthread_exit(NULL); */
    return 0;
}

