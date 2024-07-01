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

//#include "ogs-sctp.h"
#include "context.h"

#include "nbr-sctp.h"
#include <linux/filter.h>
#define NBR_CLIENT_LOOP_INTERVAL    60

static void lksctp_accept_handler(short when, ogs_socket_t fd, void *data);
static void lksctp_recv_handler(short when, ogs_socket_t fd, void *data);

void nbr_accept_handler(ogs_sock_t *sock);
void nbr_recv_remoteclient_handler(ogs_sock_t *sock);
void nbr_recv_remoteserver_handler(ogs_sock_t *sock);

ogs_sock_t *nbr_server(ogs_socknode_t *node)
{
    char buf[OGS_ADDRSTRLEN];
    ogs_sock_t *sock = NULL;

    ogs_assert(node);

    sock = ogs_sctp_server(SOCK_STREAM, node->addr, node->option);
    if (!sock) return NULL;
    node->poll = ogs_pollset_add(ogs_app()->pollset,
            OGS_POLLIN, sock->fd, lksctp_accept_handler, sock);
    ogs_assert(node->poll);

    node->sock = sock;//新增,否则内存泄露

    ogs_info("nbr_server() [%s]:%d",
            OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));

    return sock;
}

ogs_sock_t *nbr_client(ogs_socknode_t *node)
{
    char buf[OGS_ADDRSTRLEN];
    ogs_sock_t *sock = NULL;

    ogs_assert(node);

    sock = ogs_sctp_client(SOCK_STREAM, node->addr, node->option);
    if (sock) {
        node->poll = ogs_pollset_add(ogs_app()->pollset,
                OGS_POLLIN, sock->fd, lksctp_recv_handler, sock);
        ogs_assert(node->poll);
        ogs_info("nbr client() [%s]:%d",
                OGS_ADDR(&sock->remote_addr, buf), OGS_PORT(&sock->remote_addr));
		ogs_sockaddr_t *addr = NULL;

        addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
        ogs_assert(addr);
        memcpy(addr, &sock->remote_addr, sizeof(ogs_sockaddr_t));
		nbr_event_push(UPF_EVT_NBR_LO_CONNECTED,
                sock, addr, NULL, 0, 0);
    }

    return sock;
}


ogs_sock_t *nbr_client_byip(ogs_socknode_t *node, ogs_socknode_t *localnode)
{
    char buf[OGS_ADDRSTRLEN];
	char buf1[OGS_ADDRSTRLEN];
    ogs_sock_t *sock = NULL;

    ogs_assert(node);

	ogs_info("nbr_client_byip current local addr [%s]:%d",
                OGS_ADDR(localnode->addr, buf1), OGS_PORT(localnode->addr));
    sock = ogs_sctp_client_byip(SOCK_STREAM, node, localnode);
    if (sock) {
        node->poll = ogs_pollset_add(ogs_app()->pollset,
                OGS_POLLIN, sock->fd, lksctp_recv_handler, sock);
        ogs_assert(node->poll);
        ogs_info("nbr client remote server() [%s]:%d",
                OGS_ADDR(&sock->remote_addr, buf), OGS_PORT(&sock->remote_addr));
		ogs_sockaddr_t *addr = NULL;

        addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
        ogs_assert(addr);
        memcpy(addr, &sock->remote_addr, sizeof(ogs_sockaddr_t));
		nbr_event_push(UPF_EVT_NBR_LO_CONNECTED,
                sock, addr, NULL, 0, 0);
    }

    return sock;
}

void nbr_recvremotclient_upcall(short when, ogs_socket_t fd, void *data)
{
    ogs_sock_t *sock = NULL;

    ogs_assert(fd != INVALID_SOCKET);
    sock = data;
    ogs_assert(sock);

    nbr_recv_remoteclient_handler(sock);
}

static void lksctp_accept_handler(short when, ogs_socket_t fd, void *data)
{
    ogs_assert(data);
    ogs_assert(fd != INVALID_SOCKET);

    nbr_accept_handler(data);
}

