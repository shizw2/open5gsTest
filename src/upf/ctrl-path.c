#include "context.h"
#include "event.h"
#include "pfcp-path.h"
#include "ctrl-path.h"


inline static int copy_pfcp_far(ogs_pfcp_far_t *new, ogs_pfcp_far_t *old, ogs_pfcp_sess_t *pfcp_sess) {
    memcpy(new, old, sizeof(ogs_pfcp_far_t));
    new->sess = pfcp_sess;

    /* See @pfcp/context.c: ogs_pfcp_setup_far_gtpu_node() */
    if (old->gnode) {
        new->gnode = dpdk_malloc(sizeof(ogs_gtp_node_t));
        memcpy(&((ogs_gtp_node_t *) new->gnode)->addr, &((ogs_gtp_node_t *) old->gnode)->addr, sizeof(ogs_sockaddr_t));
        memcpy(&((ogs_gtp_node_t *) new->gnode)->ip, &((ogs_gtp_node_t *) old->gnode)->ip, sizeof(ogs_ip_t));
    }

    ogs_list_add(&pfcp_sess->far_list, new);
    return OGS_OK;
}

inline static int copy_pfcp_urr(ogs_pfcp_urr_t *new, ogs_pfcp_urr_t *old, ogs_pfcp_sess_t *pfcp_sess) {
    memcpy(new, old, sizeof(ogs_pfcp_urr_t));
    new->sess = pfcp_sess;
    ogs_list_add(&pfcp_sess->urr_list, new);
    return OGS_OK;
}

inline static int copy_pfcp_qer(ogs_pfcp_qer_t *new, ogs_pfcp_qer_t *old, ogs_pfcp_sess_t *pfcp_sess) {
    memcpy(new, old, sizeof(ogs_pfcp_qer_t));
    new->sess = pfcp_sess;
    ogs_list_add(&pfcp_sess->qer_list, new);
    return OGS_OK;
}

static int copy_pfcp_pdr(ogs_pfcp_pdr_t *new, ogs_pfcp_pdr_t *old, ogs_pfcp_sess_t *pfcp_sess) {
    int flow;
    ogs_pfcp_rule_t *rule;
    ogs_pfcp_rule_t *new_rule;
    int i;

    memcpy(new, old, sizeof(ogs_pfcp_pdr_t));

    if (old->far) {
        new->far = dpdk_malloc(sizeof(ogs_pfcp_far_t));
        copy_pfcp_far(new->far, old->far, pfcp_sess);
    }

    //if (old->urr) {
    //    new->urr[0] = dpdk_malloc(sizeof(ogs_pfcp_urr_t));
    //    copy_pfcp_urr(new->urr[0], old->urr[0], pfcp_sess);
    //}
    ogs_info("old->num_of_urr:%d.",old->num_of_urr);
    if (old->num_of_urr > 0) {
        for (i = 0; i < old->num_of_urr; i++){
            new->urr[i] = dpdk_malloc(sizeof(ogs_pfcp_urr_t));
            copy_pfcp_urr(new->urr[i], old->urr[i], pfcp_sess);
        }
        new->num_of_urr = old->num_of_urr;
    }

    if (old->qer) {
        new->qer = dpdk_malloc(sizeof(ogs_pfcp_qer_t));
        copy_pfcp_qer(new->qer, old->qer, pfcp_sess);
    }

    if (old->dnn) {
        new->dnn = dpdk_malloc(strlen(old->dnn) + 1);
        memcpy(new->dnn, old->dnn, strlen(old->dnn) + 1);
    }

    for (flow = 0; flow < old->num_of_flow; flow++) {
        new->flow[flow].description = dpdk_malloc(strlen(old->flow[flow].description) + 1);
        memcpy(new->flow[flow].description, old->flow[flow].description, strlen(old->flow[flow].description) + 1);
    }

    ogs_list_init(&new->rule_list);
    ogs_list_for_each(&old->rule_list, rule) {
        new_rule = dpdk_malloc(sizeof(ogs_pfcp_rule_t));
        memcpy(new_rule, rule, sizeof(ogs_pfcp_rule_t));
        new_rule->pdr = new;
        ogs_list_add(&new->rule_list, new_rule);
    }

    new->sess = pfcp_sess;
    return OGS_OK;
}

