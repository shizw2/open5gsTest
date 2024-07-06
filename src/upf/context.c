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

#include "context.h"
#include "pfcp-path.h"
#include "ogs-app-timer.h"
#if defined(USE_DPDK)
#include "upf-dpdk.h"
#include "ctrl-path.h"
#else
#include <assert.h>
#include "hash.h"
#endif

static upf_context_t self;

int __upf_log_domain;

static OGS_POOL(upf_sess_pool, upf_sess_t);
static OGS_POOL(upf_n4_seid_pool, ogs_pool_id_t);
static OGS_POOL(upf_remoteclient_pool, upf_remoteclient_t);
static OGS_POOL(upf_remoteserver_pool, upf_remoteserver_t);
static int context_initialized = 0;

static void upf_sess_urr_acc_remove_all(upf_sess_t *sess);

void upf_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize UPF context */
    memset(&self, 0, sizeof(upf_context_t));

    ogs_log_install_domain(&__upf_log_domain, "upf", ogs_core()->log.level);

    /* Setup UP Function Features */
    ogs_pfcp_self()->up_function_features.ftup = 1;
    ogs_pfcp_self()->up_function_features.empu = 1;
    ogs_pfcp_self()->up_function_features.mnop = 1;
    ogs_pfcp_self()->up_function_features.vtime = 1;
    ogs_pfcp_self()->up_function_features.frrt = 1;
    ogs_pfcp_self()->up_function_features_len = 4;

    ogs_list_init(&self.sess_list);
    ogs_pool_init(&upf_sess_pool, ogs_app()->pool.sess);
    ogs_pool_init(&upf_n4_seid_pool, ogs_app()->pool.sess);
    ogs_pool_random_id_generate(&upf_n4_seid_pool);

    self.upf_n4_seid_hash = ogs_hash_make();
    ogs_assert(self.upf_n4_seid_hash);
    self.smf_n4_seid_hash = ogs_hash_make();
    ogs_assert(self.smf_n4_seid_hash);
    self.smf_n4_f_seid_hash = ogs_hash_make();
    ogs_assert(self.smf_n4_f_seid_hash);
    self.ipv4_hash = ogs_hash_make();
    ogs_assert(self.ipv4_hash);
    self.ipv6_hash = ogs_hash_make();
    ogs_assert(self.ipv6_hash);

    self.nbr_ipv4_hash = ipv4_hash_create(1024);
    ogs_assert(self.nbr_ipv4_hash);

    ogs_list_init(&self.nbrlocalserver_list);
    ogs_list_init(&self.nbrlocalclient_list);
    ogs_list_init(&self.nbrremoteserver_list);

    ogs_list_init(&self.remoteclient_list);
    self.remoteclient_addr_hash = ogs_hash_make();
    ogs_assert(self.remoteclient_addr_hash);
    ogs_pool_init(&upf_remoteclient_pool, 10);

    ogs_list_init(&self.remoteserver_list);
    self.remoteserver_addr_hash = ogs_hash_make();
    ogs_assert(self.remoteserver_addr_hash);
    ogs_pool_init(&upf_remoteserver_pool, 10);
    context_initialized = 1;
}

static void free_upf_route_trie_node(struct upf_route_trie_node *node)
{
    if (!node)
        return;
    free_upf_route_trie_node(node->left);
    free_upf_route_trie_node(node->right);
    ogs_free(node);
}

void upf_context_final(void)
{
    ogs_assert(context_initialized == 1);

    upf_sess_remove_all();

    upf_remoteclient_remove_all();
    upf_remoteserver_remove_all();

    ogs_assert(self.upf_n4_seid_hash);
    ogs_hash_destroy(self.upf_n4_seid_hash);
    ogs_assert(self.smf_n4_seid_hash);
    ogs_hash_destroy(self.smf_n4_seid_hash);
    ogs_assert(self.smf_n4_f_seid_hash);
    ogs_hash_destroy(self.smf_n4_f_seid_hash);
    ogs_assert(self.ipv4_hash);
    ogs_hash_destroy(self.ipv4_hash);
    ogs_assert(self.ipv6_hash);
    ogs_hash_destroy(self.ipv6_hash);
    ogs_assert(self.nbr_ipv4_hash);
    ipv4_hash_destroy(self.nbr_ipv4_hash);
    ogs_assert(self.remoteclient_addr_hash);
    ogs_hash_destroy(self.remoteclient_addr_hash);
    ogs_assert(self.remoteserver_addr_hash);
    ogs_hash_destroy(self.remoteserver_addr_hash);

    free_upf_route_trie_node(self.ipv4_framed_routes);
    free_upf_route_trie_node(self.ipv6_framed_routes);

    ogs_pool_final(&upf_sess_pool);
    ogs_pool_final(&upf_n4_seid_pool);

    ogs_pool_final(&upf_remoteclient_pool);
    ogs_pool_final(&upf_remoteserver_pool);
    context_initialized = 0;
}

upf_context_t *upf_self(void)
{
    return &self;
}

static int upf_context_prepare(void)
{
    return OGS_OK;
}

static int upf_context_validation(void)
{
    if (ogs_list_first(&ogs_gtp_self()->gtpu_list) == NULL) {
        ogs_error("No upf.gtpu.address in '%s'", ogs_app()->file);
        return OGS_ERROR;
    }
    if (ogs_list_first(&ogs_pfcp_self()->subnet_list) == NULL) {
        ogs_error("No upf.session.subnet: in '%s'", ogs_app()->file);
        return OGS_ERROR;
    }
    return OGS_OK;
}

int upf_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    rv = upf_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "upf")) {
            ogs_yaml_iter_t upf_iter;
            ogs_yaml_iter_recurse(&root_iter, &upf_iter);
            while (ogs_yaml_iter_next(&upf_iter)) {
                const char *upf_key = ogs_yaml_iter_key(&upf_iter);
                ogs_assert(upf_key);
                if (!strcmp(upf_key, "gtpu")) {
                    /* handle config in gtp library */
                } else if (!strcmp(upf_key, "pfcp")) {
                    /* handle config in pfcp library */
                } else if (!strcmp(upf_key, "smf")) {
                    /* handle config in pfcp library */
                } else if (!strcmp(upf_key, "session")) {
                    /* handle config in pfcp library */
                } else if (!strcmp(upf_key, "metrics")) {
                    /* handle config in metrics library */
                } else if (!strcmp(upf_key, "dpdk")) {
                    /* handle config in dpdk library */
                } else if (!strcmp(upf_key, "nbrlocalserver")) {
                    /* handle config in nbr function */
                } else if (!strcmp(upf_key, "nbrremoteserver")) {
                    /* handle config in nbr function */
                } else
                    ogs_warn("unknown key `%s`", upf_key);
            }
        }
    }

    rv = upf_context_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}

upf_sess_t *upf_sess_add(ogs_pfcp_f_seid_t *cp_f_seid)
{
    upf_sess_t *sess = NULL;

    ogs_assert(cp_f_seid);

    ogs_pool_alloc(&upf_sess_pool, &sess);
    ogs_assert(sess);
    memset(sess, 0, sizeof *sess);

    ogs_pfcp_pool_init(&sess->pfcp);

    /* Set UPF-N4-SEID */
    ogs_pool_alloc(&upf_n4_seid_pool, &sess->upf_n4_seid_node);
    ogs_assert(sess->upf_n4_seid_node);

    sess->upf_n4_seid = *(sess->upf_n4_seid_node);

    ogs_hash_set(self.upf_n4_seid_hash, &sess->upf_n4_seid,
            sizeof(sess->upf_n4_seid), sess);

    /* Since F-SEID is composed of ogs_ip_t and uint64-seid,
     * all these values must be put into the structure-smf_n4_f_seid
     * before creating hash */
    sess->smf_n4_f_seid.seid = cp_f_seid->seid;
    ogs_assert(OGS_OK ==
            ogs_pfcp_f_seid_to_ip(cp_f_seid, &sess->smf_n4_f_seid.ip));

    ogs_hash_set(self.smf_n4_f_seid_hash, &sess->smf_n4_f_seid,
            sizeof(sess->smf_n4_f_seid), sess);
    ogs_hash_set(self.smf_n4_seid_hash, &sess->smf_n4_f_seid.seid,
            sizeof(sess->smf_n4_f_seid.seid), sess);

    ogs_list_add(&self.sess_list, sess);
    upf_metrics_inst_global_inc(UPF_METR_GLOB_GAUGE_UPF_SESSIONNBR);

    ogs_info("[Added] Number of UPF-Sessions is now %d",
            ogs_list_count(&self.sess_list));

    return sess;
}

