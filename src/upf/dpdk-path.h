#ifndef _DPDK_PATH_H_
#define _DPDK_PATH_H_
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
#include <ctrl-path.h>
#include <dpdk-common.h>
#include <rte_ring.h>

// 定义vxlan头部结构
struct vxlan_header {
    uint8_t flags;    // 8位标志位，包含I和F标志以及6位保留位
    uint8_t rsvd[3]; // 3个字节的保留位
    uint32_t vni;    // 24位VXLAN网络标识符
} __attribute__((__packed__));

// 定义vxlan_tunnel结构
struct vxlan_tunnel_header {
    struct rte_ipv4_hdr ip; 
    struct rte_udp_hdr udp;
    struct vxlan_header vxlan;
    struct rte_ether_hdr ether;   
} __attribute__((aligned(2)));

uint8_t ue_addr_match(uint32_t ip);
uint8_t ue_addr6_match(void *ip);
uint32_t get_n3_addr(void);
uint8_t *get_n3_addr6(void);

struct rte_mbuf *gtp_handle_echo_req(struct rte_mbuf *m, ogs_gtp2_header_t *gtph);
upf_sess_t *local_sess_find_by_ue_ip(struct lcore_conf *lconf, char *l3_head, uint8_t dst);
int gtp_send_user_plane(ogs_gtp_node_t *gnode, ogs_gtp2_header_t *gtp_hdesc,
        ogs_gtp2_extension_header_t *ext_hdesc, struct rte_mbuf *m);

int pfcp_send_g_pdu(ogs_pfcp_pdr_t *pdr, struct rte_mbuf *m);
int pfcp_up_handle_pdr(ogs_pfcp_pdr_t *pdr, struct rte_mbuf *m, uint8_t *downlink_data_report);

ogs_pfcp_rule_t *pfcp_pdr_rule_find_by_packet(ogs_pfcp_pdr_t *pdr, char *l3_head);

int fwd_handle_gtp_session_report(struct rte_ring *r, ogs_pfcp_pdr_t *pdr, uint32_t sess_index);

ogs_pfcp_pdr_t *n3_pdr_find_by_local_sess(upf_sess_t *sess, ogs_gtp2_header_t *gtp_h, char *ip_h, uint8_t tunnel_len);
int process_dst_if_interface_core(struct lcore_conf *lconf, struct rte_mbuf *m, char *in_l3_head);
int process_dst_if_interface_access(struct lcore_conf *lconf, struct rte_mbuf *m, ogs_pfcp_pdr_t *pdr);
int process_dst_if_interface_cp_func(struct lcore_conf *lconf, struct rte_mbuf *m, ogs_pfcp_pdr_t *pdr);

void handle_gpdu_prepare(struct rte_mbuf *m);
int upf_gtp_handle_multicast(struct rte_mbuf *m);
ogs_pfcp_pdr_t *n6_pdr_find_by_local_sess(upf_sess_t *sess, char *l3_head);

void fwd_flush_buffered_packet(upf_sess_t *sess);

int fwd_handle_volume_session_report(struct rte_ring *r, ogs_pfcp_pdr_t *pdr, upf_sess_t *sess, uint8_t type_volume);
int send_packet_to_nbr(struct lcore_conf *lconf, struct rte_mbuf *m, uint32_t nbraddr);
int send_packet_to_nbr_ipip(struct lcore_conf *lconf, struct rte_mbuf *m, uint32_t nbraddr);
int add_vxlan_header(upf_sess_t *sess, struct rte_mbuf *m/*, char *ptr,struct packet *pkt */);
struct rte_mbuf * make_vxlan_arp_request(upf_sess_t *sess);
#endif

