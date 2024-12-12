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

#ifndef SACC_TIMER_H
#define SACC_TIMER_H

#include "ogs-proto.h"

#ifdef __cplusplus
extern "C" {
#endif

/* forward declaration */
typedef enum {
    SACC_TIMER_BASE = OGS_MAX_NUM_OF_PROTO_TIMER,

    SACC_TIMER_NODE_HANDSHAKE,

    MAX_NUM_OF_SACC_TIMER,
} sacc_timer_e;

typedef struct sacc_timer_cfg_s {
    bool have;
    int max_count;
    ogs_time_t duration;
} sacc_timer_cfg_t;

sacc_timer_cfg_t *sacc_timer_cfg(sacc_timer_e id);

const char *sacc_timer_get_name(int timer_id);

void sacc_timer_node_handshake_timer_expire(void *data);
#ifdef __cplusplus
}
#endif

#endif /* SACC_TIMER_H */
