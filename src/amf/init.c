/*
 * Copyright (C) 2019-2022 by Sukchan Lee <acetcom@gmail.com>
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

#include "sbi-path.h"
#include "ngap-path.h"
#include "metrics.h"

static ogs_thread_t *thread;
static void amf_main(void *data);
static void amf_sps_main(void *data);
static int initialized = 0;

static int sps_udp_ini_open(void);//挪到新文件
static int icps_udp_ini_open(void);
static void icps_recv_cb(short when, ogs_socket_t fd, void *data);
static void icps_client_recv_cb(short when, ogs_socket_t fd, void *data);

int amf_initialize()
{
    int rv;
    //setAffinity(6);
    ogs_metrics_context_init();
    ogs_sbi_context_init();

    amf_context_init();

    rv = ogs_sbi_context_parse_config("amf", "nrf", "scp");
    if (rv != OGS_OK) return rv;

    rv = ogs_metrics_context_parse_config("amf");
    if (rv != OGS_OK) return rv;

    rv = amf_context_parse_config();
    if (rv != OGS_OK) return rv;

    rv = amf_context_nf_info();
    if (rv != OGS_OK) return rv;

    rv = amf_m_tmsi_pool_generate();
    if (rv != OGS_OK) return rv;

    rv = amf_metrics_open();
    if (rv != 0) return OGS_ERROR;

    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) return rv;

    rv = amf_sbi_open();
    if (rv != OGS_OK) return rv;

    rv = ngap_open();
    if (rv != OGS_OK) return rv;

    /*增加UDP_IN_open，初始化ICPS和SPS直接的通信接口*/
    rv = icps_udp_ini_open();
    if (rv != OGS_OK) return rv;

    thread = ogs_thread_create(amf_main, NULL);
    if (!thread) return OGS_ERROR;

    initialized = 1;

    return OGS_OK;
}

int amf_sps_initialize()
{
    int rv;
    //setAffinity(6);
    ogs_metrics_context_init();
    ogs_sbi_context_init();

    amf_context_init();

    rv = ogs_sbi_context_parse_config("amf", "nrf", "scp");
    if (rv != OGS_OK) return rv;

    rv = ogs_metrics_context_parse_config("amf");
    if (rv != OGS_OK) return rv;

    rv = amf_context_parse_config();
    if (rv != OGS_OK) return rv;

    rv = amf_context_nf_info();
    if (rv != OGS_OK) return rv;

    rv = amf_m_tmsi_pool_generate();
    if (rv != OGS_OK) return rv;

    rv = amf_metrics_open();
    if (rv != 0) return OGS_ERROR;

    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) return rv;

    rv = sps_udp_ini_open();
    if (rv != OGS_OK) return rv;

    thread = ogs_thread_create(amf_sps_main, NULL);
    if (!thread) return OGS_ERROR;

    initialized = 1;

    return OGS_OK;
}

static int sps_udp_ini_open(void)
{
    ogs_socknode_t *node = NULL;
    ogs_sock_t *sock = NULL;
    ogs_sock_t *udp;
    char buf[OGS_ADDRSTRLEN];

    ogs_list_for_each(&amf_self()->icps_list, node) {
        udp = ogs_udp_client(node->addr, node->option);
        if (udp) {
            ogs_info("ogs_udp_client() [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));

            node->sock = udp;
        }else{ 
            ogs_error("ogs_udp_client() error [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));
            return OGS_ERROR;
        }

        node->poll = ogs_pollset_add(ogs_app()->pollset,
                OGS_POLLIN, udp->fd, icps_client_recv_cb, udp);
        ogs_assert(node->poll);
    }

    //set timer
	ogs_timer_t *timer = ogs_timer_add(ogs_app()->timer_mgr,amf_timer_internel_heart_beat_timer_expire,udp);
	
	ogs_timer_start(timer,1);

    return OGS_OK;
}

static int icps_udp_ini_open(void)
{
    ogs_socknode_t *node = NULL;    
    ogs_sock_t *udp;
    char buf[OGS_ADDRSTRLEN];

    ogs_list_for_each(&amf_self()->icps_list, node) {
        udp = ogs_udp_server(node->addr, node->option);
        if (udp) {
            ogs_info("udp_server() [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));

            node->sock = udp;
        }else{ 
            ogs_error("udp_server() error [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));
            return OGS_ERROR;
        }

        node->poll = ogs_pollset_add(ogs_app()->pollset,
                OGS_POLLIN, udp->fd, icps_recv_cb, udp);
        ogs_assert(node->poll);
    }

    return OGS_OK;
}

static void icps_recv_cb(short when, ogs_socket_t fd, void *data)
{
    int rv;

    ssize_t size;
    amf_event_t *e = NULL;
    ogs_pkbuf_t *pkbuf = NULL;
    ogs_sockaddr_t from;
  

    ogs_assert(fd != INVALID_SOCKET);

    pkbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
    ogs_assert(pkbuf);
    ogs_pkbuf_put(pkbuf, OGS_MAX_SDU_LEN);

    size = ogs_recvfrom(fd, pkbuf->data, pkbuf->len, 0, &from);
    if (size <= 0) {
        ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno,
                "ogs_recvfrom() failed");
        ogs_pkbuf_free(pkbuf);
        return;
    }

    ogs_pkbuf_trim(pkbuf, size);

    e = amf_event_new(AMF_EVENT_INTERNEL_MESSAGE);
    ogs_assert(e);

    e->pkbuf = pkbuf;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        ogs_pkbuf_free(e->pkbuf);
        ogs_event_free(e);
    }
}

static void icps_client_recv_cb(short when, ogs_socket_t fd, void *data)
{
    int rv;

    ssize_t size;
    amf_event_t *e = NULL;
    ogs_pkbuf_t *pkbuf = NULL;
    ogs_sockaddr_t from;
  

    ogs_assert(fd != INVALID_SOCKET);

    pkbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
    ogs_assert(pkbuf);
    ogs_pkbuf_put(pkbuf, OGS_MAX_SDU_LEN);

    size = ogs_recvfrom(fd, pkbuf->data, pkbuf->len, 0, &from);
    if (size <= 0) {
        ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno,
                "ogs_recvfrom() failed");
        ogs_pkbuf_free(pkbuf);
        return;
    }

    ogs_pkbuf_trim(pkbuf, size);

    e = amf_event_new(AMF_EVENT_INTERNEL_MESSAGE);
    ogs_assert(e);

    e->pkbuf = pkbuf;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        ogs_pkbuf_free(e->pkbuf);
        ogs_event_free(e);
    }
}