static void lksctp_recv_handler(short when, ogs_socket_t fd, void *data)
{
    ogs_sock_t *sock = NULL;

    sock = data;
    ogs_assert(fd != INVALID_SOCKET);
    ogs_assert(sock);
    
    nbr_recv_remoteserver_handler(sock);
}

void nbr_accept_handler(ogs_sock_t *sock)
{
    char buf[OGS_ADDRSTRLEN];
    ogs_sock_t *new = NULL;

    ogs_assert(sock);

    new = ogs_sock_accept(sock);
    if (new) {
        ogs_sockaddr_t *addr = NULL;

        addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
        ogs_assert(addr);
        memcpy(addr, &new->remote_addr, sizeof(ogs_sockaddr_t));

        ogs_info("nbr remote client accepted[%s]:%d in nbr sctp module",
            OGS_ADDR(addr, buf), OGS_PORT(addr));

        nbr_event_push(UPF_EVT_NBR_LO_ACCEPT,
                new, addr, NULL, 0, 0);
    } else {
        ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno, "accept() failed");
    }
}

void nbr_recv_remoteclient_handler(ogs_sock_t *sock)
{
    ogs_pkbuf_t *pkbuf;
    int size;
    ogs_sockaddr_t *addr = NULL;
    ogs_sockaddr_t from;
    ogs_sctp_info_t sinfo;
    int flags = 0;

    ogs_assert(sock);

    pkbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
    ogs_assert(pkbuf);
    ogs_pkbuf_put(pkbuf, OGS_MAX_SDU_LEN);
    size = ogs_sctp_recvmsg(
            sock, pkbuf->data, pkbuf->len, &from, &sinfo, &flags);
    if (size < 0 || size >= OGS_MAX_SDU_LEN) {
        ogs_error("ogs_sctp_recvmsg(%d) failed(%d:%s)",
                size, errno, strerror(errno));
        ogs_pkbuf_free(pkbuf);
        return;
    }

    if (flags & MSG_NOTIFICATION) {
        union sctp_notification *not =
            (union sctp_notification *)pkbuf->data;

        switch(not->sn_header.sn_type) {
        case SCTP_ASSOC_CHANGE :
            ogs_debug("SCTP_ASSOC_CHANGE:"
                    "[T:%d, F:0x%x, S:%d, I/O:%d/%d]", 
                    not->sn_assoc_change.sac_type,
                    not->sn_assoc_change.sac_flags,
                    not->sn_assoc_change.sac_state,
                    not->sn_assoc_change.sac_inbound_streams,
                    not->sn_assoc_change.sac_outbound_streams);

            if (not->sn_assoc_change.sac_state == SCTP_COMM_UP) {
                ogs_debug("SCTP_COMM_UP");

                addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
                ogs_assert(addr);
                memcpy(addr, &from, sizeof(ogs_sockaddr_t));

                nbr_event_push(UPF_EVT_NBR_REMOTECLIENT_LO_SCTP_COMM_UP,
                        sock, addr, NULL,
                        not->sn_assoc_change.sac_inbound_streams,
                        not->sn_assoc_change.sac_outbound_streams);
            } else if (not->sn_assoc_change.sac_state == SCTP_SHUTDOWN_COMP ||
                    not->sn_assoc_change.sac_state == SCTP_COMM_LOST) {

                if (not->sn_assoc_change.sac_state == SCTP_SHUTDOWN_COMP)
                    ogs_debug("SCTP_SHUTDOWN_COMP");
                if (not->sn_assoc_change.sac_state == SCTP_COMM_LOST)
                    ogs_debug("SCTP_COMM_LOST");

                addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
                ogs_assert(addr);
                memcpy(addr, &from, sizeof(ogs_sockaddr_t));

                nbr_event_push(UPF_EVT_NBR_REMOTECLIENT_LO_CONNREFUSED,
                        sock, addr, NULL, 0, 0);
            }
            break;
        case SCTP_SHUTDOWN_EVENT :
            ogs_debug("SCTP_SHUTDOWN_EVENT:[T:%d, F:0x%x, L:%d]",
                    not->sn_shutdown_event.sse_type,
                    not->sn_shutdown_event.sse_flags,
                    not->sn_shutdown_event.sse_length);
            addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
            ogs_assert(addr);
            memcpy(addr, &from, sizeof(ogs_sockaddr_t));

            nbr_event_push(UPF_EVT_NBR_REMOTECLIENT_LO_CONNREFUSED,
                    sock, addr, NULL, 0, 0);
            break;

        case SCTP_SEND_FAILED :
            ogs_error("SCTP_SEND_FAILED:[T:%d, F:0x%x, S:%d]",
                    not->sn_send_failed.ssf_type,
                    not->sn_send_failed.ssf_flags,
                    not->sn_send_failed.ssf_error);
            break;

        case SCTP_PEER_ADDR_CHANGE:
            ogs_warn("SCTP_PEER_ADDR_CHANGE:[T:%d, F:0x%x, S:%d]", 
                    not->sn_paddr_change.spc_type,
                    not->sn_paddr_change.spc_flags,
                    not->sn_paddr_change.spc_error);
            break;
        case SCTP_REMOTE_ERROR:
            ogs_warn("SCTP_REMOTE_ERROR:[T:%d, F:0x%x, S:%d]", 
                    not->sn_remote_error.sre_type,
                    not->sn_remote_error.sre_flags,
                    not->sn_remote_error.sre_error);
            break;
        default :
            ogs_error("Discarding event with unknown flags:0x%x type:0x%x",
                    flags, not->sn_header.sn_type);
            break;
        }
    } else if (flags & MSG_EOR) {
        ogs_pkbuf_trim(pkbuf, size);

        addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
        ogs_assert(addr);
        memcpy(addr, &from, sizeof(ogs_sockaddr_t));

        nbr_event_push(UPF_EVT_NBR_REMOTECLIENT_MESSAGE, sock, addr, pkbuf, 0, 0);
        return;
    } else {
        ogs_fatal("Invalid flag(0x%x)", flags);
        //ogs_assert_if_reached();
    }

    ogs_pkbuf_free(pkbuf);
}