static int copy_pfcp_sess(ogs_pfcp_sess_t *new_sess, ogs_pfcp_sess_t *old_sess) {
    ogs_pfcp_pdr_t *pdr;
    ogs_pfcp_pdr_t *new_pdr;

    new_sess->obj = old_sess->obj;

    ogs_list_init(&new_sess->pdr_list);
    ogs_list_init(&new_sess->far_list);
    ogs_list_init(&new_sess->urr_list);
    ogs_list_init(&new_sess->qer_list);

    ogs_list_for_each(&old_sess->pdr_list, pdr) {
        new_pdr = dpdk_malloc(sizeof(ogs_pfcp_pdr_t));
        copy_pfcp_pdr(new_pdr, pdr, new_sess);
        ogs_list_add(&new_sess->pdr_list, new_pdr);
    }

    ogs_debug("dump pdr_list");
    ogs_list_for_each(&new_sess->pdr_list, pdr) {
        ogs_debug("pdr: %p", pdr);
    }

    if (old_sess->bar) {
        new_sess->bar = dpdk_malloc(sizeof(ogs_pfcp_bar_t));
        new_sess->bar->id = old_sess->bar->id;
        new_sess->bar->sess = new_sess;
    }
    return OGS_OK;
}

static void *copy_upf_sess(upf_sess_t *old_sess) {
    int ret;
    upf_sess_t *new_sess;
    new_sess = dpdk_malloc(sizeof(upf_sess_t));
    if (!new_sess) {
        ogs_error("Error rte_malloc() upf_sess_t");
        goto cleanup;
    }

    new_sess->upf_n4_seid_node = old_sess->upf_n4_seid_node;
    new_sess->upf_n4_seid = old_sess->upf_n4_seid;
    new_sess->smf_n4_f_seid.seid = old_sess->smf_n4_f_seid.seid;

    ret = copy_pfcp_sess(&new_sess->pfcp, &old_sess->pfcp);
    if (ret)
        goto cleanup;

    if (old_sess->ipv4) {
        new_sess->ipv4 = dpdk_malloc(sizeof(ogs_pfcp_ue_ip_t));
        memcpy(new_sess->ipv4, old_sess->ipv4, sizeof(ogs_pfcp_ue_ip_t));
        if (old_sess->ipv4->subnet) {
            new_sess->ipv4->subnet = dpdk_malloc(sizeof(ogs_pfcp_subnet_t));
            memcpy(new_sess->ipv4->subnet, old_sess->ipv4->subnet, sizeof(ogs_pfcp_subnet_t));
        }
    }

    if (old_sess->ipv6) {
        new_sess->ipv6 = dpdk_malloc(sizeof(ogs_pfcp_ue_ip_t));
        memcpy(new_sess->ipv6, old_sess->ipv6, sizeof(ogs_pfcp_ue_ip_t));
        if (old_sess->ipv6->subnet) {
            new_sess->ipv6->subnet = dpdk_malloc(sizeof(ogs_pfcp_subnet_t));
            memcpy(new_sess->ipv6->subnet, old_sess->ipv6->subnet, sizeof(ogs_pfcp_subnet_t));
        }
    }
    
    if (old_sess->ipv4_framed_routes) {
        new_sess->ipv4_framed_routes = dpdk_malloc(sizeof(ogs_ipsubnet_t));
        memcpy(new_sess->ipv4_framed_routes, old_sess->ipv4_framed_routes, sizeof(ogs_ipsubnet_t));
    }
    
    if (old_sess->ipv6_framed_routes) {
        new_sess->ipv6_framed_routes = dpdk_malloc(sizeof(ogs_ipsubnet_t));
        memcpy(new_sess->ipv6_framed_routes, old_sess->ipv6_framed_routes, sizeof(ogs_ipsubnet_t));
    }
    
    return new_sess;

    // TODO: 中间过程malloc错误没有处理
    cleanup:
    if (new_sess)
        dpdk_free(new_sess);
    return NULL;
}

static void free_pfcp_pdr(ogs_pfcp_pdr_t *pdr) {
    int flow;
    ogs_lnode_t *lnode = NULL;
    ogs_pfcp_rule_t *rule = NULL;

    if (pdr->far) {
        if (pdr->far->gnode) {
            dpdk_free(pdr->far->gnode);
        }
        dpdk_free(pdr->far);
    }
    if (pdr->urr) {
        dpdk_free(pdr->urr);
    }
    if (pdr->qer) {
        dpdk_free(pdr->qer);
    }
    if (pdr->dnn) {
        dpdk_free(pdr->dnn);
    }

    for (flow = 0; flow < pdr->num_of_flow; flow++) {
        dpdk_free(pdr->flow[flow].description);
    }

    ogs_list_for_each(&pdr->rule_list, rule) {
        if (lnode) {
            dpdk_free(lnode);
        }
        lnode = (ogs_lnode_t *) rule;
    }
    if (lnode) {
        dpdk_free(lnode);
    }
}