static ogs_timer_t *t_termination_holding = NULL;

static void event_termination(void)
{
    ogs_sbi_nf_instance_t *nf_instance = NULL;

    /* Sending NF Instance De-registeration to NRF */
    ogs_list_for_each(&ogs_sbi_self()->nf_instance_list, nf_instance)
        ogs_sbi_nf_fsm_fini(nf_instance);

    /* Starting holding timer */
    t_termination_holding = ogs_timer_add(ogs_app()->timer_mgr, NULL, NULL);
    ogs_assert(t_termination_holding);
#define TERMINATION_HOLDING_TIME ogs_time_from_msec(300)
    ogs_timer_start(t_termination_holding, TERMINATION_HOLDING_TIME);

    /* Sending termination event to the queue */
    ogs_queue_term(ogs_app()->queue);
    ogs_pollset_notify(ogs_app()->pollset);
}

void amf_terminate(void)
{
    if (!initialized) return;

    /* Daemon terminating */
    event_termination();
    ogs_thread_destroy(thread);
    ogs_timer_delete(t_termination_holding);

    ngap_close();
    amf_sbi_close();
    amf_metrics_close();

    amf_context_final();
    ogs_sbi_context_final();
    ogs_metrics_context_final();
}

static void amf_main(void *data)
{ 
    ogs_fsm_t amf_sm;
    int rv;
	
    setAffinity(6);
	
    ogs_fsm_init(&amf_sm, amf_state_initial, amf_state_final, 0);

    for ( ;; ) {
        ogs_pollset_poll(ogs_app()->pollset,
                ogs_timer_mgr_next(ogs_app()->timer_mgr));

        /*
         * After ogs_pollset_poll(), ogs_timer_mgr_expire() must be called.
         *
         * The reason is why ogs_timer_mgr_next() can get the corrent value
         * when ogs_timer_stop() is called internally in ogs_timer_mgr_expire().
         *
         * You should not use event-queue before ogs_timer_mgr_expire().
         * In this case, ogs_timer_mgr_expire() does not work
         * because 'if rv == OGS_DONE' statement is exiting and
         * not calling ogs_timer_mgr_expire().
         */
        ogs_timer_mgr_expire(ogs_app()->timer_mgr);

        for ( ;; ) {
            amf_event_t *e = NULL;

            rv = ogs_queue_trypop(ogs_app()->queue, (void**)&e);
            ogs_assert(rv != OGS_ERROR);

            if (rv == OGS_DONE)
                goto done;

            if (rv == OGS_RETRY)
                break;

            ogs_assert(e);
            ogs_fsm_dispatch(&amf_sm, e);
            ogs_event_free(e);
        }
    }
done:

    ogs_fsm_fini(&amf_sm, 0);
}


static void amf_sps_main(void *data)
{
    ogs_fsm_t amf_sps_sm;
    int rv;

    ogs_fsm_init(&amf_sps_sm, amf_state_initial, amf_state_final, 0);

    for ( ;; ) {
        ogs_pollset_poll(ogs_app()->pollset,
                ogs_timer_mgr_next(ogs_app()->timer_mgr));

       
        ogs_timer_mgr_expire(ogs_app()->timer_mgr);

        for ( ;; ) {
            amf_event_t *e = NULL;

            rv = ogs_queue_trypop(ogs_app()->queue, (void**)&e);
            ogs_assert(rv != OGS_ERROR);

            if (rv == OGS_DONE)
                goto done;

            if (rv == OGS_RETRY)
                break;

            ogs_assert(e);
            ogs_fsm_dispatch(&amf_sps_sm, e);
            ogs_event_free(e);
        }
    }
done:

    ogs_fsm_fini(&amf_sps_sm, 0);
}