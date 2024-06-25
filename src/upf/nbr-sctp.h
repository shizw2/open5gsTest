/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
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

#ifndef NBR_PATH_H
#define NBR_PATH_H

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif


#define nbr_event_push  upf_sctp_event_push

int nbr_open(void);
void nbr_close(void);

ogs_sock_t *nbr_server(ogs_socknode_t *node);
ogs_sock_t *nbr_client(ogs_socknode_t *node);

void nbr_recvremotclient_upcall(short when, ogs_socket_t fd, void *data);
int nbr_send(ogs_sock_t *sock,
        ogs_pkbuf_t *pkbuf, ogs_sockaddr_t *addr, uint16_t stream_no);


int nbr_open_client_by_ip(uint32_t addr);
int nbr_open_single_client(ogs_socknode_t *node);

#ifdef __cplusplus
}
#endif

#endif /* NGAP_PATH_H */