int upf_sess_remove(upf_sess_t *sess)
{
    ogs_assert(sess);

    upf_sess_urr_acc_remove_all(sess);

    ogs_list_remove(&self.sess_list, sess);
    ogs_pfcp_sess_clear(&sess->pfcp);

    ogs_hash_set(self.upf_n4_seid_hash, &sess->upf_n4_seid,
            sizeof(sess->upf_n4_seid), NULL);

    ogs_hash_set(self.smf_n4_seid_hash, &sess->smf_n4_f_seid.seid,
            sizeof(sess->smf_n4_f_seid.seid), NULL);
    ogs_hash_set(self.smf_n4_f_seid_hash, &sess->smf_n4_f_seid,
            sizeof(sess->smf_n4_f_seid), NULL);

    if (sess->ipv4) {
        ogs_hash_set(self.ipv4_hash, sess->ipv4->addr, OGS_IPV4_LEN, NULL);
        ogs_pfcp_ue_ip_free(sess->ipv4);
    }
    if (sess->ipv6) {
        ogs_hash_set(self.ipv6_hash,
                sess->ipv6->addr, OGS_IPV6_DEFAULT_PREFIX_LEN >> 3, NULL);
        ogs_pfcp_ue_ip_free(sess->ipv6);
    }

    upf_sess_set_ue_ipv4_framed_routes(sess, NULL);
    upf_sess_set_ue_ipv6_framed_routes(sess, NULL);

    ogs_pfcp_pool_final(&sess->pfcp);

    ogs_pool_free(&upf_n4_seid_pool, sess->upf_n4_seid_node);
    ogs_pool_free(&upf_sess_pool, sess);
    if (sess->apn_dnn)
        ogs_free(sess->apn_dnn);
    upf_metrics_inst_global_dec(UPF_METR_GLOB_GAUGE_UPF_SESSIONNBR);

    ogs_info("[Removed] Number of UPF-sessions is now %d",
            ogs_list_count(&self.sess_list));

    return OGS_OK;
}

void upf_sess_remove_all(void)
{
    upf_sess_t *sess = NULL, *next = NULL;

    ogs_list_for_each_safe(&self.sess_list, next, sess) {
        upf_sess_remove(sess);
    }
}

upf_sess_t *upf_sess_find_by_smf_n4_seid(uint64_t seid)
{
    return ogs_hash_get(self.smf_n4_seid_hash, &seid, sizeof(seid));
}

upf_sess_t *upf_sess_find_by_smf_n4_f_seid(ogs_pfcp_f_seid_t *f_seid)
{
    struct {
        uint64_t seid;
        ogs_ip_t ip;
    } key;

    ogs_assert(f_seid);
    ogs_assert(OGS_OK == ogs_pfcp_f_seid_to_ip(f_seid, &key.ip));
    key.seid = f_seid->seid;

    return ogs_hash_get(self.smf_n4_f_seid_hash, &key, sizeof(key));
}

upf_sess_t *upf_sess_find_by_upf_n4_seid(uint64_t seid)
{
    return ogs_hash_get(self.upf_n4_seid_hash, &seid, sizeof(seid));
}

upf_sess_t *upf_sess_find_by_ipv4(uint32_t addr)
{
    upf_sess_t *ret;
    struct upf_route_trie_node *trie = self.ipv4_framed_routes;
    const int nbits = sizeof(addr) << 3;
    int i;

    ogs_assert(self.ipv4_hash);

    ret = ogs_hash_get(self.ipv4_hash, &addr, OGS_IPV4_LEN);
    if (ret)
        return ret;

    for (i =  0; i <= nbits; i++) {
        int bit = nbits - i - 1;

        if (!trie)
            break;
        if (trie->sess)
            ret = trie->sess;
        if (i == nbits)
            break;

        if ((1 << bit) & be32toh(addr))
            trie = trie->right;
        else
            trie = trie->left;
    }
    return ret;
}

upf_sess_t *upf_sess_find_by_ipv6(uint32_t *addr6)
{
    upf_sess_t *ret = NULL;
    struct upf_route_trie_node *trie = self.ipv6_framed_routes;
    int i;
    const int chunk_size = sizeof(*addr6) << 3;

    ogs_assert(self.ipv6_hash);
    ogs_assert(addr6);
    ret = ogs_hash_get(
            self.ipv6_hash, addr6, OGS_IPV6_DEFAULT_PREFIX_LEN >> 3);
    if (ret)
        return ret;

    for (i = 0; i <= OGS_IPV6_128_PREFIX_LEN; i++) {
        int part = i / chunk_size;
        int bit = (OGS_IPV6_128_PREFIX_LEN - i - 1) % chunk_size;

        if (!trie)
            break;
        if (trie->sess)
            ret = trie->sess;
        if (i == OGS_IPV6_128_PREFIX_LEN)
            break;

        if ((1 << bit) & be32toh(addr6[part]))
            trie = trie->right;
        else
            trie = trie->left;
    }
    return ret;
}

upf_sess_t *upf_sess_add_by_message(ogs_pfcp_message_t *message)
{
    upf_sess_t *sess = NULL;
    ogs_pfcp_f_seid_t *f_seid = NULL;

    ogs_pfcp_session_establishment_request_t *req =
        &message->pfcp_session_establishment_request;;

    f_seid = req->cp_f_seid.data;
    if (req->cp_f_seid.presence == 0 || f_seid == NULL) {
        ogs_error("No CP F-SEID");
        return NULL;
    }
    f_seid->seid = be64toh(f_seid->seid);

    sess = upf_sess_find_by_smf_n4_f_seid(f_seid);
    if (!sess) {
        sess = upf_sess_add(f_seid);
        if (!sess) {
            ogs_error("No Session Context");
            return NULL;
        }
    }
    ogs_assert(sess);

    return sess;
}