int free_upf_dpdk_sess(upf_sess_t *upf_sess) {
    ogs_pfcp_pdr_t *pdr;

    ogs_lnode_t *lnode = NULL;
    ogs_list_for_each(&upf_sess->pfcp.pdr_list, pdr) {
        if (lnode) {
            free_pfcp_pdr((ogs_pfcp_pdr_t *) lnode);
        }
        lnode = (ogs_lnode_t *) pdr;
    }
    if (lnode) {
        free_pfcp_pdr((ogs_pfcp_pdr_t *) lnode);
    }

    if (upf_sess->pfcp.bar) {
        dpdk_free(upf_sess->pfcp.bar);
    }

    if (upf_sess->ipv4) {
        if (upf_sess->ipv4->subnet) {
            dpdk_free(upf_sess->ipv4->subnet);
        }
        dpdk_free(upf_sess->ipv4);
    }
    if (upf_sess->ipv6) {
        if (upf_sess->ipv6->subnet) {
            dpdk_free(upf_sess->ipv6->subnet);
        }
        dpdk_free(upf_sess->ipv6);
    }
    dpdk_free(upf_sess);
    return OGS_OK;
}

static struct rte_ring *get_p2f_ring(upf_sess_t *sess) {
    int fwd_index = 0;
    uint32_t ipv4;
    struct dpdk_upf_s *context;
    context = upf_dpdk_context();
    // TODO: dispatch ipv6
    if (sess->ipv4) {
        ipv4 = sess->ipv4->addr[0];
        fwd_index = ntohl(ipv4) % context->fwd_num;
        ogs_debug("Dispach upf_sess["FORMAT_IPV4"] to fwd_index[%d]", FORMAT_IPV4_ARGS(ipv4), fwd_index);
    }
    return context->p2f_ring[fwd_index];
}

static struct rte_ring *get_f2p_ring(int fwd_id) {
    struct dpdk_upf_s *context;
    context = upf_dpdk_context();

    return context->f2p_ring[fwd_id];
}

static void handle_event(upf_dpdk_event_t *event) {
    int ret;
    upf_dpdk_event_report_t *event_report;
    upf_sess_t *upf_sess = NULL;

    uint8_t *frame = NULL;
    ogs_pfcp_far_t *far = NULL;
    // TODO: init frame_len
    uint32_t frame_len = 60;

    ogs_pfcp_user_plane_report_t up_report;
    ogs_pkbuf_t pkbuf;
    memset(&up_report, 0, sizeof(ogs_pfcp_user_plane_report_t));
    memset(&pkbuf, 0, sizeof(ogs_pkbuf_t));

    if (event->event_type != UPF_DPDK_SESS_REPORT) {
        dpdk_free(event);
        return;
    }
    event_report = event->event_body;
    ogs_assert(event_report);

    switch (event->subtype) {
        case SESS_REPORT_NORMAL:
            upf_sess = upf_sess_find_by_upf_n4_seid(event_report->sess_index);
            if (!upf_sess)
                goto cleanup;

            up_report.type.downlink_data_report = 1;
            up_report.downlink_data.pdr_id = event_report->pdr_id;
            up_report.downlink_data.qfi = event_report->qfi;
            ret = upf_pfcp_send_session_report_request(upf_sess, &up_report);
            if (ret != OGS_OK) {
                ogs_error("Error send session report");
            }
            break;
        case SESS_REPORT_ERR_IND:
            frame = event_report->paylod;
            pkbuf.data = frame;
            far = ogs_pfcp_far_find_by_gtpu_error_indication(&pkbuf);
            if (far) {
                ogs_assert(true == ogs_pfcp_up_handle_error_indication(far, &up_report));

                if (up_report.type.error_indication_report) {
                    ogs_assert(far->sess);
                    upf_sess = UPF_SESS(far->sess);
                    ogs_assert(upf_sess);

                    ogs_assert(OGS_OK == upf_pfcp_send_session_report_request(upf_sess, &up_report));
                }

            } else {
                ogs_error("[DROP] Cannot find FAR by Error-Indication");
                ogs_log_hexdump(OGS_LOG_ERROR, pkbuf.data, frame_len);
            }
            break;
        default:
            ogs_error("Unknown event subtype: %d", event->subtype);
            break;
    }

    cleanup:
    if (event_report->paylod)
        dpdk_free(event_report->paylod);

    dpdk_free(event_report);
    dpdk_free(event);
}

int upf_dpdk_loop_event(void) {
    int i, ret;
    struct dpdk_upf_s *dpdk_context = upf_dpdk_context();
    upf_dpdk_event_t *event = NULL;

    for (i = 0; i < dpdk_context->fwd_num; i++) {
        for (;;) {
            ret = rte_ring_dequeue(dpdk_context->f2p_ring[i], (void **) &event);
            if (ret)
                break;
            handle_event(event);
        }
    }
    return OGS_OK;
}

