/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
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

#include "upf-sm.h"
#include "context.h"
#include "event.h"
#include "pfcp-path.h"
#include "gtp-path.h"
#include "ogs-app-timer.h"
#include "license.h"

void upf_state_initial(ogs_fsm_t *s, upf_event_t *e)
{
    upf_sm_debug(e);

    ogs_assert(s);

    OGS_FSM_TRAN(s, &upf_state_operational);
}

void upf_state_final(ogs_fsm_t *s, upf_event_t *e)
{
    upf_sm_debug(e);

    ogs_assert(s);
}

void upf_state_operational(ogs_fsm_t *s, upf_event_t *e)
{
    int rv;
    ogs_pkbuf_t *recvbuf = NULL;

    ogs_pfcp_message_t *pfcp_message = NULL;
    ogs_pfcp_node_t *node = NULL;
    ogs_pfcp_xact_t *xact = NULL;
    int license_state;

    upf_sm_debug(e);

    ogs_assert(s);

    switch (e->id) {
    case OGS_FSM_ENTRY_SIG:
        break;

    case OGS_FSM_EXIT_SIG:
        break;

    case UPF_EVT_N4_MESSAGE:
        ogs_assert(e);
        recvbuf = e->pkbuf;
        ogs_assert(recvbuf);
        node = e->pfcp_node;
        ogs_assert(node);

        /*
         * Issue #1911
         *
         * Because ogs_pfcp_message_t is over 80kb in size,
         * it can cause stack overflow.
         * To avoid this, the pfcp_message structure uses heap memory.
         */
        if ((pfcp_message = ogs_pfcp_parse_msg(recvbuf)) == NULL) {
            ogs_error("ogs_pfcp_parse_msg() failed");
            ogs_pkbuf_free(recvbuf);
            break;
        }

        rv = ogs_pfcp_xact_receive(node, &pfcp_message->h, &xact);
        if (rv != OGS_OK) {
            ogs_pkbuf_free(recvbuf);
            ogs_pfcp_message_free(pfcp_message);
            break;
        }

        e->pfcp_message = pfcp_message;
        e->pfcp_xact = xact;
        ogs_fsm_dispatch(&node->sm, e);
        if (OGS_FSM_CHECK(&node->sm, upf_pfcp_state_exception)) {
            ogs_error("PFCP state machine exception");
        }

        ogs_pkbuf_free(recvbuf);
        ogs_pfcp_message_free(pfcp_message);
        break;
    case UPF_EVT_N4_TIMER:
    case UPF_EVT_N4_NO_HEARTBEAT:
        node = e->pfcp_node;
        ogs_assert(node);
        ogs_assert(OGS_FSM_STATE(&node->sm));

        ogs_fsm_dispatch(&node->sm, e);
        break;
    case OGS_EVENT_APP_CHECK_TIMER:
        ogs_assert(e);

        switch(e->timer_id) {
        case OGS_TIMER_LICENSE_CHECK:
            license_state = checkLicenseAfterRuntime(LICENSE_CHECK_INTERVAL,30);
            ogs_info("license state:%s, runtime:%lu, durationtime:%lu, expireTime:%s.", 
                    get_license_state_name(license_state),
                    getLicenseRunTime(),
                    getLicenseDurationTime(),
                    timestampToString(getLicenseExpireTime()));
            upf_metrics_report_license_alarm(license_state);
            if (license_state == LICENSE_STATE_SOON_TO_EXPIRE){
                ogs_warn("license soon to expire.");
            }else if (license_state == LICENSE_STATE_EXPIRED){
                ogs_fatal("license expired.");
                exit(0);
            }

            license_check_restart();
            break;
        case OGS_TIMER_YAML_CONFIG_CHECK:
            yaml_check_proc();
            ogs_yaml_check_restart();
            break;

        default:
            ogs_error("Unknown timer[%s:%d]",
                    ogs_timer_get_name(e->timer_id), e->timer_id);
        }
        break;
    default:
        ogs_error("No handler for event %s", upf_event_get_name(e));
        break;
    }
}
