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

#include "context.h"


const char *sacc_timer_get_name(int timer_id)
{
    switch (timer_id) {    
    case SACC_TIMER_NODE_HANDSHAKE:
        return "SACC_TIMER_NODE_HANDSHAKE";
    default: 
        break;
    }

    ogs_error("Unknown Timer[%d]", timer_id);
    return "UNKNOWN_TIMER";
}

void sacc_timer_node_handshake_timer_expire(void *data)
{
    int rv;
    sacc_event_t *e = NULL;

    //ogs_assert(data);
  
    e = sacc_event_new(SACC_EVENT_TIMER);
    ogs_assert(e);

    e->h.timer_id = SACC_TIMER_NODE_HANDSHAKE;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        ogs_event_free(e);
    }
}