void nbr_recv_remoteserver_handler(ogs_sock_t *sock)
{
    ogs_pkbuf_t *pkbuf;
    int size;
    ogs_sockaddr_t *addr = NULL;
    ogs_sockaddr_t from;
    ogs_sctp_info_t sinfo;
    int flags = 0;

    ogs_assert(sock);

    pkbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
    ogs_assert(pkbuf);
    ogs_pkbuf_put(pkbuf, OGS_MAX_SDU_LEN);
    size = ogs_sctp_recvmsg(
            sock, pkbuf->data, pkbuf->len, &from, &sinfo, &flags);
    if (size < 0 || size >= OGS_MAX_SDU_LEN) {
        ogs_error("nbr_recv_remoteserver_handler(%d) failed(%d:%s)",
                size, errno, strerror(errno));
        ogs_pkbuf_free(pkbuf);
        return;
    }

    if (flags & MSG_NOTIFICATION) {
        union sctp_notification *not =
            (union sctp_notification *)pkbuf->data;

        switch(not->sn_header.sn_type) {
        case SCTP_ASSOC_CHANGE :
            ogs_debug("SCTP_ASSOC_CHANGE:"
                    "[T:%d, F:0x%x, S:%d, I/O:%d/%d]", 
                    not->sn_assoc_change.sac_type,
                    not->sn_assoc_change.sac_flags,
                    not->sn_assoc_change.sac_state,
                    not->sn_assoc_change.sac_inbound_streams,
                    not->sn_assoc_change.sac_outbound_streams);

            if (not->sn_assoc_change.sac_state == SCTP_COMM_UP) {
                ogs_debug("SCTP_COMM_UP");

                addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
                ogs_assert(addr);
                memcpy(addr, &from, sizeof(ogs_sockaddr_t));

                nbr_event_push(UPF_EVT_NBR_REMOTESERVER_LO_SCTP_COMM_UP,
                        sock, addr, NULL,
                        not->sn_assoc_change.sac_inbound_streams,
                        not->sn_assoc_change.sac_outbound_streams);
            } else if (not->sn_assoc_change.sac_state == SCTP_SHUTDOWN_COMP ||
                    not->sn_assoc_change.sac_state == SCTP_COMM_LOST) {

                if (not->sn_assoc_change.sac_state == SCTP_SHUTDOWN_COMP)
                    ogs_debug("SCTP_SHUTDOWN_COMP");
                if (not->sn_assoc_change.sac_state == SCTP_COMM_LOST)
                    ogs_debug("SCTP_COMM_LOST");

                addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
                ogs_assert(addr);
                memcpy(addr, &from, sizeof(ogs_sockaddr_t));

                nbr_event_push(UPF_EVT_NBR_REMOTESERVER_LO_CONNREFUSED,
                        sock, addr, NULL, 0, 0);
            }
            break;
        case SCTP_SHUTDOWN_EVENT :
        case SCTP_SEND_FAILED :
            if (not->sn_header.sn_type == SCTP_SHUTDOWN_EVENT)
                ogs_debug("SCTP_SHUTDOWN_EVENT:[T:%d, F:0x%x, L:%d]", 
                        not->sn_shutdown_event.sse_type,
                        not->sn_shutdown_event.sse_flags,
                        not->sn_shutdown_event.sse_length);
            if (not->sn_header.sn_type == SCTP_SEND_FAILED)
                ogs_error("SCTP_SEND_FAILED:[T:%d, F:0x%x, S:%d]", 
                        not->sn_send_failed.ssf_type,
                        not->sn_send_failed.ssf_flags,
                        not->sn_send_failed.ssf_error);

            addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
            ogs_assert(addr);
            memcpy(addr, &from, sizeof(ogs_sockaddr_t));

            nbr_event_push(UPF_EVT_NBR_REMOTESERVER_LO_CONNREFUSED,
                    sock, addr, NULL, 0, 0);
            break;
        case SCTP_PEER_ADDR_CHANGE:
            ogs_warn("SCTP_PEER_ADDR_CHANGE:[T:%d, F:0x%x, S:%d]", 
                    not->sn_paddr_change.spc_type,
                    not->sn_paddr_change.spc_flags,
                    not->sn_paddr_change.spc_error);
            break;
        case SCTP_REMOTE_ERROR:
            ogs_warn("SCTP_REMOTE_ERROR:[T:%d, F:0x%x, S:%d]", 
                    not->sn_remote_error.sre_type,
                    not->sn_remote_error.sre_flags,
                    not->sn_remote_error.sre_error);
            break;
        default :
            ogs_error("Discarding event with unknown flags:0x%x type:0x%x",
                    flags, not->sn_header.sn_type);
            break;
        }
    } else if (flags & MSG_EOR) {
        ogs_pkbuf_trim(pkbuf, size);

        addr = ogs_calloc(1, sizeof(ogs_sockaddr_t));
        ogs_assert(addr);
        memcpy(addr, &from, sizeof(ogs_sockaddr_t));

        nbr_event_push(UPF_EVT_NBR_REMOTESERVER_MESSAGE, sock, addr, pkbuf, 0, 0);
        return;
    } else {
        ogs_fatal("Invalid flag(0x%x)", flags);
        //ogs_assert_if_reached();
    }
    ogs_pkbuf_free(pkbuf);
}