static int send_p2f_event(upf_dpdk_event_t *event, upf_sess_t *sess) {
    int ret = 0;
    struct rte_ring *ring = NULL;

    ring = get_p2f_ring(sess);
    ret = rte_ring_enqueue(ring, event);

    if (ret != 0) {
        ogs_error("Error enqueue upf to dpdk event");
        if (event->event_type == UPF_DPDK_SESS_ESTAB || event->event_type == UPF_DPDK_SESS_MOD) {
            free_upf_dpdk_sess(event->event_body);
        }
        dpdk_free(event);
        return OGS_ERROR;
    }
    ogs_debug("CP send event type=%d, sess index=%d, upf seid=%ld", event->event_type, *sess->upf_n4_seid_node, sess->upf_n4_seid);
    return OGS_OK;
}

int upf_dpdk_sess_establish(upf_sess_t *sess) {
    ogs_debug("upf_dpdk_sess_establish() start");
    upf_dpdk_event_t *event = dpdk_malloc(sizeof(upf_dpdk_event_t));
    if (!event) {
        ogs_error("Error allocate memory.");
        return OGS_ERROR;
    }

    void *event_body = copy_upf_sess(sess);
    if (!event_body) {
        ogs_error("Error copy upf sess.");
        return OGS_ERROR;
    }

    event->event_type = UPF_DPDK_SESS_ESTAB;
    event->event_body = event_body;

    return send_p2f_event(event, sess);
}

int upf_dpdk_sess_delete(upf_sess_t *sess) {
    ogs_debug("upf_dpdk_sess_delete() start");
    upf_dpdk_event_t *event = dpdk_malloc(sizeof(upf_dpdk_event_t));
    if (!event) {
        ogs_error("Error allocate memory.");
        return OGS_ERROR;
    }

    event->event_type = UPF_DPDK_SESS_DEL;
    // TODO: dispatch ipv6
    event->event_body = (void *) (uintptr_t)sess->ipv4->addr[0];

    return send_p2f_event(event, sess);
}

int upf_dpdk_sess_modify(upf_sess_t *sess) {
    ogs_debug("upf_dpdk_sess_modify() start");
    upf_dpdk_event_t *event = dpdk_malloc(sizeof(upf_dpdk_event_t));
    if (!event) {
        ogs_error("Error allocate memory.");
        return OGS_ERROR;
    }

    void *event_body = copy_upf_sess(sess);
    if (!event_body) {
        ogs_error("Error copy upf sess.");
        return OGS_ERROR;
    }

    event->event_type = UPF_DPDK_SESS_MOD;
    event->event_body = event_body;

    return send_p2f_event(event, sess);
}

int upf_dpdk_sess_report(int fwd_id, upf_dpdk_event_t *event) {
    int ret;
    struct rte_ring *ring = NULL;

    ring = get_f2p_ring(fwd_id);

    ret = rte_ring_enqueue(ring, &event);
    if (ret != 1) {
        ogs_error("Error enqueue dpdk to upf event");
        return OGS_ERROR;
    }
    return OGS_OK;
}

static struct rte_ring *get_p2f_nbr_ring(void){
    int fwd_index = 0;
    uint32_t ipv4;
    struct dpdk_upf_s *context;
    context = upf_dpdk_context();
    // TODO: dispatch ipv6

	fwd_index = 0;
    return context->p2f_ring[fwd_index];
}

static int send_p2f_nbr_event(upf_dpdk_event_t *event)
{
    int ret = 0;
    struct rte_ring *ring = NULL;

    ring = get_p2f_nbr_ring();
    ret = rte_ring_enqueue(ring, event);

    if (ret != 0) {
        ogs_error("Error enqueue upf to dpdk event");
        dpdk_free(event->event_body);
        dpdk_free(event);
        return OGS_ERROR;
    }
    ogs_error("CP send event type=%d", event->event_type);
    return OGS_OK;
}
int upf_dpdk_nbr_notify(upf_nbr_message_t *nbrmsg)
{
    ogs_error("upf_dpdk_nbr_notify() start");
    upf_dpdk_event_t *event = dpdk_malloc(sizeof(upf_dpdk_event_t));
    if (!event) {
        ogs_error("Error allocate memory.");
        return OGS_ERROR;
    }
    upf_nbr_message_t *nbrmsgnew = dpdk_malloc(sizeof(upf_nbr_message_t));
	memcpy(nbrmsgnew, nbrmsg, sizeof(upf_nbr_message_t));
	ogs_error("upf_dpdk_nbr_notify nbrmsgnew->serveraddr 0x%x, nbrmsgnew->optype %d", nbrmsgnew->serveraddr, nbrmsgnew->optype);
    void *event_body = (void *)nbrmsgnew;
    if (!event_body) {
        ogs_error("Error copy upf nbrmsg.");
        return OGS_ERROR;
    }

    event->event_type = UPF_DPDK_NBR_MSG;
    event->event_body = event_body;

    return send_p2f_nbr_event(event);
}