uint8_t upf_sess_set_ue_ip(upf_sess_t *sess,
        uint8_t session_type, ogs_pfcp_pdr_t *pdr)
{
    ogs_pfcp_ue_ip_addr_t *ue_ip = NULL;
    char buf1[OGS_ADDRSTRLEN];
    char buf2[OGS_ADDRSTRLEN];

    uint8_t cause_value = OGS_PFCP_CAUSE_REQUEST_ACCEPTED;

    ogs_assert(sess);
    ogs_assert(session_type);
    ogs_assert(pdr);
    ogs_assert(pdr->ue_ip_addr_len);
    ue_ip = &pdr->ue_ip_addr;
    ogs_assert(ue_ip);

    if (sess->ipv4) {
        ogs_hash_set(self.ipv4_hash,
                sess->ipv4->addr, OGS_IPV4_LEN, NULL);
        ogs_pfcp_ue_ip_free(sess->ipv4);
    }
    if (sess->ipv6) {
        ogs_hash_set(self.ipv6_hash,
                sess->ipv6->addr, OGS_IPV6_DEFAULT_PREFIX_LEN >> 3, NULL);
        ogs_pfcp_ue_ip_free(sess->ipv6);
    }

    /* Set PDN-Type and UE IP Address */
    if (session_type == OGS_PDU_SESSION_TYPE_IPV4) {
        if (ue_ip->ipv4 || pdr->dnn) {
            sess->ipv4 = ogs_pfcp_ue_ip_alloc(&cause_value, AF_INET,
                            pdr->dnn, (uint8_t *)&(ue_ip->addr));
            if (!sess->ipv4) {
                ogs_error("ogs_pfcp_ue_ip_alloc() failed[%d]", cause_value);
                ogs_assert(cause_value != OGS_PFCP_CAUSE_REQUEST_ACCEPTED);
                return cause_value;
            }
            ogs_hash_set(self.ipv4_hash, sess->ipv4->addr, OGS_IPV4_LEN, sess);
	    upf_send_singlelocalueip_to_nbrclient((uint32_t)ue_ip->addr);
        } else {
            ogs_warn("Cannot support PDN-Type[%d], [IPv4:%d IPv6:%d DNN:%s]",
                session_type, ue_ip->ipv4, ue_ip->ipv6,
                pdr->dnn ? pdr->dnn : "");
        }
    } else if (session_type == OGS_PDU_SESSION_TYPE_IPV6) {
        if (ue_ip->ipv6 || pdr->dnn) {
            sess->ipv6 = ogs_pfcp_ue_ip_alloc(&cause_value, AF_INET6,
                            pdr->dnn, ue_ip->addr6);
            if (!sess->ipv6) {
                ogs_error("ogs_pfcp_ue_ip_alloc() failed[%d]", cause_value);
                ogs_assert(cause_value != OGS_PFCP_CAUSE_REQUEST_ACCEPTED);
                return cause_value;
            }
            ogs_hash_set(self.ipv6_hash, sess->ipv6->addr,
                    OGS_IPV6_DEFAULT_PREFIX_LEN >> 3, sess);
        } else {
            ogs_warn("Cannot support PDN-Type[%d], [IPv4:%d IPv6:%d DNN:%s]",
                session_type, ue_ip->ipv4, ue_ip->ipv6,
                pdr->dnn ? pdr->dnn : "");
        }
    } else if (session_type == OGS_PDU_SESSION_TYPE_IPV4V6) {
        if (ue_ip->ipv4 || pdr->dnn) {
            sess->ipv4 = ogs_pfcp_ue_ip_alloc(&cause_value, AF_INET,
                            pdr->dnn, (uint8_t *)&(ue_ip->both.addr));
            if (!sess->ipv4) {
                ogs_error("ogs_pfcp_ue_ip_alloc() failed[%d]", cause_value);
                ogs_assert(cause_value != OGS_PFCP_CAUSE_REQUEST_ACCEPTED);
                return cause_value;
            }
            ogs_hash_set(self.ipv4_hash, sess->ipv4->addr, OGS_IPV4_LEN, sess);
        } else {
            ogs_warn("Cannot support PDN-Type[%d], [IPv4:%d IPv6:%d DNN:%s]",
                session_type, ue_ip->ipv4, ue_ip->ipv6,
                pdr->dnn ? pdr->dnn : "");
        }

        if (ue_ip->ipv6 || pdr->dnn) {
            sess->ipv6 = ogs_pfcp_ue_ip_alloc(&cause_value, AF_INET6,
                            pdr->dnn, ue_ip->both.addr6);
            if (!sess->ipv6) {
                ogs_error("ogs_pfcp_ue_ip_alloc() failed[%d]", cause_value);
                ogs_assert(cause_value != OGS_PFCP_CAUSE_REQUEST_ACCEPTED);
                if (sess->ipv4) {
                    ogs_hash_set(self.ipv4_hash,
                            sess->ipv4->addr, OGS_IPV4_LEN, NULL);
                    ogs_pfcp_ue_ip_free(sess->ipv4);
                    sess->ipv4 = NULL;
                }
                return cause_value;
            }
            ogs_hash_set(self.ipv6_hash, sess->ipv6->addr,
                    OGS_IPV6_DEFAULT_PREFIX_LEN >> 3, sess);
        } else {
            ogs_warn("Cannot support PDN-Type[%d], [IPv4:%d IPv6:%d DNN:%s]",
                session_type, ue_ip->ipv4, ue_ip->ipv6,
                pdr->dnn ? pdr->dnn : "");
        }
    } else {
        ogs_warn("Cannot support PDN-Type[%d], [IPv4:%d IPv6:%d DNN:%s]",
                session_type, ue_ip->ipv4, ue_ip->ipv6,
                pdr->dnn ? pdr->dnn : "");
    }

    ogs_info("UE F-SEID[UP:0x%lx CP:0x%lx] "
             "APN[%s] PDN-Type[%d] IPv4[%s] IPv6[%s]",
        (long)sess->upf_n4_seid, (long)sess->smf_n4_f_seid.seid,
        pdr->dnn, session_type,
        sess->ipv4 ? OGS_INET_NTOP(&sess->ipv4->addr, buf1) : "",
        sess->ipv6 ? OGS_INET6_NTOP(&sess->ipv6->addr, buf2) : "");

    return cause_value;
}

/* Remove amd free framed ROUTE from TRIE. It isn't an error if the framed
   route doesn't exist in TRIE. */
