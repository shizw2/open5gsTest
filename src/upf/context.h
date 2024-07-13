/*
 * Copyright (C) 2019-2023 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UPF_CONTEXT_H
#define UPF_CONTEXT_H

#include "upf-config.h"

#if HAVE_NET_IF_H
#include <net/if.h>
#endif

#include "ogs-gtp.h"
#include "ogs-pfcp.h"
#include "ogs-app.h"
#include "ogs-sctp.h"
#include "nbr-sctp.h"
#include "ipfw/ogs-ipfw.h"

#include "timer.h"
#include "upf-sm.h"
#include "metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int __upf_log_domain;

#undef OGS_LOG_DOMAIN
#define OGS_LOG_DOMAIN __upf_log_domain

struct upf_route_trie_node;

typedef struct upf_context_s {
    ogs_hash_t *upf_n4_seid_hash;   /* hash table (UPF-N4-SEID) */
    ogs_hash_t *smf_n4_seid_hash;   /* hash table (SMF-N4-SEID) */
    ogs_hash_t *smf_n4_f_seid_hash; /* hash table (SMF-N4-F-SEID) */
    ogs_hash_t *ipv4_hash;  /* hash table (IPv4 Address) */
    struct ipv4_hashtbl *nbr_ipv4_hash;
    ogs_hash_t *ipv6_hash;  /* hash table (IPv6 Address) */
    ogs_hash_t *frag_hash;  /* hash table (frag key) */
    /* IPv4 framed routes trie */
    struct upf_route_trie_node *ipv4_framed_routes;
    /* IPv6 framed routes trie */
    struct upf_route_trie_node *ipv6_framed_routes;

    ogs_list_t sess_list;

    ogs_list_t      nbrlocalserver_list;
    ogs_list_t      nbrlocalclient_list;
    ogs_list_t      nbrremoteserver_list;

    ogs_list_t      remoteclient_list;
    ogs_hash_t      *remoteclient_addr_hash;
    uint16_t        nbrlocalserver_port;

    ogs_list_t      remoteserver_list;
    ogs_hash_t      *remoteserver_addr_hash;
    uint16_t        nbrremoteserver_port;
    ogs_timer_t    *nbr_timer;
    uint16_t        clientportbegin;

    int             nbr_rawsocket;
    ogs_poll_t      *nbr_rawpoll;
} upf_context_t;

/* trie mapping from IP framed routes to session. */
struct upf_route_trie_node {
    struct upf_route_trie_node *left;
    struct upf_route_trie_node *right;
    upf_sess_t *sess;
};


typedef struct upf_remoteclient_s {
    ogs_lnode_t     lnode;

    ogs_sctp_sock_t sctp;       /* SCTP socket */



    uint16_t        max_num_of_ostreams;/* SCTP Max num of outbound streams */
    uint16_t        ostream_id;         /* gnb_ostream_id generator */


} upf_remoteclient_t;

typedef struct upf_remoteserver_s {
    ogs_lnode_t     lnode;

    ogs_sctp_sock_t sctp;       /* SCTP socket */



    uint16_t        max_num_of_ostreams;/* SCTP Max num of outbound streams */
    uint16_t        ostream_id;         /* gnb_ostream_id generator */


} upf_remoteserver_t;

/* Accounting: */
typedef struct upf_sess_urr_acc_s {
    bool reporting_enabled;
    ogs_timer_t *t_validity_time; /* Quota Validity Time expiration handler */
    ogs_timer_t *t_time_quota; /* Time Quota expiration handler */
    ogs_timer_t *t_time_threshold; /* Time Threshold expiration handler */
    uint32_t time_start; /* When t_time_* started */
    ogs_pfcp_urr_ur_seqn_t report_seqn; /* Next seqn to use when reporting */
    uint64_t total_octets;
    uint64_t ul_octets;
    uint64_t dl_octets;
    uint64_t total_pkts;
    uint64_t ul_pkts;
    uint64_t dl_pkts;
    ogs_time_t time_of_first_packet;
    ogs_time_t time_of_last_packet;
    /* Snapshot of measurement when last report was sent: */
    struct {
        uint64_t total_octets;
        uint64_t ul_octets;
        uint64_t dl_octets;
        uint64_t total_pkts;
        uint64_t ul_pkts;
        uint64_t dl_pkts;
        ogs_time_t timestamp;
    } last_report;
} upf_sess_urr_acc_t;

