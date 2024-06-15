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

#include "ogs-gtp.h"

ogs_sock_t *ogs_gtp_server(ogs_socknode_t *node)
{
    char buf[OGS_ADDRSTRLEN];
    ogs_sock_t *gtp;
    ogs_assert(node);

    gtp = ogs_udp_server(node->addr, node->option);
    if (gtp) {
        ogs_info("gtp_server() [%s]:%d",
                OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));

        node->sock = gtp;
    }

    return gtp;
}

int ogs_gtp_connect(ogs_sock_t *ipv4, ogs_sock_t *ipv6, ogs_gtp_node_t *gnode)
{
    ogs_sockaddr_t *addr;
    char buf[OGS_ADDRSTRLEN];

    ogs_assert(ipv4 || ipv6);
    ogs_assert(gnode);
    ogs_assert(gnode->sa_list);

    addr = gnode->sa_list;
    while (addr) {
        ogs_sock_t *sock = NULL;

        if (addr->ogs_sa_family == AF_INET)
            sock = ipv4;
        else if (addr->ogs_sa_family == AF_INET6)
            sock = ipv6;
        else
            ogs_assert_if_reached();

        if (sock) {
            ogs_info("gtp_connect() [%s]:%d",
                    OGS_ADDR(addr, buf), OGS_PORT(addr));

            gnode->sock = sock;
            memcpy(&gnode->addr, addr, sizeof gnode->addr);
            break;
        }

        addr = addr->next;
    }

    if (addr == NULL) {
        ogs_log_message(OGS_LOG_WARN, ogs_socket_errno,
                "gtp_connect() [%s]:%d failed",
                OGS_ADDR(gnode->sa_list, buf), OGS_PORT(gnode->sa_list));
        return OGS_ERROR;
    }

    return OGS_OK;
}

int ogs_gtp_send(ogs_gtp_node_t *gnode, ogs_pkbuf_t *pkbuf)
{
    ssize_t sent;
    ogs_sock_t *sock = NULL;

    ogs_assert(gnode);
    ogs_assert(pkbuf);
    sock = gnode->sock;
    ogs_assert(sock);

    sent = ogs_send(sock->fd, pkbuf->data, pkbuf->len, 0);
    if (sent < 0 || sent != pkbuf->len) {
        if (ogs_socket_errno != OGS_EAGAIN) {
            ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno,
                    "ogs_gtp_send() failed");
        }
        return OGS_ERROR;
    }

    return OGS_OK;
}

int ogs_gtp_sendto(ogs_gtp_node_t *gnode, ogs_pkbuf_t *pkbuf)
{
    ssize_t sent;
    ogs_sock_t *sock = NULL;
    ogs_sockaddr_t *addr = NULL;

    ogs_assert(gnode);
    ogs_assert(pkbuf);
    sock = gnode->sock;
    ogs_assert(sock);
    addr = &gnode->addr;
    ogs_assert(addr);

    sent = ogs_sendto(sock->fd, pkbuf->data, pkbuf->len, 0, addr);
    if (sent < 0 || sent != pkbuf->len) {
        if (ogs_socket_errno != OGS_EAGAIN) {
            char buf[OGS_ADDRSTRLEN];
            int err = ogs_socket_errno;
            ogs_log_message(OGS_LOG_ERROR, err,
                    "ogs_gtp_sendto(%u, %p, %u, 0, %s:%u) failed",
                    sock->fd, pkbuf->data, pkbuf->len,
                    OGS_ADDR(addr, buf), OGS_PORT(addr));
        }
        return OGS_ERROR;
    }

    return OGS_OK;
}

void ogs_gtp_send_error_message(
        ogs_gtp_xact_t *xact, uint32_t teid, uint8_t type, uint8_t cause_value)
{
    switch (xact->gtp_version) {
    case 1:
        ogs_gtp1_send_error_message(xact, teid, type, cause_value);
        break;
    case 2:
        ogs_gtp2_send_error_message(xact, teid, type, cause_value);
        break;
    }
}

#define MAX_CACHE_SIZE 20  // 缓存区最大报文数量
// 全局缓存区，用于存储待发送的报文
ogs_pkbuf_t *cache[MAX_CACHE_SIZE];
size_t cache_size = 0;


int ogs_gtp_sendmmsg(ogs_gtp_node_t *gnode, ogs_pkbuf_t **pkbufs, size_t num_pkbufs) {
    if (gnode == NULL || pkbufs == NULL || num_pkbufs == 0) {
        return OGS_ERROR;
    }

    ssize_t sent;
    struct mmsghdr mmsg[MAX_CACHE_SIZE];
    struct iovec iov[MAX_CACHE_SIZE];
    size_t i;
    size_t max_msgs = (num_pkbufs > MAX_CACHE_SIZE) ? MAX_CACHE_SIZE : num_pkbufs;

    for (i = 0; i < max_msgs; ++i) {
        ogs_pkbuf_t *pkbuf = pkbufs[i];
        ogs_assert(pkbuf);

        iov[i].iov_base = pkbuf->data;
        iov[i].iov_len = pkbuf->len;

        mmsg[i].msg_hdr.msg_name = (struct sockaddr *)&gnode->addr.sa; // 使用 sa 成员作为地址
        mmsg[i].msg_hdr.msg_namelen = ogs_sockaddr_len(&gnode->addr); // 使用ogs_sockaddr_len获取长度
        mmsg[i].msg_hdr.msg_iov = &iov[i];
        mmsg[i].msg_hdr.msg_iovlen = 1;
        //mmsg[i].msg_len = pkbuf->len;
        //mmsg[i].msg_hdr.msg_flags      = 0;
        mmsg[i].msg_hdr.msg_control    = NULL;
        mmsg[i].msg_hdr.msg_controllen = 0;
    }

    sent = sendmmsg(gnode->sock->fd, mmsg, max_msgs, 0);
    if (sent < 0) {
        int err = ogs_socket_errno;
        if (err != OGS_EAGAIN) {
            ogs_log_message(OGS_LOG_ERROR, err, "sendmmsg failed");
        }
        return OGS_ERROR;
    } else if (sent != max_msgs) {
        ogs_log_message(OGS_LOG_WARN, 0, "Not all packets were sent");
        // 可以选择重试未发送的报文
    }else{
        //ogs_log_message(OGS_LOG_INFO, 0, "sendmmsg sucess");
    }

    return OGS_OK;
}

// 将报文添加到缓存区，并检查是否需要发送
void ogs_gtp_msendto(ogs_gtp_node_t *gnode, ogs_pkbuf_t *pkbuf) {
    int i;
    if (pkbuf == NULL || gnode == NULL || gnode->sock == NULL) {
        // 参数无效，记录日志并返回
        ogs_log_message(OGS_LOG_ERROR, 0, "Invalid argument for add_to_cache_and_send_if_needed");
        return;
    }

    // 将报文添加到缓存区
    cache[cache_size++] = pkbuf;

    // 检查缓存区是否已满
    if (cache_size == MAX_CACHE_SIZE) {
        // 缓存区已满，调用ogs_gtp_sendmmsg发送所有报文
        if (ogs_gtp_sendmmsg(gnode, cache, cache_size) != OGS_OK) {
            // 发送失败，记录日志
            ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno, "Failed to send packets using sendmmsg");
        }
        
        for (i = 0; i < cache_size; ++i) {
            ogs_pkbuf_free(cache[i]);
        }
        // 发送后清空缓存区
        cache_size = 0;
    }
}