void nbr_client_check_timer_cb(void *data)
{
    ogs_socknode_t *node = NULL;
    uint16_t index = 0;
	upf_remoteserver_t *remoteserver = NULL;

	ogs_error("nbr_client_check_timer_cb time out");
	ogs_socknode_t *node2 = NULL;
	ogs_list_for_each(&upf_self()->nbrlocalclient_list, node2)
	{
		break;
	}
	if(!node2)
	{
	    return;
	}
	ogs_list_for_each(&upf_self()->nbrremoteserver_list, node)
	{
	    remoteserver = upf_remoteserver_find_by_remoteclient_ipaddr(node->addr->sin.sin_addr.s_addr);
		if(!remoteserver)
		{
		    ogs_error("current node not connect to remote server 0x%x, index %d, clientportbegin %d",
                    node->addr->sin.sin_addr.s_addr, index, upf_self()->clientportbegin);
			node2->addr->ogs_sin_port = htobe16(upf_self()->clientportbegin) + htobe16(index);
		    if (nbr_client_byip(node, node2) == NULL)
		    {
				ogs_error("Connect to remote server failed");
		    }
		}
		index++;
	}
	ogs_timer_start(upf_self()->nbr_timer, ogs_time_from_sec(NBR_CLIENT_LOOP_INTERVAL));
}

