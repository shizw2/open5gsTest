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

#ifndef PCF_EVENT_H
#define PCF_EVENT_H

#include "ogs-proto.h"
#include "ogs-diameter-rx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pcf_ue_s pcf_ue_t;
typedef struct pcf_sess_s pcf_sess_t;
typedef struct pcf_app_s pcf_app_t;

typedef enum {
    PCF_EVENT_BASE = OGS_MAX_NUM_OF_PROTO_EVENT,

    PCF_EVENT_RX_CMD,
    MAX_NUM_OF_PCF_EVENT,

} pcf_event_e;

typedef struct pcf_event_s {
    ogs_event_t h;

    pcf_ue_t *pcf_ue;
    pcf_sess_t *sess;
    pcf_app_t *app;
    ogs_diam_rx_message_t *rx_message;
} pcf_event_t;

OGS_STATIC_ASSERT(OGS_EVENT_SIZE >= sizeof(pcf_event_t));

pcf_event_t *pcf_event_new(int id);

const char *pcf_event_get_name(pcf_event_t *e);

#ifdef __cplusplus
}
#endif

#endif /* PCF_EVENT_H */
