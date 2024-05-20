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

#define _GNU_SOURCE
#include "context.h"
#include "gtp-path.h"
#include "pfcp-path.h"
#include "metrics.h"
#include "license.h"
#include "ogs-app-timer.h"
#include "telnet.h"
#if defined(USE_DPDK)
#include <unistd.h>
#include <sched.h>
#include "upf-dpdk.h"
#include "ctrl-path.h"
#endif

static ogs_thread_t *thread;
static ogs_thread_t *cli_thread;
static void upf_main(void *data);
void setCommands(void);
static int initialized = 0;

#if defined(USE_DPDK)
static void bind_core(int core)
{
    int cpus = 0;
    cpu_set_t mask;

    cpus = sysconf(_SC_NPROCESSORS_ONLN);
    printf("cpus: %d\n", cpus);

    CPU_ZERO(&mask);
    CPU_SET(core, &mask);
    if (sched_setaffinity(core, sizeof(mask), &mask) == -1) {
        printf("Set CPU affinity failue, ERROR:%s\n", strerror(errno));
        return ;
    }
}
#endif

int upf_initialize(void)
{
    int rv;

#define APP_NAME "upf"
    rv = ogs_app_parse_local_conf(APP_NAME);
    if (rv != OGS_OK) return rv;

    upf_metrics_init();

    ogs_gtp_context_init(OGS_MAX_NUM_OF_GTPU_RESOURCE);
    ogs_pfcp_context_init();

    upf_context_init();
    upf_event_init();
    upf_gtp_init();

    rv = license_check_init();
    if (rv != OGS_OK) return rv;


    rv = ogs_pfcp_xact_init();
    if (rv != OGS_OK) return rv;

    rv = ogs_gtp_context_parse_config(APP_NAME, "smf");
    if (rv != OGS_OK) return rv;

    rv = ogs_pfcp_context_parse_config(APP_NAME, "smf");
    if (rv != OGS_OK) return rv;

    rv = ogs_metrics_context_parse_config(APP_NAME);
    if (rv != OGS_OK) return rv;

    rv = upf_context_parse_config();
    if (rv != OGS_OK) return rv;

#if defined(USE_DPDK)
    rv = upf_dpdk_context_parse_config();
    if (rv != OGS_OK) return rv;
#endif

    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) return rv;

    rv = ogs_pfcp_ue_pool_generate();
    if (rv != OGS_OK) return rv;

    ogs_metrics_context_open(ogs_metrics_self());

    rv = upf_pfcp_open();
    if (rv != OGS_OK) return rv;

#if defined(USE_DPDK)
    rv = upf_dpdk_open();
#else
    rv = upf_gtp_open();
#endif
    if (rv != OGS_OK) return rv;

    /*启动yaml配置检测定时器*/
    rv = ogs_yaml_check_init();
    if (rv != OGS_OK) return rv;
    
    thread = ogs_thread_create(upf_main, NULL);
    if (!thread) return OGS_ERROR;
    
    setCommands();
    cli_thread = ogs_thread_create(telnetMain, &ogs_app()->cli_list);
    initialized = 1;

    return OGS_OK;
}

void upf_terminate(void)
{
    if (!initialized) return;

    ogs_free(cli_thread);

    upf_event_term();

    ogs_thread_destroy(thread);

    upf_pfcp_close();

#if defined(USE_DPDK)
    upf_dpdk_close();
#else
    upf_gtp_close();
#endif

    ogs_metrics_context_close(ogs_metrics_self());

    upf_context_final();

    ogs_pfcp_context_final();
    ogs_gtp_context_final();

    ogs_pfcp_xact_final();

    upf_gtp_final();
    upf_event_final();

    upf_metrics_final();
}

#if defined(USE_DPDK)
static void dkuf_update_time(void)
{
    static uint8_t init = 1;
    static uint64_t last_sec = 0;
    if (init) {
        last_sec = dkuf.sys_up_sec;
        init = 0;
    }

    dkuf.sys_up_ms = rte_rdtsc() / dkuf.ticks_per_ms;
    dkuf.sys_up_sec = dkuf.sys_up_ms >> 10;
    if (unlikely(dkuf.sys_up_sec - last_sec >= 10)) {
        print_stat();
        send_garp();
        last_sec = dkuf.sys_up_sec;
    }
}
#endif

static void upf_main(void *data)
{
    ogs_fsm_t upf_sm;
    int rv;

#if defined(USE_DPDK)
    //bind core pfcp;
    bind_core(dkuf.pfcp_lcore);
#endif

    ogs_fsm_init(&upf_sm, upf_state_initial, upf_state_final, 0);

    for ( ;; ) {
#if defined(USE_DPDK)
        dkuf_update_time();
#endif

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
            upf_event_t *e = NULL;

            rv = ogs_queue_trypop(ogs_app()->queue, (void**)&e);
            ogs_assert(rv != OGS_ERROR);

            if (rv == OGS_DONE)
                goto done;

            if (rv == OGS_RETRY)
                break;

            ogs_assert(e);
            ogs_fsm_dispatch(&upf_sm, e);
            if (e->id != OGS_EVENT_APP_CHECK_TIMER){
                upf_event_free(e);
            }else{
                ogs_event_free(e);//OGS_EVENT_APP_CHECK_TIMER 统一使用ogs_event_new生成
            }
        }

#if defined(USE_DPDK)
        // handle dpdk event
        upf_dpdk_loop_event();
#endif
    }
done:

    ogs_fsm_fini(&upf_sm, 0);
}