#define UPF_SESS(pfcp_sess) ogs_container_of(pfcp_sess, upf_sess_t, pfcp)
typedef struct upf_sess_s {
    ogs_lnode_t     lnode;
    ogs_pool_id_t   id;
    ogs_pool_id_t   *upf_n4_seid_node;  /* A node of UPF-N4-SEID */

    ogs_pfcp_sess_t pfcp;

    uint64_t        upf_n4_seid;        /* UPF SEID is dervied from NODE */
    struct {
        uint64_t    seid;
        ogs_ip_t    ip;
    } smf_n4_f_seid;                    /* SMF SEID is received from Peer */

    /* APN Configuration */
    ogs_pfcp_ue_ip_t *ipv4;
    ogs_pfcp_ue_ip_t *ipv6;

    ogs_ipsubnet_t   *ipv4_framed_routes;
    ogs_ipsubnet_t   *ipv6_framed_routes;

    char            *gx_sid;            /* Gx Session ID */ /* DPDK_UP_NO_USE */
    ogs_pfcp_node_t *pfcp_node;         /* DPDK_UP_NO_USE */

    /* Accounting: */
    upf_sess_urr_acc_t urr_acc[OGS_MAX_NUM_OF_URR]; /* FIXME: This probably needs to be mved to a hashtable or alike */
    char            *apn_dnn;            /* APN/DNN Item */
    uint8_t          bnbr;
    uint32_t         nbraddr;
} upf_sess_t;

typedef struct upf_nbr_message_s {
	uint32_t        serveraddr;
	uint8_t         optype;
	uint16_t        uenum;
	uint32_t        addr[200];
} upf_nbr_message_t;
void upf_context_init(void);
void upf_context_final(void);
upf_context_t *upf_self(void);

int upf_context_parse_config(void);

upf_sess_t *upf_sess_add_by_message(ogs_pfcp_message_t *message);

upf_sess_t *upf_sess_add(ogs_pfcp_f_seid_t *f_seid);
int upf_sess_remove(upf_sess_t *sess);
void upf_sess_remove_all(void);
upf_sess_t *upf_sess_find_by_smf_n4_seid(uint64_t seid);
upf_sess_t *upf_sess_find_by_smf_n4_f_seid(ogs_pfcp_f_seid_t *f_seid);
upf_sess_t *upf_sess_find_by_upf_n4_seid(uint64_t seid);
upf_sess_t *upf_sess_find_by_ipv4(uint32_t addr);
upf_sess_t *upf_sess_find_by_ipv6(uint32_t *addr6);
upf_sess_t *upf_sess_find_by_id(ogs_pool_id_t id);

uint8_t upf_sess_set_ue_ip(upf_sess_t *sess,
        uint8_t session_type, ogs_pfcp_pdr_t *pdr);
uint8_t upf_sess_set_ue_ipv4_framed_routes(upf_sess_t *sess,
        char *framed_routes[]);
uint8_t upf_sess_set_ue_ipv6_framed_routes(upf_sess_t *sess,
        char *framed_routes[]);

void upf_sess_urr_acc_add(upf_sess_t *sess, ogs_pfcp_urr_t *urr, size_t size, bool is_uplink);
void upf_sess_urr_acc_fill_usage_report(upf_sess_t *sess, const ogs_pfcp_urr_t *urr,
                                        ogs_pfcp_user_plane_report_t *report, unsigned int idx);
void upf_sess_urr_acc_snapshot(upf_sess_t *sess, ogs_pfcp_urr_t *urr);
void upf_sess_urr_acc_timers_setup(upf_sess_t *sess, ogs_pfcp_urr_t *urr);

int upf_context_parse_nbr_config(void);
upf_remoteclient_t *upf_remoteclient_find_by_addr(ogs_sockaddr_t *addr);
upf_remoteclient_t *upf_remoteclient_add(ogs_sock_t *sock, ogs_sockaddr_t *addr);

void upf_remoteclient_remove(upf_remoteclient_t *remoteclient);
void upf_remoteclient_remove_all(void);

upf_remoteserver_t *upf_remoteserver_find_by_addr(ogs_sockaddr_t *addr);
upf_remoteserver_t *upf_remoteserver_add(ogs_sock_t *sock, ogs_sockaddr_t *addr);

void upf_remoteserver_remove(upf_remoteserver_t *remoteserver);
void upf_remoteserver_remove_all(void);

void upf_send_singlelocalueip_to_nbrclient(uint32_t ueaddr);
void upf_handle_remoteserver_nbrmessage(upf_nbr_message_t *nbrmessage, uint16_t len);
upf_remoteserver_t *upf_remoteserver_find_by_remoteclient_ipaddr(uint32_t ipaddr);
void upf_send_singlelocalueip_to_nbrclient_del(uint32_t ueaddr);
void upf_send_alllocalueip_to_newnbrclient(upf_remoteclient_t *remoteclient);
void upf_handle_remoteserver_lost(uint32_t nbrserveraddr);
int ipv4_hash_remove_nbrservaddr(struct ipv4_hashtbl *h, uint32_t ip);
void ipv4_hash_destroy_include_sess(struct ipv4_hashtbl *h);
#ifdef __cplusplus
}
#endif

#endif /* UPF_CONTEXT_H */
