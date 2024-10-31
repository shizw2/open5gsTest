/*
 * Copyright (C) 2023 by <Your Name>
 *
 * This file is part of <Your Project>.
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

#include "sacc.h"
#include "sbi-path.h"


sacc_config_t g_config = {
    .enable = 1,
    .port = 7777,//2333,
    .scanInterval = 5,
    .heartbeatInterval = 5,
    .heartbeatLost = 5,
    .role = "T2",
    .group = 1,
    .node = 1,
    .nodeNum = 2
};

sacc_peer_t g_sacc_peers[MAX_PEER_NUM];

int sacc_initialize(const sacc_config_t *config) {
    int rv,n;

    ogs_info("SACC initialized with configuration: enable=%d, port=%d, scanInterval=%d, heartbeatInterval=%d, heartbeatLost=%d, role=%s, group=%d, node=%d, nodeNum=%d",
                 g_config.enable, g_config.port, g_config.scanInterval, g_config.heartbeatInterval, g_config.heartbeatLost,
                 g_config.role, g_config.group, g_config.node, g_config.nodeNum);

    char ip[16];

    memset(g_sacc_peers, 0, sizeof(sacc_peer_t)*MAX_PEER_NUM);

    //TODO:为了测试
    g_config.node = ogs_global_conf()->parameter.capacity - 99;

    if (ogs_global_conf()->parameter.capacity != 100){//先只测试单向
        return OGS_OK;
    }

    for (n = 0; n < g_config.nodeNum && n < MAX_PEER_NUM; n++){
        char ip[16];
        char uri[256] = {};
        int base = 100 + g_config.group;
        int offset = n * 2 + 1; // Calculate the last octet as N*2-1
        //snprintf(ip, sizeof(ip), "192.168.%d.%d", base, offset);
        //snprintf(ip, sizeof(ip), "127.0.%d.%d", base, offset);
        if (ogs_global_conf()->parameter.capacity == 100){
            snprintf(ip, sizeof(ip), "127.0.0.%d", 25);
        }else{
            snprintf(ip, sizeof(ip), "127.0.0.%d", 5);
        }
        if (n == g_config.node){            
            rv = ogs_addaddrinfo(&g_config.addr,AF_INET, ip, g_config.port, 0);
            ogs_assert(rv == OGS_OK);
        }else{
            rv = ogs_addaddrinfo(&g_sacc_peers[n].addr,AF_INET, ip, g_config.port, 0);
            ogs_assert(rv == OGS_OK);
            ogs_info("peer addr:%p",g_sacc_peers[n].addr);
            sacc_associate_peer_client(&g_sacc_peers[n]);

            snprintf(uri, sizeof(uri), "http://%s:7777/acc/v1/nfinfo", ip);
            g_sacc_peers[n].uri = ogs_strdup(uri);
            snprintf(uri, sizeof(uri), "http://%s:7777/acc/v1/heartbeat", ip);
            g_sacc_peers[n].heartbeat_uri = ogs_strdup(uri);
            ogs_info("peer client:%p",g_sacc_peers[n].client);

        }
    }

    return OGS_OK;
}


void sacc_associate_peer_client(sacc_peer_t *peer)
{
    ogs_sbi_client_t *client = NULL;
    ogs_sockaddr_t *addr = NULL, *addr6 = NULL;
    OpenAPI_uri_scheme_e scheme = OpenAPI_uri_scheme_http;

    ogs_assert(peer);

    addr = peer->addr;

    ogs_info("sacc_associate_peer_client addr:%p",addr);

    if (!client) {
        client = ogs_sbi_client_find(
                scheme, NULL, 0, addr, addr6);
        if (!client) {
            ogs_debug("%s: ogs_sbi_client_add()", OGS_FUNC);
            client = ogs_sbi_client_add(
                    scheme, NULL, 0, addr, addr6);
            if (!client) {
                ogs_error("%s: ogs_sbi_client_add() failed", OGS_FUNC);
                return;
            }
        }    
    }

    if (client){
        OGS_SBI_SETUP_CLIENT(peer, client);
        ogs_info("add client to peer");
    }
}


void sacc_scan(void) {
    int n;

    if (is_amf_sps()){
        return;
    }

    if (ogs_global_conf()->parameter.capacity != 100){//TODO:先只测试单向
        return ;
    }

    if (!g_config.enable) {
        ogs_info("SACC scan is disabled.");
        return;
    }
    ogs_info("SACC scanning for new nodes...");
        
    for (n = 0; n < g_config.nodeNum && n < MAX_PEER_NUM; n++){
        if (g_sacc_peers[n].state != 0){
            continue;
        }

        if (n == g_config.node){
            continue;
        }

        ogs_info("SACC scanning for new node %d, state:%d...",n,g_sacc_peers[n].state);
        
        sacc_send_handshake_request(&g_sacc_peers[n]); 
    }
}

void sacc_heartbeat(void) {
    int n;
    if (!g_config.enable) {
        ogs_info("SACC heartbeat is disabled.");
        return;
    }

    ogs_info("SACC sending heartbeats...");
    for (n = 0; n < g_config.nodeNum && n < MAX_PEER_NUM; n++){
        if (g_sacc_peers[n].state != 1){
            continue;
        }

        if (n == g_config.node){
            continue;
        }

        ogs_info("SACC scanning for new node %d...",n);
        
        sacc_send_heartbeat_request(&g_sacc_peers[n]); 
    }
}


ogs_sbi_request_t *sacc_build_handshake_request(
        sacc_peer_t *peer, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    sacc_handshake_t handshake;

    ogs_assert(peer);
    ogs_assert(peer->uri);

    memset(&handshake, 0, sizeof(sacc_handshake_t));

    snprintf(handshake.deviceId, sizeof(handshake.deviceId), "%d", g_config.node);
    snprintf(handshake.group, sizeof(handshake.group), "G%d", g_config.group);
    snprintf(handshake.node, sizeof(handshake.node), "N%d", g_config.node);

    OGS_ADDR(g_config.addr, handshake.serviceIp);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    message.h.uri = peer->uri;  //测试不赋值不行，会报No Service Name (../lib/sbi/message.c:321)

    message.sacc_handshakeReqData = &handshake;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

    return request;
}


void sacc_send_handshake_request(sacc_peer_t *peer) {
    bool rc;
    ogs_sbi_request_t *request = NULL;
    ogs_sbi_client_t *client = NULL;

    ogs_assert(peer);
    client = peer->client;
    ogs_assert(client);

    request = sacc_build_handshake_request(peer, NULL);
    ogs_assert(request);
    rc = ogs_sbi_send_request_to_client(
            client, ogs_sbi_client_handler, request, peer);
    ogs_expect(rc == true);

    ogs_sbi_request_free(request);
}

void sacc_send_heartbeat_request(sacc_peer_t *peer) {
    bool rc;
    ogs_sbi_request_t *request = NULL;
    ogs_sbi_client_t *client = NULL;

    ogs_assert(peer);
    client = peer->client;
    ogs_assert(client);

    request = sacc_build_handshake_request(peer, NULL);
    ogs_assert(request);
    rc = ogs_sbi_send_request_to_client(
            client, ogs_sbi_client_handler, request, peer);
    ogs_expect(rc == true);

    ogs_sbi_request_free(request);
}

bool sacc_handle_handshake_req(ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;
    sacc_handshake_t *handshakeReq;

    ogs_assert(stream);

    memset(&sendmsg, 0, sizeof(sendmsg));

    handshakeReq = recvmsg->sacc_handshakeReqData;
    sendmsg.sacc_handshakeReqData = recvmsg->sacc_handshakeReqData;

    ogs_info("receive sacc handshake request, %s %s %s %s",handshakeReq->deviceId,handshakeReq->group,handshakeReq->node,handshakeReq->serviceIp);


    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_OK);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

    return true;
}

bool sacc_handle_handshake_resp(ogs_sbi_message_t *recvmsg)
{

    sacc_handshake_t *handshakeReq;

   
    handshakeReq = recvmsg->sacc_handshakeReqData;
  

    ogs_info("receive sacc handshake response, %s %s %s %s",handshakeReq->deviceId,handshakeReq->group,handshakeReq->node,handshakeReq->serviceIp);


    return true;
}