void _gtpv1_nbr_recv_common_cb(
        short when, ogs_socket_t fd, void *data);

int nbr_open(void)
{
    ogs_socknode_t *node = NULL;
    ogs_socknode_t *node2 = NULL;
	uint16_t index = 0;
    ogs_list_for_each(&upf_self()->nbrlocalserver_list, node)
    {
        if (nbr_server(node) == NULL)
        {
			continue;
        }
    }
    
	ogs_list_for_each(&upf_self()->nbrlocalclient_list, node2)
    {
        break;
    }
    if(!node2)
    {
        ogs_info("nbrlocalclient_list is null");
        return OGS_OK;
    }
	ogs_info("upf_self()->clientportbegin is 0x%x", upf_self()->clientportbegin);
    ogs_list_for_each(&upf_self()->nbrremoteserver_list, node)
    {
        ogs_info("nbrremoteserver_list index is %d", index);
        node2->addr->ogs_sin_port = htobe16(upf_self()->clientportbegin) + htobe16(index);
        if (nbr_client_byip(node, node2) == NULL)
        {
			//continue;
        }
		index++;
    }

	if(node2)
	{
	    ogs_info("start nbr client check timer");
		upf_self()->nbr_timer = ogs_timer_add(ogs_app()->timer_mgr, nbr_client_check_timer_cb, NULL);
	    ogs_assert(upf_self()->nbr_timer);

	    ogs_timer_start(upf_self()->nbr_timer, ogs_time_from_sec(NBR_CLIENT_LOOP_INTERVAL));
	}

    upf_self()->nbr_rawsocket = socket(AF_INET, SOCK_RAW, IPPROTO_IPIP);
    if (upf_self()->nbr_rawsocket < 0) {
        perror("Error creating socket");
        exit(1);
    }
    
    ogs_pollset_add(ogs_app()->pollset,
                OGS_POLLIN, upf_self()->nbr_rawsocket, _gtpv1_nbr_recv_common_cb, upf_self()->nbr_rawsocket);
    ogs_info("add raw socket success。fd:%d.",upf_self()->nbr_rawsocket);
    return OGS_OK;
}


void nbr_close()
{
    ogs_socknode_remove_all(&upf_self()->nbrlocalserver_list);
    ogs_socknode_remove_all(&upf_self()->nbrlocalclient_list);
    ogs_socknode_remove_all(&upf_self()->nbrremoteserver_list);
}
int nbr_open_single_client(ogs_socknode_t *node, uint16_t index)
{
    ogs_socknode_t *node2 = NULL;
	ogs_list_for_each(&upf_self()->nbrlocalclient_list, node2)
	{
		break;
	}
    if(node2)
    {
        node2->addr->ogs_sin_port = htobe16(upf_self()->clientportbegin) + htobe16(index);
		ogs_error("index is %d, port is 0x%x", index, node2->addr->ogs_sin_port);
    }
	else
	{
	    ogs_error("local client not exist,return");
		return OGS_OK;
	}
    if (nbr_client_byip(node, node2) == NULL)
    {
		ogs_error("Connect to remote server failed");
    }

    return OGS_OK;
}

int nbr_open_client_by_ip(uint32_t addr)
{
    ogs_socknode_t *node = NULL;
    uint16_t index = 0;
	ogs_list_for_each(&upf_self()->nbrremoteserver_list, node)
	{
	    if(node->addr->sin.sin_addr.s_addr == addr)
	    {
	        nbr_open_single_client(node, index);
			break;
	    }
		index++;
	}
	return  OGS_OK;
}