static void free_framed_route_from_trie(ogs_ipsubnet_t *route)
{
    const int chunk_size = sizeof(route->sub[0]) << 3;
    const int is_ipv4 = route->family == AF_INET;
    const int nbits = is_ipv4 ? chunk_size : OGS_IPV6_128_PREFIX_LEN;
    struct upf_route_trie_node **trie =
        is_ipv4 ? &self.ipv4_framed_routes : &self.ipv6_framed_routes;

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

static void add_framed_route_to_trie(ogs_ipsubnet_t *route, upf_sess_t *sess)
{
    const int chunk_size = sizeof(route->sub[0]) << 3;
    const int is_ipv4 = route->family == AF_INET;
    const int nbits = is_ipv4 ? chunk_size : OGS_IPV6_128_PREFIX_LEN;
    struct upf_route_trie_node **trie =
        is_ipv4 ? &self.ipv4_framed_routes : &self.ipv6_framed_routes;
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

static int parse_framed_route(ogs_ipsubnet_t *subnet, const char *framed_route)
{
    char *mask = ogs_strdup(framed_route);
    char *addr = strsep(&mask, "/");
    int rv;

    rv = ogs_ipsubnet(subnet, addr, mask);
    ogs_free(addr);
    return rv;
}

uint8_t upf_sess_set_ue_ipv4_framed_routes(upf_sess_t *sess,
        char *framed_routes[])
{
    int i = 0, j = 0, rv;
    uint8_t cause_value = OGS_PFCP_CAUSE_REQUEST_ACCEPTED;

    ogs_assert(sess);

    for (i = 0; i < OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI; i++) {
        if (!sess->ipv4_framed_routes || !sess->ipv4_framed_routes[i].family)
            break;
        free_framed_route_from_trie(&sess->ipv4_framed_routes[i]);
        memset(&sess->ipv4_framed_routes[i], 0,
               sizeof(sess->ipv4_framed_routes[i]));
    }

    for (i = 0, j = 0; i < OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI; i++) {
        if (!framed_routes || !framed_routes[i])
            break;

        if (sess->ipv4_framed_routes == NULL) {
            sess->ipv4_framed_routes = ogs_calloc(
                OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI, sizeof(ogs_ipsubnet_t));
            ogs_assert(sess->ipv4_framed_routes);
        }

        rv = parse_framed_route(&sess->ipv4_framed_routes[j], framed_routes[i]);

        if (rv != OGS_OK) {
            ogs_warn("Ignoring invalid framed route %s", framed_routes[i]);
            memset(&sess->ipv4_framed_routes[j], 0,
                   sizeof(sess->ipv4_framed_routes[j]));
            continue;
        }
        add_framed_route_to_trie(&sess->ipv4_framed_routes[j], sess);
        j++;
    }
    if (j == 0 && sess->ipv4_framed_routes) {
        ogs_free(sess->ipv4_framed_routes);
        sess->ipv4_framed_routes = NULL;
    }

    return cause_value;
}

uint8_t upf_sess_set_ue_ipv6_framed_routes(upf_sess_t *sess,
        char *framed_routes[])
{
    int i = 0, j = 0, rv;
    uint8_t cause_value = OGS_PFCP_CAUSE_REQUEST_ACCEPTED;

    ogs_assert(sess);

    for (i = 0; i < OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI; i++) {
        if (!sess->ipv6_framed_routes || !sess->ipv6_framed_routes[i].family)
            break;
        free_framed_route_from_trie(&sess->ipv6_framed_routes[i]);
    }

    for (i = 0, j = 0; i < OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI; i++) {
        if (!framed_routes || !framed_routes[i])
            break;

        if (sess->ipv6_framed_routes == NULL) {
            sess->ipv6_framed_routes = ogs_calloc(
                OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI, sizeof(ogs_ipsubnet_t));
            ogs_assert(sess->ipv6_framed_routes);
        }

        rv = parse_framed_route(&sess->ipv6_framed_routes[j], framed_routes[i]);

        if (rv != OGS_OK) {
            ogs_warn("Ignoring invalid framed route %s", framed_routes[i]);
            memset(&sess->ipv6_framed_routes[j], 0,
                   sizeof(sess->ipv6_framed_routes[j]));
            continue;
        }
        add_framed_route_to_trie(&sess->ipv6_framed_routes[j], sess);
        j++;
    }
    if (j == 0 && sess->ipv6_framed_routes) {
        ogs_free(sess->ipv6_framed_routes);
        sess->ipv6_framed_routes = NULL;
    }

    return cause_value;
}

void upf_sess_urr_acc_add(upf_sess_t *sess, ogs_pfcp_urr_t *urr, size_t size, bool is_uplink)
{
    upf_sess_urr_acc_t *urr_acc = &sess->urr_acc[urr->id];
    uint64_t vol;

    /* Increment total & ul octets + pkts */
    urr_acc->total_octets += size;
    urr_acc->total_pkts++;
    if (is_uplink) {
        urr_acc->ul_octets += size;
        urr_acc->ul_pkts++;
    } else {
        urr_acc->dl_octets += size;
        urr_acc->dl_pkts++;
    }

    urr_acc->time_of_last_packet = ogs_time_now();
    if (urr_acc->time_of_first_packet == 0)
        urr_acc->time_of_first_packet = urr_acc->time_of_last_packet;

    /* generate report if volume threshold/quota is reached */
    vol = urr_acc->total_octets - urr_acc->last_report.total_octets;
    if ((urr->rep_triggers.volume_quota && urr->vol_quota.tovol && vol >= urr->vol_quota.total_volume) ||
        (urr->rep_triggers.volume_threshold && urr->vol_threshold.tovol && vol >= urr->vol_threshold.total_volume)) {
        ogs_pfcp_user_plane_report_t report;
        memset(&report, 0, sizeof(report));
        upf_sess_urr_acc_fill_usage_report(sess, urr, &report, 0);
        report.num_of_usage_report = 1;
        upf_sess_urr_acc_snapshot(sess, urr);

        ogs_assert(OGS_OK ==
            upf_pfcp_send_session_report_request(sess, &report));
        /* Start new report period/iteration: */
        upf_sess_urr_acc_timers_setup(sess, urr);
    }
}

/* report struct must be memzeroed before first use of this function.
 * report->num_of_usage_report must be set by the caller */
void upf_sess_urr_acc_fill_usage_report(upf_sess_t *sess, const ogs_pfcp_urr_t *urr,
                                  ogs_pfcp_user_plane_report_t *report, unsigned int idx)
{
    upf_sess_urr_acc_t *urr_acc = &sess->urr_acc[urr->id];
    ogs_time_t last_report_timestamp;
    ogs_time_t now;

    now = ogs_time_now(); /* we need UTC for start_time and end_time */

    if (urr_acc->last_report.timestamp)
        last_report_timestamp = urr_acc->last_report.timestamp;
    else
        last_report_timestamp = ogs_time_from_ntp32(urr_acc->time_start);

    report->type.usage_report = 1;
    report->usage_report[idx].id = urr->id;
    report->usage_report[idx].seqn = urr_acc->report_seqn++;
    report->usage_report[idx].start_time = urr_acc->time_start;
    report->usage_report[idx].end_time = ogs_time_to_ntp32(now);
    report->usage_report[idx].vol_measurement = (ogs_pfcp_volume_measurement_t){
        .dlnop = 1,
        .ulnop = 1,
        .tonop = 1,
        .dlvol = 1,
        .ulvol = 1,
        .tovol = 1,
        .total_volume = urr_acc->total_octets - urr_acc->last_report.total_octets,
        .uplink_volume = urr_acc->ul_octets - urr_acc->last_report.ul_octets,
        .downlink_volume = urr_acc->dl_octets - urr_acc->last_report.dl_octets,
        .total_n_packets = urr_acc->total_pkts - urr_acc->last_report.total_pkts,
        .uplink_n_packets = urr_acc->ul_pkts - urr_acc->last_report.ul_pkts,
        .downlink_n_packets = urr_acc->dl_pkts - urr_acc->last_report.dl_pkts,
    };
    if (now >= last_report_timestamp)
        report->usage_report[idx].dur_measurement = ((now - last_report_timestamp) + (OGS_USEC_PER_SEC/2)) / OGS_USEC_PER_SEC; /* FIXME: should use MONOTONIC here */
    /* else memset sets it to 0 */
    report->usage_report[idx].time_of_first_packet = ogs_time_to_ntp32(urr_acc->time_of_first_packet); /* TODO: First since last report? */
    report->usage_report[idx].time_of_last_packet = ogs_time_to_ntp32(urr_acc->time_of_last_packet);

    /* Time triggers: */
    if (urr->quota_validity_time > 0 &&
            report->usage_report[idx].dur_measurement >= urr->quota_validity_time)
        report->usage_report[idx].rep_trigger.quota_validity_time = 1;
    if (urr->time_quota > 0 &&
            report->usage_report[idx].dur_measurement >= urr->time_quota)
        report->usage_report[idx].rep_trigger.time_quota = 1;
    if (urr->time_threshold > 0 &&
            report->usage_report[idx].dur_measurement >= urr->time_threshold)
        report->usage_report[idx].rep_trigger.time_threshold = 1;

    /* Volume triggers: */
    if (urr->rep_triggers.volume_quota && urr->vol_quota.tovol &&
            report->usage_report[idx].vol_measurement.total_volume >= urr->vol_quota.total_volume)
        report->usage_report[idx].rep_trigger.volume_quota = 1;
    if (urr->rep_triggers.volume_threshold && urr->vol_threshold.tovol &&
            report->usage_report[idx].vol_measurement.total_volume >= urr->vol_threshold.total_volume)
        report->usage_report[idx].rep_trigger.volume_threshold = 1;
}

void upf_sess_urr_acc_snapshot(upf_sess_t *sess, ogs_pfcp_urr_t *urr)
{
    upf_sess_urr_acc_t *urr_acc = &sess->urr_acc[urr->id];
    urr_acc->last_report.total_octets = urr_acc->total_octets;
    urr_acc->last_report.dl_octets = urr_acc->dl_octets;
    urr_acc->last_report.ul_octets = urr_acc->ul_octets;
    urr_acc->last_report.total_pkts = urr_acc->total_pkts;
    urr_acc->last_report.dl_pkts = urr_acc->dl_pkts;
    urr_acc->last_report.ul_pkts = urr_acc->ul_pkts;
    urr_acc->last_report.timestamp = ogs_time_now();
}

static void upf_sess_urr_acc_timers_cb(void *data)
{
    ogs_pfcp_urr_t *urr = (ogs_pfcp_urr_t *)data;
    ogs_pfcp_user_plane_report_t report;
    ogs_pfcp_sess_t *pfcp_sess = urr->sess;
    upf_sess_t *sess = UPF_SESS(pfcp_sess);

    ogs_info("upf_time_threshold_cb() triggered! urr=%p", urr);

    if (urr->rep_triggers.quota_validity_time ||
        urr->rep_triggers.time_quota ||
        urr->rep_triggers.time_threshold) {
        memset(&report, 0, sizeof(report));
        upf_sess_urr_acc_fill_usage_report(sess, urr, &report, 0);
        report.num_of_usage_report = 1;
        upf_sess_urr_acc_snapshot(sess, urr);

        ogs_assert(OGS_OK ==
            upf_pfcp_send_session_report_request(sess, &report));
    }
    /* Start new report period/iteration: */
    upf_sess_urr_acc_timers_setup(sess, urr);
}

static void upf_sess_urr_acc_validity_time_setup(upf_sess_t *sess, ogs_pfcp_urr_t *urr)
{
    upf_sess_urr_acc_t *urr_acc = &sess->urr_acc[urr->id];

    ogs_debug("Installing URR Quota Validity Time timer");
    urr_acc->reporting_enabled = true;
    if (!urr_acc->t_validity_time)
        urr_acc->t_validity_time = ogs_timer_add(ogs_app()->timer_mgr,
                                        upf_sess_urr_acc_timers_cb, urr);
    ogs_timer_start(urr_acc->t_validity_time,
            ogs_time_from_sec(urr->quota_validity_time));
}

static void upf_sess_urr_acc_time_quota_setup(upf_sess_t *sess, ogs_pfcp_urr_t *urr)
{
    upf_sess_urr_acc_t *urr_acc = &sess->urr_acc[urr->id];

    ogs_debug("Installing URR Time Quota timer");
    urr_acc->reporting_enabled = true;
    if (!urr_acc->t_time_quota)
        urr_acc->t_time_quota = ogs_timer_add(ogs_app()->timer_mgr,
                                        upf_sess_urr_acc_timers_cb, urr);
    ogs_timer_start(urr_acc->t_time_quota, ogs_time_from_sec(urr->time_quota));
}

static void upf_sess_urr_acc_time_threshold_setup(upf_sess_t *sess, ogs_pfcp_urr_t *urr)
{
    upf_sess_urr_acc_t *urr_acc = &sess->urr_acc[urr->id];

    ogs_debug("Installing URR Time Threshold timer");
    urr_acc->reporting_enabled = true;
    if (!urr_acc->t_time_threshold)
        urr_acc->t_time_threshold = ogs_timer_add(ogs_app()->timer_mgr,
                                        upf_sess_urr_acc_timers_cb, urr);
    ogs_timer_start(urr_acc->t_time_threshold,
            ogs_time_from_sec(urr->time_threshold));
}

void upf_sess_urr_acc_timers_setup(upf_sess_t *sess, ogs_pfcp_urr_t *urr)
{
    upf_sess_urr_acc_t *urr_acc = &sess->urr_acc[urr->id];
    urr_acc->time_start = ogs_time_ntp32_now();
    if (urr->rep_triggers.quota_validity_time && urr->quota_validity_time > 0)
        upf_sess_urr_acc_validity_time_setup(sess, urr);
    if (urr->rep_triggers.time_quota && urr->time_quota > 0)
        upf_sess_urr_acc_time_quota_setup(sess, urr);
    if (urr->rep_triggers.time_threshold && urr->time_threshold > 0)
        upf_sess_urr_acc_time_threshold_setup(sess, urr);
}

static void upf_sess_urr_acc_remove_all(upf_sess_t *sess)
{
    unsigned int i;
    for (i = 0; i < OGS_ARRAY_SIZE(sess->urr_acc); i++) {
        if (sess->urr_acc[i].t_time_threshold) {
            ogs_timer_delete(sess->urr_acc[i].t_time_threshold);
            sess->urr_acc[i].t_time_threshold = NULL;
        }
        if (sess->urr_acc[i].t_validity_time) {
            ogs_timer_delete(sess->urr_acc[i].t_validity_time);
            sess->urr_acc[i].t_validity_time = NULL;
        }
        if (sess->urr_acc[i].t_time_quota) {
            ogs_timer_delete(sess->urr_acc[i].t_time_quota);
            sess->urr_acc[i].t_time_quota = NULL;
        }
    }
}

int yaml_check_proc(void)
{
    int rv;    
    //ogs_info("check yaml config.");

    if (!osg_app_is_config_modified()) {
        return 0;//如果文件未修改,则返回
    }

    ogs_info("yaml config file changed,reloading......");

    //0、读配置
    rv = ogs_app_config_read();
    if (rv != OGS_OK) return rv;

    //1、app级配置    
    rv = ogs_app_context_parse_config();
    if (rv != OGS_OK) return rv;

    //1.1重新设置日志级别
    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) return rv;
    
    return 0;
}

int upf_context_parse_nbr_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    rv = upf_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "upf")) {
            ogs_yaml_iter_t upf_iter;
            ogs_yaml_iter_recurse(&root_iter, &upf_iter);
            while (ogs_yaml_iter_next(&upf_iter)) {
                const char *upf_key = ogs_yaml_iter_key(&upf_iter);
                ogs_assert(upf_key);
                if (!strcmp(upf_key, "nbrlocalserver")) {
                    ogs_yaml_iter_t nbrlocalserver_array, nbrlocalserver_iter;
                    ogs_yaml_iter_recurse(&upf_iter, &nbrlocalserver_array);
                    do {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[OGS_MAX_NUM_OF_HOSTNAME];
                        uint16_t port = self.nbrlocalserver_port;
                        const char *dev = NULL;
                        ogs_sockaddr_t *addr = NULL;

                        if (ogs_yaml_iter_type(&nbrlocalserver_array) ==
                                YAML_MAPPING_NODE) {
                            memcpy(&nbrlocalserver_iter, &nbrlocalserver_array,
                                    sizeof(ogs_yaml_iter_t));
                        } else if (ogs_yaml_iter_type(&nbrlocalserver_array) ==
                            YAML_SEQUENCE_NODE) {
                            if (!ogs_yaml_iter_next(&nbrlocalserver_array))
                                break;
                            ogs_yaml_iter_recurse(&nbrlocalserver_array, &nbrlocalserver_iter);
                        } else if (ogs_yaml_iter_type(&nbrlocalserver_array) ==
                            YAML_SCALAR_NODE) {
                            break;
                        } else
                            ogs_assert_if_reached();

                        while (ogs_yaml_iter_next(&nbrlocalserver_iter)) {
                            const char *nbrlocalserver_key =
                                ogs_yaml_iter_key(&nbrlocalserver_iter);
                            ogs_assert(nbrlocalserver_key);
                            if (!strcmp(nbrlocalserver_key, "family")) {
                                const char *v = ogs_yaml_iter_value(&nbrlocalserver_iter);
                                if (v) family = atoi(v);
                                if (family != AF_UNSPEC &&
                                    family != AF_INET && family != AF_INET6) {
                                    ogs_warn("Ignore family(%d) : "
                                        "AF_UNSPEC(%d), "
                                        "AF_INET(%d), AF_INET6(%d) ",
                                        family, AF_UNSPEC, AF_INET, AF_INET6);
                                    family = AF_UNSPEC;
                                }
                            } else if (!strcmp(nbrlocalserver_key, "addr") ||
                                    !strcmp(nbrlocalserver_key, "name")) {
                                ogs_yaml_iter_t hostname_iter;
                                ogs_yaml_iter_recurse(
                                        &nbrlocalserver_iter, &hostname_iter);
                                ogs_assert(ogs_yaml_iter_type(&hostname_iter) !=
                                    YAML_MAPPING_NODE);

                                do {
                                    if (ogs_yaml_iter_type(&hostname_iter) ==
                                            YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    ogs_assert(num < OGS_MAX_NUM_OF_HOSTNAME);
                                    hostname[num++] =
                                        ogs_yaml_iter_value(&hostname_iter);
                                } while (
                                    ogs_yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE);
                            } else if (!strcmp(nbrlocalserver_key, "port")) {
                                const char *v = ogs_yaml_iter_value(&nbrlocalserver_iter);
                                if (v) port = atoi(v);
                            } else if (!strcmp(nbrlocalserver_key, "dev")) {
                                dev = ogs_yaml_iter_value(&nbrlocalserver_iter);
                            } else
                                ogs_warn("unknown key `%s`", nbrlocalserver_key);
                        }

                        addr = NULL;
                        for (i = 0; i < num; i++) {
                            rv = ogs_addaddrinfo(&addr,
                                    family, hostname[i], port, 0);
                            ogs_assert(rv == OGS_OK);
                        }

                        if (addr) {
                            ogs_socknode_add(
                                        &self.nbrlocalserver_list, AF_INET, addr, NULL);

                            ogs_freeaddrinfo(addr);
                        }

                    } while (ogs_yaml_iter_type(&nbrlocalserver_array) ==
                            YAML_SEQUENCE_NODE);
                }
				else if (!strcmp(upf_key, "nbrlocalclient")) {
                    ogs_yaml_iter_t nbrlocalclient_array, nbrlocalclient_iter;
                    ogs_yaml_iter_recurse(&upf_iter, &nbrlocalclient_array);
                    do {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[OGS_MAX_NUM_OF_HOSTNAME];
                        uint16_t port = 0;
                        const char *dev = NULL;
                        ogs_sockaddr_t *addr = NULL;

                        if (ogs_yaml_iter_type(&nbrlocalclient_array) ==
                                YAML_MAPPING_NODE) {
                            memcpy(&nbrlocalclient_iter, &nbrlocalclient_array,
                                    sizeof(ogs_yaml_iter_t));
                        } else if (ogs_yaml_iter_type(&nbrlocalclient_array) ==
                            YAML_SEQUENCE_NODE) {
                            if (!ogs_yaml_iter_next(&nbrlocalclient_array))
                                break;
                            ogs_yaml_iter_recurse(&nbrlocalclient_array, &nbrlocalclient_iter);
                        } else if (ogs_yaml_iter_type(&nbrlocalclient_array) ==
                            YAML_SCALAR_NODE) {
                            break;
                        } else
                            ogs_assert_if_reached();

                        while (ogs_yaml_iter_next(&nbrlocalclient_iter)) {
                            const char *nbrlocalclient_key =
                                ogs_yaml_iter_key(&nbrlocalclient_iter);
                            ogs_assert(nbrlocalclient_key);
                            if (!strcmp(nbrlocalclient_key, "family")) {
                                const char *v = ogs_yaml_iter_value(&nbrlocalclient_iter);
                                if (v) family = atoi(v);
                                if (family != AF_UNSPEC &&
                                    family != AF_INET && family != AF_INET6) {
                                    ogs_warn("Ignore family(%d) : "
                                        "AF_UNSPEC(%d), "
                                        "AF_INET(%d), AF_INET6(%d) ",
                                        family, AF_UNSPEC, AF_INET, AF_INET6);
                                    family = AF_UNSPEC;
                                }
                            } else if (!strcmp(nbrlocalclient_key, "addr") ||
                                    !strcmp(nbrlocalclient_key, "name")) {
                                ogs_yaml_iter_t hostname_iter;
                                ogs_yaml_iter_recurse(
                                        &nbrlocalclient_iter, &hostname_iter);
                                ogs_assert(ogs_yaml_iter_type(&hostname_iter) !=
                                    YAML_MAPPING_NODE);

                                do {
                                    if (ogs_yaml_iter_type(&hostname_iter) ==
                                            YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    ogs_assert(num < OGS_MAX_NUM_OF_HOSTNAME);
                                    hostname[num++] =
                                        ogs_yaml_iter_value(&hostname_iter);
                                } while (
                                    ogs_yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE);
                            } else if (!strcmp(nbrlocalclient_key, "port")) {
                                const char *v = ogs_yaml_iter_value(&nbrlocalclient_iter);
                                if (v)
								{
								    port = atoi(v);
									self.clientportbegin = port;
                                }
                            } else if (!strcmp(nbrlocalclient_key, "dev")) {
                                dev = ogs_yaml_iter_value(&nbrlocalclient_iter);
                            } else
                                ogs_warn("unknown key `%s`", nbrlocalclient_key);
                        }

                        addr = NULL;
                        for (i = 0; i < num; i++) {
                            rv = ogs_addaddrinfo(&addr,
                                    family, hostname[i], port, 0);
                            ogs_assert(rv == OGS_OK);
                        }

                        if (addr) {
                                ogs_socknode_add(
                                        &self.nbrlocalclient_list, AF_INET, addr, NULL);

                            ogs_freeaddrinfo(addr);
                        }

                    } while (ogs_yaml_iter_type(&nbrlocalclient_array) ==
                            YAML_SEQUENCE_NODE);
                }
				else if(!strcmp(upf_key, "nbrremoteserver")){
                    ogs_yaml_iter_t nbrremoteserver_array, nbrremoteserver_iter;
                    ogs_yaml_iter_recurse(&upf_iter, &nbrremoteserver_array);
                    do {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[OGS_MAX_NUM_OF_HOSTNAME];
                        uint16_t port;
                        const char *dev = NULL;
                        ogs_sockaddr_t *addr = NULL;

                        if (ogs_yaml_iter_type(&nbrremoteserver_array) ==
                                YAML_MAPPING_NODE) {
                            memcpy(&nbrremoteserver_iter, &nbrremoteserver_array,
                                    sizeof(ogs_yaml_iter_t));
                        } else if (ogs_yaml_iter_type(&nbrremoteserver_array) ==
                            YAML_SEQUENCE_NODE) {
                            if (!ogs_yaml_iter_next(&nbrremoteserver_array))
                                break;
                            ogs_yaml_iter_recurse(&nbrremoteserver_array, &nbrremoteserver_iter);
                        } else if (ogs_yaml_iter_type(&nbrremoteserver_array) ==
                            YAML_SCALAR_NODE) {
                            break;
                        } else
                            ogs_assert_if_reached();

                        while (ogs_yaml_iter_next(&nbrremoteserver_iter)) {
                            const char *nbrremoteserver_key =
                                ogs_yaml_iter_key(&nbrremoteserver_iter);
                            ogs_assert(nbrremoteserver_key);
                            if (!strcmp(nbrremoteserver_key, "family")) {
                                const char *v = ogs_yaml_iter_value(&nbrremoteserver_iter);
                                if (v) family = atoi(v);
                                if (family != AF_UNSPEC &&
                                    family != AF_INET && family != AF_INET6) {
                                    ogs_warn("Ignore family(%d) : "
                                        "AF_UNSPEC(%d), "
                                        "AF_INET(%d), AF_INET6(%d) ",
                                        family, AF_UNSPEC, AF_INET, AF_INET6);
                                    family = AF_UNSPEC;
                                }
                            } else if (!strcmp(nbrremoteserver_key, "addr") ||
                                    !strcmp(nbrremoteserver_key, "name")) {
                                ogs_yaml_iter_t hostname_iter;
                                ogs_yaml_iter_recurse(
                                        &nbrremoteserver_iter, &hostname_iter);
                                ogs_assert(ogs_yaml_iter_type(&hostname_iter) !=
                                    YAML_MAPPING_NODE);

                                do {
                                    if (ogs_yaml_iter_type(&hostname_iter) ==
                                            YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    ogs_assert(num < OGS_MAX_NUM_OF_HOSTNAME);
                                    hostname[num++] =
                                        ogs_yaml_iter_value(&hostname_iter);
                                } while (
                                    ogs_yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE);
                            } else if (!strcmp(nbrremoteserver_key, "port")) {
                                const char *v = ogs_yaml_iter_value(&nbrremoteserver_iter);
                                if (v) port = atoi(v);
                            } else if (!strcmp(nbrremoteserver_key, "dev")) {
                                dev = ogs_yaml_iter_value(&nbrremoteserver_iter);
                            } else
                                ogs_warn("unknown key `%s`", nbrremoteserver_key);
                        }

                        addr = NULL;
                        for (i = 0; i < num; i++) {
                            rv = ogs_addaddrinfo(&addr,
                                    family, hostname[i], port, 0);
                            ogs_assert(rv == OGS_OK);
                        }

                        if (addr) {
                            ogs_socknode_add(
                                        &self.nbrremoteserver_list, AF_INET, addr, NULL);

                            ogs_freeaddrinfo(addr);
                        }

                    } while (ogs_yaml_iter_type(&nbrremoteserver_array) ==
                            YAML_SEQUENCE_NODE);
                }
            }
        }
    }

    //rv = upf_context_validation();
    //if (rv != OGS_OK) return rv;

    return OGS_OK;
}

upf_remoteclient_t *upf_remoteclient_find_by_addr(ogs_sockaddr_t *addr)
{
    ogs_assert(addr);
    return (upf_remoteclient_t *)ogs_hash_get(self.remoteclient_addr_hash,
            addr, sizeof(ogs_sockaddr_t));

    return NULL;
}


upf_remoteclient_t *upf_remoteclient_add(ogs_sock_t *sock, ogs_sockaddr_t *addr)
{
    upf_remoteclient_t *remoteclient = NULL;
    upf_event_t e;

    ogs_assert(sock);
    ogs_assert(addr);

    ogs_pool_alloc(&upf_remoteclient_pool, &remoteclient);
    ogs_assert(remoteclient);
    memset(remoteclient, 0, sizeof *remoteclient);

    remoteclient->sctp.sock = sock;
    remoteclient->sctp.addr = addr;
    remoteclient->sctp.type = SOCK_STREAM;
    remoteclient->sctp.poll.read = ogs_pollset_add(ogs_app()->pollset,
        OGS_POLLIN, sock->fd, nbr_recvremotclient_upcall, sock);
    ogs_assert(remoteclient->sctp.poll.read);

    remoteclient->max_num_of_ostreams = OGS_DEFAULT_SCTP_MAX_NUM_OF_OSTREAMS;
    remoteclient->ostream_id = 0;
    /*if (ogs_app()->sctp.max_num_of_ostreams) {
        remoteclient->max_num_of_ostreams = ogs_app()->sctp.max_num_of_ostreams;
        ogs_info("[NBR] max_num_of_ostreams : %d", remoteclient->max_num_of_ostreams);
    }*/

    ogs_hash_set(self.remoteclient_addr_hash,
            remoteclient->sctp.addr, sizeof(ogs_sockaddr_t), remoteclient);


    ogs_list_add(&self.remoteclient_list, remoteclient);

    ogs_info("[Added] Number of remoteclient is now %d",
            ogs_list_count(&self.remoteclient_list));

    return remoteclient;
}

upf_remoteserver_t *upf_remoteserver_find_by_addr(ogs_sockaddr_t *addr)
{
    ogs_assert(addr);
    return (upf_remoteserver_t *)ogs_hash_get(self.remoteserver_addr_hash,
            addr, sizeof(ogs_sockaddr_t));

    return NULL;
}


upf_remoteserver_t *upf_remoteserver_add(ogs_sock_t *sock, ogs_sockaddr_t *addr)
{
    upf_remoteserver_t *remoteserver = NULL;
    upf_event_t e;

    ogs_assert(sock);
    ogs_assert(addr);

    ogs_pool_alloc(&upf_remoteserver_pool, &remoteserver);
    ogs_assert(remoteserver);
    memset(remoteserver, 0, sizeof *remoteserver);

    remoteserver->sctp.sock = sock;
    ogs_info("upf_remoteserver_add,add:%p",addr);
    remoteserver->sctp.addr = addr;
	remoteserver->sctp.type = SOCK_SEQPACKET;
    /*
    remoteserver->sctp.poll.read = ogs_pollset_add(ogs_app()->pollset,
        OGS_POLLIN, sock->fd, nbr_recvremotclient_upcall, sock);
    ogs_assert(remoteserver->sctp.poll.read);
    */
    remoteserver->max_num_of_ostreams = OGS_DEFAULT_SCTP_MAX_NUM_OF_OSTREAMS;
    remoteserver->ostream_id = 0;
    /*if (ogs_app()->sctp.max_num_of_ostreams) {
        remoteserver->max_num_of_ostreams = ogs_app()->sctp.max_num_of_ostreams;
        ogs_info("[NBR] max_num_of_ostreams : %d", remoteserver->max_num_of_ostreams);
    }*/

    ogs_hash_set(self.remoteserver_addr_hash,
            remoteserver->sctp.addr, sizeof(ogs_sockaddr_t), remoteserver);


    ogs_list_add(&self.remoteserver_list, remoteserver);

    ogs_info("[Added] Number of remoteserver is now %d",
            ogs_list_count(&self.remoteserver_list));

    return remoteserver;
}

void upf_remoteclient_remove(upf_remoteclient_t *remoteclient)
{

    ogs_assert(remoteclient);
    ogs_assert(remoteclient->sctp.sock);

    ogs_list_remove(&self.remoteclient_list, remoteclient);


    ogs_hash_set(self.remoteclient_addr_hash,
            remoteclient->sctp.addr, sizeof(ogs_sockaddr_t), NULL);

    ogs_sctp_flush_and_destroy(&remoteclient->sctp);

    ogs_pool_free(&upf_remoteclient_pool, remoteclient);

    ogs_info("[Removed] Number of Remoteclient is now %d",
            ogs_list_count(&self.remoteclient_list));
}

void upf_remoteserver_remove(upf_remoteserver_t *remoteserver)
{
    ogs_socknode_t *node = NULL;
	
    ogs_assert(remoteserver);
    ogs_assert(remoteserver->sctp.sock);

    ogs_list_remove(&self.remoteserver_list, remoteserver);


    ogs_hash_set(self.remoteserver_addr_hash,
            remoteserver->sctp.addr, sizeof(ogs_sockaddr_t), NULL);
    ogs_list_for_each(&upf_self()->nbrremoteserver_list, node)
    {
        if(node->addr->sin.sin_addr.s_addr == remoteserver->sctp.addr->sin.sin_addr.s_addr)
        {
            if (node->poll)
            {
                ogs_info("[Removed] upf_remoteserver_remove remove node->poll");
                ogs_pollset_remove(node->poll);
                node->poll = NULL;
            }
            if (node->sock){//add at 20240706
                ogs_sctp_destroy(node->sock);
                node->sock = NULL;
            }
			break;
        }
    }
    //ogs_sctp_flush_and_destroy(&remoteserver->sctp);
    if (remoteserver->sctp.sock){
        ogs_info("test:remoteserver->sctp.sock:%p",remoteserver->sctp.sock);
        //ogs_sctp_destroy(remoteserver->sctp.sock);//TODO;这里可以不释放,因为socket在nbrremoteserver_list中会释放
    }
	if(remoteserver->sctp.addr){
        ogs_info("upf_remoteserver_remove,free addr:%p",remoteserver->sctp.addr);
	    ogs_free(remoteserver->sctp.addr);
	}
    ogs_pool_free(&upf_remoteserver_pool, remoteserver);

    ogs_info("[Removed] Number of Remoteserver is now %d",
            ogs_list_count(&self.remoteserver_list));
}

void upf_remoteclient_remove_all(void)
{
    upf_remoteclient_t *remoteclient = NULL, *next_remoteclient = NULL;

    ogs_list_for_each_safe(&self.remoteclient_list, next_remoteclient, remoteclient)
        upf_remoteclient_remove(remoteclient);
}

void upf_remoteserver_remove_all(void)
{
    upf_remoteserver_t *remoteserver = NULL, *next_remoteserver = NULL;

    ogs_list_for_each_safe(&self.remoteserver_list, next_remoteserver, remoteserver)
        upf_remoteserver_remove(remoteserver);
}

void upf_send_singlelocalueip_to_nbrclient(uint32_t ueaddr)
{
    upf_nbr_message_t nbrmessage;
    int sent;
	uint16_t len;
	upf_remoteclient_t *remoteclient = NULL;

    ogs_info("upf_send_singlelocalueip_to_nbrclient, addr:0x%x.",ueaddr);
	
	memset(&nbrmessage, 0, sizeof(upf_nbr_message_t));
	#if defined(USE_DPDK)
	nbrmessage.serveraddr = dkuf.n6_addr.ipv4;
    #else 
    ogs_socknode_t *node = NULL;	
    node = ogs_list_first(&self.nbrlocalserver_list);
    if (node != NULL){
        nbrmessage.serveraddr = node->addr->sin.sin_addr.s_addr;
    }
	#endif
	nbrmessage.optype = 1;
	nbrmessage.uenum = 1;
	nbrmessage.addr[0] = ueaddr;
    len = sizeof(upf_nbr_message_t) - (200 - nbrmessage.uenum)*4;

    ogs_list_for_each(&upf_self()->remoteclient_list, remoteclient)
    {
		sent = ogs_sctp_sendmsg(remoteclient->sctp.sock, &nbrmessage, len,
	            remoteclient->sctp.addr, 88, 0);
	    if (sent < 0 || sent != len) {
	        ogs_error("upf_send_singlelocalueip_to_nbrclient(len:%d,ssn:%d) error (%d:%s)",
	                len, 0, errno, strerror(errno));
	    }
    }
}

void upf_handle_remoteserver_nbrmessage(upf_nbr_message_t *nbrmessage, uint16_t len)
{
    upf_sess_t *old = NULL;
	upf_sess_t *new_sess = NULL;
	upf_sess_t *sess = NULL;
    uint16_t loop;
    char *ipstr = NULL;
    ogs_error("upf_handle_remoteserver_nbrmessage() start,uenum:%d",nbrmessage->uenum);

    //增加nbr ue
	if(nbrmessage->optype == 1)
	{
		for(loop = 0; loop < nbrmessage->uenum; loop++)
		{
            ipstr = ogs_ipv4_to_string(nbrmessage->addr[loop]);
		    ogs_error("loop %d, addr %s\n", loop, ipstr);
		    old = ipv4_sess_find(self.nbr_ipv4_hash, nbrmessage->addr[loop]);

			if (old)
			{
		        ipv4_hash_remove(self.nbr_ipv4_hash, nbrmessage->addr[loop]);
		        //fwd_flush_buffered_packet(old);
		        //free_upf_dpdk_sess(old);
                ogs_free(old);
		    }
			new_sess = ogs_malloc(sizeof(upf_sess_t));
			if(!new_sess)
			{
			    ogs_error("dpdk_malloc failed\n");
			    continue;
			}
			new_sess->bnbr = 1;
			new_sess->nbraddr = nbrmessage->serveraddr;
			ogs_error("upf_handle_remoteserver_nbrmessage, ipaddr %s, 0x%x\n", ipstr,nbrmessage->addr[loop]);
			ipv4_hash_insert(self.nbr_ipv4_hash, nbrmessage->addr[loop], new_sess);
            ogs_free(ipstr);
		}
	}
    else if(nbrmessage->optype == 2)
    {
        //删除ue
        for(loop = 0; loop < nbrmessage->uenum; loop++)
		{
		    old = ipv4_sess_find(self.nbr_ipv4_hash, nbrmessage->addr[loop]);

			if (old)
			{
			    if(old->nbraddr == nbrmessage->serveraddr)
			    {
			        ipv4_hash_remove(self.nbr_ipv4_hash, nbrmessage->addr[loop]);
			        //fwd_flush_buffered_packet(old);
			        //free_upf_dpdk_sess(old);
                    ogs_free(old);
			    }
		    }
		}
    }
	else if(nbrmessage->optype == 3)
	{
	    //remote nbr server lost,delete all nbrserver ue
	    ipv4_hash_remove_nbrservaddr(self.nbr_ipv4_hash, nbrmessage->serveraddr);
	}


	#if defined(USE_DPDK)
	upf_dpdk_nbr_notify(nbrmessage);
    #endif
}

upf_remoteserver_t *upf_remoteserver_find_by_remoteclient_ipaddr(uint32_t ipaddr)
{    
	upf_remoteserver_t *remoteserver = NULL, *next_remoteserver = NULL;

    ogs_list_for_each_safe(&self.remoteserver_list, next_remoteserver, remoteserver)
    {
        if(remoteserver->sctp.addr->sin.sin_addr.s_addr == ipaddr)
        {
            return remoteserver;
        }
    }

	return NULL;
}


void upf_send_singlelocalueip_to_nbrclient_del(uint32_t ueaddr)
{
    upf_nbr_message_t nbrmessage;
    int sent;
	uint16_t len;
	upf_remoteclient_t *remoteclient = NULL;
	
	memset(&nbrmessage, 0, sizeof(upf_nbr_message_t));
	#if defined(USE_DPDK)
	nbrmessage.serveraddr = dkuf.n6_addr.ipv4;
    #else 
    ogs_socknode_t *node = NULL;	
    node = ogs_list_first(&self.nbrlocalserver_list);
    if (node != NULL){
        nbrmessage.serveraddr = node->addr->sin.sin_addr.s_addr;
    }
	#endif
	nbrmessage.optype = 2;
	nbrmessage.uenum = 1;
	nbrmessage.addr[0] = ueaddr;
    len = sizeof(upf_nbr_message_t) - (200 - nbrmessage.uenum)*4;

    ogs_list_for_each(&upf_self()->remoteclient_list, remoteclient)
    {
		sent = ogs_sctp_sendmsg(remoteclient->sctp.sock, &nbrmessage, len,
	            remoteclient->sctp.addr, 88, 0);
	    if (sent < 0 || sent != len) {
	        ogs_error("upf_send_singlelocalueip_to_nbrclient_del(len:%d,ssn:%d) error (%d:%s)",
	                len, 0, errno, strerror(errno));
	    }
    }
}

void upf_handle_remoteserver_lost(uint32_t nbrserveraddr)
{
    upf_nbr_message_t nbrmsg;

	memset(&nbrmsg, 0, sizeof(upf_nbr_message_t));

	nbrmsg.serveraddr = nbrserveraddr;
	nbrmsg.optype = 3;
	nbrmsg.uenum = 0;
    ogs_error("upf_handle_remoteserver_lost() nbrmsg.serveraddr 0x%x, nbrmsg.optype %d", nbrmsg.serveraddr, nbrmsg.optype);
	#if defined(USE_DPDK)
	upf_dpdk_nbr_notify(&nbrmsg);
    #endif
}


void upf_send_alllocalueip_to_newnbrclient(upf_remoteclient_t *remoteclient)
{
    upf_nbr_message_t nbrmessage;
    int sent;
	uint16_t len;
	upf_sess_t *sess = NULL, *next = NULL;;
    memset(&nbrmessage, 0, sizeof(upf_nbr_message_t));
	#if defined(USE_DPDK)
	nbrmessage.serveraddr = dkuf.n6_addr.ipv4;
    #else
    ogs_socknode_t *node = NULL;	
    node = ogs_list_first(&self.nbrlocalserver_list);
    if (node != NULL){
        nbrmessage.serveraddr = node->addr->sin.sin_addr.s_addr;
    }
	#endif
	nbrmessage.optype = 1;
    ogs_list_for_each_safe(&self.sess_list, next, sess)
	{
	    if(nbrmessage.uenum < 200)
	    {
	        if(sess && sess->ipv4)
	        {
	            nbrmessage.addr[nbrmessage.uenum++] = sess->ipv4->addr[0];
	        }
	    }
    }
	ogs_info("upf_send_alllocalueip_to_newnbrclient uenum %d",nbrmessage.uenum);
	if(nbrmessage.uenum)
	{
	    len = sizeof(upf_nbr_message_t) - (200 - nbrmessage.uenum)*4;

		sent = ogs_sctp_sendmsg(remoteclient->sctp.sock, &nbrmessage, len,
		            remoteclient->sctp.addr, 88, 0);
	    if (sent < 0 || sent != len) {
	        ogs_error("upf_send_singlelocalueip_to_nbrclient(len:%d,ssn:%d) error (%d:%s)",
	                len, 0, errno, strerror(errno));
	    }
	}
}


int ipv4_hash_remove_nbrservaddr(struct ipv4_hashtbl *h, uint32_t ip)
{
    ipv4_node_t *prev, *next = NULL;
    ipv4_node_t *cur = NULL;
    uint32_t i;
	upf_sess_t *sess = NULL;
	//ogs_error("%s, serveraddr 0x%x\n", __func__, ip);
	assert(h && h->htable);

    for (i = 0; i < h->size; i++)
	{
        cur = h->htable[i];
        while (cur) 
		{

			sess = (upf_sess_t *)cur->sess;
			#if 0
			if(sess)
			{
			    ogs_error("%s, sess->nbraddr 0x%x\n", __func__, sess->nbraddr);
			}
			#endif
			if(sess && sess->nbraddr == ip)
			{
			    next = cur->next;
	            next->prev = cur->prev;

				if (cur->prev)
				{
			        prev = cur->prev;
			        prev->next = cur->next;
			    } else
				{
			        h->htable[ip & h->bitmask] = next;
			    }
				free(cur);
                #if defined(USE_DPDK)
				free_upf_dpdk_sess(sess);
                #else
                ogs_free(sess);
                #endif
				h->num--;
				cur = next;
			}
			else
			{
	            next = cur->next;
	            cur = next;
			}

			
        }
    }
    return 0;
}
