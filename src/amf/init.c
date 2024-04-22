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
#include "udp-ini-path.h"
#include "metrics.h"
#include "license.h"
#include "ogs-app-timer.h"
#include "telnet.h"

static ogs_thread_t *thread;
static ogs_thread_t *cli_thread;
static void amf_main(void *data);
static void amf_sps_main(void *data);
static int initialized = 0;
void setCommands(void);
int amf_initialize(void)
{
    int rv;

    amf_metrics_init();

    ogs_sbi_context_init(OpenAPI_nf_type_AMF);
    amf_context_init();

    rv = license_check_init();
    if (rv != OGS_OK) return rv;

    rv = ogs_sbi_context_parse_config("amf", "nrf", "scp");
    if (rv != OGS_OK) return rv;

    rv = ogs_metrics_context_parse_config("amf");
    if (rv != OGS_OK) return rv;

    rv = amf_context_parse_config(false);
    if (rv != OGS_OK) return rv;

    rv = amf_context_nf_info(false);
    if (rv != OGS_OK) return rv;

    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) return rv;

    ogs_metrics_context_open(ogs_metrics_self());

    rv = amf_sbi_open();
    if (rv != OGS_OK) return rv;

    rv = ngap_open();
    if (rv != OGS_OK) return rv;

    /*增加UDP_IN_open，初始化ICPS和SPS直接的通信接口*/
    rv = udp_ini_open();
    if (rv != OGS_OK) return rv;

    /*启动yaml配置检测定时器*/
    rv = ogs_yaml_check_init();
    if (rv != OGS_OK) return rv;

    thread = ogs_thread_create(amf_main, NULL);
    if (!thread) return OGS_ERROR;

    initialized = 1;

    //set_telnet_cmd_callback(telnet_proc_cmd);
    setCommands();
    cli_thread = ogs_thread_create(telnetMain, &ogs_app()->cli_list);
    if (!cli_thread) return OGS_ERROR;
    
    return OGS_OK;
}

void amf_sps_update_cli_port(void);
void amf_sps_update_cli_port(void){
    ogs_socknode_t *node = NULL;
    ogs_sockaddr_t *addr = NULL;    
    char buf[OGS_ADDRSTRLEN];
    int port;
    
    ogs_list_for_each(&ogs_app()->cli_list, node) {
        addr = node->addr;
        port = OGS_PORT(addr);
        addr->ogs_sin_port = htobe16(be16toh(addr->ogs_sin_port) + 1);
        ogs_info("cli port change from %u to %u.\r\n", port,OGS_PORT(addr));
    }    
}

int amf_sps_initialize()
{
    int rv;
    //setAffinity(6);
    amf_metrics_init();
    ogs_sbi_context_init(OpenAPI_nf_type_AMF);

    amf_context_init();    

    rv = license_check_init();
    if (rv != OGS_OK) return rv;
    
    rv = ogs_sbi_context_parse_config("amf", "nrf", "scp");
    if (rv != OGS_OK) return rv;

    rv = ogs_metrics_context_parse_config("amf");
    if (rv != OGS_OK) return rv;

    rv = amf_context_parse_config(false);
    if (rv != OGS_OK) return rv;

    rv = amf_context_nf_info(false);
    if (rv != OGS_OK) return rv;

    //rv = amf_m_tmsi_pool_generate();
    //if (rv != OGS_OK) return rv;

    ogs_metrics_context_open(ogs_metrics_self());

    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) return rv;

    //rv = amf_sps_sbi_open();
    //if (rv != OGS_OK) return rv;
    rv = amf_sbi_open();
    if (rv != OGS_OK) return rv;

    rv = udp_ini_open();
    if (rv != OGS_OK) return rv;

    /*启动yaml配置检测定时器*/
    if (rv != OGS_OK) return rv;
    rv = ogs_yaml_check_init();

    thread = ogs_thread_create(amf_sps_main, NULL);
    if (!thread) return OGS_ERROR;
    
    setCommands();
    amf_sps_update_cli_port();
    cli_thread = ogs_thread_create(telnetMain, &ogs_app()->cli_list);
    if (!cli_thread) return OGS_ERROR;

    initialized = 1;

    return OGS_OK;
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
    ogs_info("amf_terminate initialized%d.",initialized);
    if (!initialized) return;

    /* Daemon terminating */
    event_termination();    
    ogs_free(cli_thread);
    ogs_thread_destroy(thread);
    ogs_timer_delete(t_termination_holding);

    ngap_close();
    amf_sbi_close();

    ogs_metrics_context_close(ogs_metrics_self());

    amf_context_final();
    ogs_sbi_context_final();

    amf_metrics_final();

    udp_ini_close();
}

static void amf_main(void *data)
{
    ogs_fsm_t amf_sm;
    int rv;

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
            ogs_info("ogs_queue_trypop,rv:%d.",rv);
            if (rv == OGS_DONE){
                ogs_info("ogs done");
                goto done;
            }
            
            if (rv == OGS_RETRY){
                ogs_info("ogs OGS_RETRY");
                
                break;
            }

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