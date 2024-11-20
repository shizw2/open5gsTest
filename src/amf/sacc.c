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
#include "sacc_context.h"
#include "sbi-path.h"


sacc_config_t g_local_node_config = {
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

sacc_node_t g_sacc_nodes[MAX_PEER_NUM+1];


char* sacc_msg_ToString(int msg_type)
{
    const char *msg_typeArray[] =  { "handshake", "heartbeat"};
    size_t sizeofArray = sizeof(msg_typeArray) / sizeof(msg_typeArray[0]);
    if (msg_type < sizeofArray)
        return (char *)msg_typeArray[msg_type];
    else
        return (char *)"Unknown";
}

char * sacc_node_state_ToString(int state){
    const char *stateArray[] =  { "init", "online", "offline"};
    size_t sizeofArray = sizeof(stateArray) / sizeof(stateArray[0]);
    if (state < sizeofArray)
        return (char *)stateArray[state];
    else
        return (char *)"Unknown";
}
int sacc_initialize(const sacc_config_t *config) {
    int rv,n;
    char buf[OGS_ADDRSTRLEN];

    ogs_info("SACC initialized with configuration: enable=%d, port=%d, scanInterval=%d, heartbeatInterval=%d, heartbeatLost=%d, role=%s, group=%d, node=%d, nodeNum=%d",
                 g_local_node_config.enable, g_local_node_config.port, g_local_node_config.scanInterval, g_local_node_config.heartbeatInterval, g_local_node_config.heartbeatLost,
                 g_local_node_config.role, g_local_node_config.group, g_local_node_config.node, g_local_node_config.nodeNum);

    char ip[16];

    memset(g_sacc_nodes, 0, sizeof(sacc_node_t)*MAX_PEER_NUM);

    //TODO:为了测试,node跟amf的端口号一致
    g_local_node_config.node = amf_self()->icps_port + 1 - 9777;

    for (n = 1; n <= g_local_node_config.nodeNum && n < MAX_PEER_NUM; n++){
        char ip[16];
        char uri[256] = {};
        int base = 100 + g_local_node_config.group;
        int offset = n * 2 + 1; // Calculate the last octet as N*2-1
        //snprintf(ip, sizeof(ip), "192.168.%d.%d", base, offset);
        //构造测试地址
        snprintf(ip, sizeof(ip), "127.0.0.%d", 5+ (n-1)*20);//5,25,45,65...   
       
        rv = ogs_addaddrinfo(&g_sacc_nodes[n].addr,AF_INET, ip, g_local_node_config.port, 0);
        ogs_assert(rv == OGS_OK);

        g_sacc_nodes[n].group = g_local_node_config.group;
        g_sacc_nodes[n].node = n;
        g_sacc_nodes[n].state = SACC_PEER_STATE_INIT;      

        ogs_info("sacc node %d addr:%s",n, OGS_ADDR(g_sacc_nodes[n].addr,buf));
        sacc_associate_peer_client(&g_sacc_nodes[n]);

        snprintf(uri, sizeof(uri), "http://%s:%d/acc/v1/nfinfo", ip,g_local_node_config.port);
        g_sacc_nodes[n].uri = ogs_strdup(uri);
        snprintf(uri, sizeof(uri), "http://%s:%d/acc/v1/heartbeat", ip,g_local_node_config.port);
        g_sacc_nodes[n].heartbeat_uri = ogs_strdup(uri);
        ogs_info("node handshake uri:%s,heartbeat uri:%s", g_sacc_nodes[n].uri,g_sacc_nodes[n].heartbeat_uri);
        
    }

    return OGS_OK;
}

int sacc_initialize2(const sacc_config_t *config) {
    int rv,n;
    char buf[OGS_ADDRSTRLEN];

    ogs_info("SACC initialized with configuration: enable=%d, port=%d, scanInterval=%d, heartbeatInterval=%d, heartbeatLost=%d, role=%s, group=%d, node=%d, nodeNum=%d",
                 g_local_node_config.enable, g_local_node_config.port, g_local_node_config.scanInterval, g_local_node_config.heartbeatInterval, g_local_node_config.heartbeatLost,
                 g_local_node_config.role, g_local_node_config.group, g_local_node_config.node, g_local_node_config.nodeNum);

    char ip[16];

    memset(g_sacc_nodes, 0, sizeof(sacc_node_t)*MAX_PEER_NUM);

    //TODO:为了测试,node跟amf的端口号一致
    g_local_node_config.node = amf_self()->icps_port + 1 - 9777;

    for (n = 1; n <= g_local_node_config.nodeNum && n < MAX_PEER_NUM; n++){
        char ip[16];
        char uri[256] = {};
        int base = 100 + g_local_node_config.group;
        int offset = n * 2 + 1; // Calculate the last octet as N*2-1
        //snprintf(ip, sizeof(ip), "192.168.%d.%d", base, offset);
        //构造测试地址
        snprintf(ip, sizeof(ip), "127.0.0.%d", 5+ (n-1)*20);//5,25,45,65...   
       
        rv = ogs_addaddrinfo(&g_sacc_nodes[n].addr,AF_INET, ip, g_local_node_config.port, 0);
        ogs_assert(rv == OGS_OK);

        g_sacc_nodes[n].group = g_local_node_config.group;
        g_sacc_nodes[n].node = n;
        g_sacc_nodes[n].state = SACC_PEER_STATE_INIT;      

        ogs_info("sacc node %d addr:%s",n, OGS_ADDR(g_sacc_nodes[n].addr,buf));
        sacc_associate_peer_client(&g_sacc_nodes[n]);

        snprintf(uri, sizeof(uri), "http://%s:%d/acc/v1/nfinfo", ip,g_local_node_config.port);
        g_sacc_nodes[n].uri = ogs_strdup(uri);
        snprintf(uri, sizeof(uri), "http://%s:%d/acc/v1/heartbeat", ip,g_local_node_config.port);
        g_sacc_nodes[n].heartbeat_uri = ogs_strdup(uri);
        ogs_info("node handshake uri:%s,heartbeat uri:%s", g_sacc_nodes[n].uri,g_sacc_nodes[n].heartbeat_uri);
        
    }

    return OGS_OK;
}


void sacc_associate_peer_client(sacc_node_t *peer)
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

    if (!g_local_node_config.enable) {
        ogs_info("SACC scan is disabled.");
        return;
    }

    //ogs_info("SACC scanning for nodes...");
        
    for (n = 1; n <= g_local_node_config.nodeNum && n < MAX_PEER_NUM; n++){
        if (g_sacc_nodes[n].state == SACC_PEER_STATE_ONLINE){//激活的不再探测
            continue;
        }

        if (n == g_local_node_config.node){//跳过本节点
            continue;
        }

        ogs_debug("node %d send sacc handshake to node %d.",g_local_node_config.node, g_sacc_nodes[n].node);
        sacc_send_request(SACC_MSG_TYPE_HANDSHAKE , &g_sacc_nodes[n]); 
    }
}

void sacc_heartbeat(void) {
    int n;
    ogs_sbi_nf_instance_t *nf_instance;
    
    if (!g_local_node_config.enable) {
        ogs_info("sacc heartbeat is disabled.");
        return;
    }

    //ogs_info("sacc sending heartbeats...");
    
    for (n = 1; n <= g_local_node_config.nodeNum && n < MAX_PEER_NUM; n++){
        if (g_sacc_nodes[n].state != SACC_PEER_STATE_ONLINE){//对激活的进行心跳
            continue;
        }

        if (n == g_local_node_config.node){//跳过本节点
            continue;
        }

        ogs_debug("node %d send sacc heartbeat to node %d.",g_local_node_config.node, g_sacc_nodes[n].node);
        g_sacc_nodes[n].heartbeatLost++;
        if (g_sacc_nodes[n].heartbeatLost >MAC_HEARTBEAT_LOST_CNT){
            if ( g_sacc_nodes[n].state != SACC_PEER_STATE_OFFLINE){
                ogs_info("node %d is offline",n);
                g_sacc_nodes[n].state = SACC_PEER_STATE_OFFLINE;
                if (g_sacc_nodes[n].smf_nf_instance){
                    ogs_info("send de-register %s[%s] to nrf.", OpenAPI_nf_type_ToString(g_sacc_nodes[n].smf_nf_instance->nf_type),g_sacc_nodes[n].smf_nf_instance->id);
                    sacc_nnrf_nfm_send_nf_de_register(g_sacc_nodes[n].smf_nf_instance);
                }

                if (g_sacc_nodes[n].ausf_nf_instance){
                    ogs_info("send de-register %s[%s] to nrf.", OpenAPI_nf_type_ToString(g_sacc_nodes[n].ausf_nf_instance->nf_type),g_sacc_nodes[n].ausf_nf_instance->id);
                    sacc_nnrf_nfm_send_nf_de_register(g_sacc_nodes[n].ausf_nf_instance);
                }

                if (g_sacc_nodes[n].udm_nf_instance){
                    ogs_info("send de-register %s[%s] to nrf.", OpenAPI_nf_type_ToString(g_sacc_nodes[n].udm_nf_instance->nf_type),g_sacc_nodes[n].udm_nf_instance->id);
                    sacc_nnrf_nfm_send_nf_de_register(g_sacc_nodes[n].udm_nf_instance);
                }
                /*ogs_list_for_each(&ogs_sbi_self()->nf_instance_list, nf_instance){
                    if (nf_instance->nf_type == OpenAPI_nf_type_UDM
                    ||nf_instance->nf_type == OpenAPI_nf_type_AUSF
                    ||nf_instance->nf_type == OpenAPI_nf_type_SMF){
                        //ogs_sbi_nf_fsm_fini(nf_instance);
                        ogs_info("send de-register %s[%s] to nrf.", OpenAPI_nf_type_ToString(nf_instance->nf_type),nf_instance->id);
                        sacc_nnrf_nfm_send_nf_de_register(nf_instance);
                    }
                }*/
            }
        }
        sacc_send_request(SACC_MSG_TYPE_HEARDBEAT , &g_sacc_nodes[n]); 
    }
}


ogs_sbi_request_t *sacc_build_request(int msg_type,
        sacc_node_t *peer, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    sacc_msg_data_t msg_data;

    ogs_assert(peer);
    ogs_assert(peer->uri);

    memset(&msg_data, 0, sizeof(sacc_msg_data_t));

    snprintf(msg_data.deviceId, sizeof(msg_data.deviceId), "%d", g_local_node_config.node);
    snprintf(msg_data.group, sizeof(msg_data.group), "%d", g_local_node_config.group);
    snprintf(msg_data.node, sizeof(msg_data.node), "%d", g_local_node_config.node);
    OGS_ADDR(g_sacc_nodes[g_local_node_config.node].addr, msg_data.serviceIp);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    if (msg_type == SACC_MSG_TYPE_HANDSHAKE){
        message.h.uri = peer->uri;  //测试不赋值不行，会报No Service Name (../lib/sbi/message.c:321)
    }else if (msg_type == SACC_MSG_TYPE_HEARDBEAT){
        message.h.uri = peer->heartbeat_uri;
    }

    message.sacc_msg_Data = &msg_data;

    //ogs_info("node %d build sacc %s request from node:%s deviceId:%s group:%s serviceIp:%s to node:%d.",g_local_node_config.node, sacc_msg_ToString(msg_type), msg_data.node,msg_data.deviceId,msg_data.group,msg_data.serviceIp, peer->node);

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

    return request;
}


void sacc_send_request(int msg_type, sacc_node_t *peer) {
    bool rc;
    ogs_sbi_request_t *request = NULL;
    ogs_sbi_client_t *client = NULL;

    ogs_assert(peer);
    client = peer->client;
    ogs_assert(client);

    request = sacc_build_request(msg_type, peer, NULL);
    ogs_assert(request);
    rc = ogs_sbi_send_request_to_client(
            client, ogs_sbi_client_handler, request, peer);
    ogs_expect(rc == true);

    ogs_sbi_request_free(request);
}

bool sacc_handle_request(int msg_type, ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;
    sacc_msg_data_t msg_data;
    sacc_msg_data_t *recv_msg_Data;
    int node;
    int group;

    ogs_assert(stream);

    memset(&sendmsg, 0, sizeof(sendmsg));
    memset(&msg_data, 0, sizeof(msg_data));

    recv_msg_Data = recvmsg->sacc_msg_Data;
    node = atoi(recv_msg_Data->node);
    group = atoi(recv_msg_Data->group);

    ogs_debug("node %d receive sacc %s request from node:%s deviceId:%s group:%s serviceIp:%s",g_local_node_config.node, sacc_msg_ToString(msg_type), recv_msg_Data->node, recv_msg_Data->deviceId, recv_msg_Data->group, recv_msg_Data->serviceIp);

    if (node > g_local_node_config.nodeNum || node < 1){
        ogs_info("incomming node %d is out of range, ignore it.",node);
        return false;
    }

    if (group != g_local_node_config.group){
        ogs_info("incomming node %d has different group %d with this node %d's group %d, ignore it.",node,group,g_local_node_config.node,g_local_node_config.group);
        return false;
    }

    if (strcmp(g_local_node_config.role, SACC_NODE_ROLE_T2) != 0){
        ogs_info("this node %d is not work in %s mode, ignore it.",g_local_node_config.node, SACC_NODE_ROLE_T2);
        return false;
    }

    //g_sacc_nodes[node].state = SACC_PEER_STATE_ONLINE; //收到对端请求,并不需要修改状态
    g_sacc_nodes[node].heartbeatLost = 0;//重置心跳丢失计数

    snprintf(msg_data.deviceId, sizeof(msg_data.deviceId), "%s", g_sacc_nodes[g_local_node_config.node].deviceId);
    snprintf(msg_data.group, sizeof(msg_data.group), "%d", g_local_node_config.group);
    snprintf(msg_data.node, sizeof(msg_data.node), "%d", g_local_node_config.node);
    OGS_ADDR(g_sacc_nodes[g_local_node_config.node].addr, msg_data.serviceIp);
    snprintf(msg_data.result, sizeof(msg_data.result), "OK");

    sendmsg.sacc_msg_Data = &msg_data;
    sendmsg.http.location = recvmsg->h.uri;

    ogs_debug("node %d send sacc %s response to node:%s.",g_local_node_config.node, sacc_msg_ToString(msg_type), recvmsg->sacc_msg_Data->node);

    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_OK);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

    return true;
}

bool sacc_handle_response(int msg_type, ogs_sbi_message_t *recvmsg)
{
    sacc_msg_data_t *recv_msg_Data;
    int node;
    int group;

    recv_msg_Data = recvmsg->sacc_msg_Data;

    node = atoi(recv_msg_Data->node);
    group = atoi(recv_msg_Data->group);

    ogs_info("node %d receive sacc %s response, peer node info: deviceId:%s group:%s node:%s serviceIp:%s result:%s",g_local_node_config.node, sacc_msg_ToString(msg_type),recv_msg_Data->deviceId,recv_msg_Data->group,recv_msg_Data->node,recv_msg_Data->serviceIp, recv_msg_Data->result);

    if (node > g_local_node_config.nodeNum || node < 1){
        ogs_info("incomming node %d is out of range, ignore it.",node);
        return false;
    }

    if (group != g_local_node_config.group){
        ogs_info("incomming node %d has different group %d with this node %d's group %d, ignore it.",node,group,g_local_node_config.node,g_local_node_config.group);
        return false;
    }

    if (strcmp(recv_msg_Data->result, "OK") != 0){
        ogs_info("incomming node %d sacc %s response result is not OK, ignore it.",node, sacc_msg_ToString(msg_type));
        g_sacc_nodes[node].state = SACC_PEER_STATE_OFFLINE;
        return false;
    }

    if (msg_type == SACC_MSG_TYPE_HANDSHAKE){
        //if (amf_self()->icps_port == 9777){//TODO:测试，待删除
            //ogs_info("try register UDM to nrf.");
            sacc_sbi_register_nrfinstance_for_udm(&g_sacc_nodes[node]);
            sacc_sbi_register_nrfinstance_for_ausf(&g_sacc_nodes[node]);
            sacc_sbi_register_nrfinstance_for_smf(&g_sacc_nodes[node]);
            sacc_sbi_register_nrfinstance_for_amf(&g_sacc_nodes[node]);
        //}
    }

    g_sacc_nodes[node].state = SACC_PEER_STATE_ONLINE;
    g_sacc_nodes[node].heartbeatLost = 0;//重置心跳丢失计数
    return true;
}

//ogs_nnrf_nfm_send_nf_register
bool sacc_nnrf_nfm_send_nf_register(ogs_sbi_nf_instance_t *nf_instance)
{
    bool rc;
    ogs_sbi_request_t *request = NULL;

    ogs_assert(nf_instance);

    ogs_info("sacc_nnrf_nfm_send_nf_register, nf-type:%s.", OpenAPI_nf_type_ToString(nf_instance->nf_type));

    request = sacc_nnrf_nfm_build_register(nf_instance);
    if (!request) {
        ogs_error("No Request");
        return false;
    }

    rc = ogs_sbi_send_notification_request(
            OGS_SBI_SERVICE_TYPE_NNRF_NFM, NULL, request, nf_instance);
    ogs_expect(rc == true);

    ogs_sbi_request_free(request);

    return rc;
}

bool sacc_nnrf_nfm_send_nf_de_register(ogs_sbi_nf_instance_t *nf_instance)
{
    bool rc;
    ogs_sbi_request_t *request = NULL;

    ogs_assert(nf_instance);

    request = sacc_nnrf_nfm_build_de_register(nf_instance);
    if (!request) {
        ogs_error("No Request");
        return false;
    }

    rc = ogs_sbi_send_notification_request(
            OGS_SBI_SERVICE_TYPE_NNRF_NFM, NULL, request, nf_instance);
    ogs_expect(rc == true);

    ogs_sbi_request_free(request);

    return rc;
}


ogs_sbi_request_t *sacc_nnrf_nfm_build_de_register(ogs_sbi_nf_instance_t *nf_instance)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    ogs_assert(nf_instance);
    ogs_assert(nf_instance->id);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_DELETE;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NNRF_NFM;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] =
        (char *)OGS_SBI_RESOURCE_NAME_NF_INSTANCES;
    message.h.resource.component[1] = nf_instance->id;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

    return request;
}

ogs_sbi_request_t *sacc_nnrf_nfm_build_register(
        ogs_sbi_nf_instance_t *nf_instance)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    OpenAPI_nf_profile_t *NFProfile = NULL;

    //nf_instance = ogs_sbi_self()->nf_instance;
    ogs_assert(nf_instance);
    ogs_assert(nf_instance->id);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_PUT;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NNRF_NFM;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] =
        (char *)OGS_SBI_RESOURCE_NAME_NF_INSTANCES;
    message.h.resource.component[1] = nf_instance->id;

    message.http.content_encoding = (char*)ogs_sbi_self()->content_encoding;

    NFProfile = ogs_nnrf_nfm_build_nf_profile(
                    nf_instance, NULL, NULL, true);
    if (!NFProfile) {
        ogs_error("No NFProfile");
        goto end;
    }

    message.NFProfile = NFProfile;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

end:

    if (NFProfile)
        ogs_nnrf_nfm_free_nf_profile(NFProfile);

    return request;
}

void sacc_sbi_register_nrfinstance_for_udm(sacc_node_t *peer)
{
    ogs_sbi_nf_instance_t *nf_instance = NULL;
	ogs_sbi_nf_service_t *nf_service = NULL;
	int i = 0;
	bool handled;
	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];    
    ogs_uuid_t uuid;

    //ogs_uuid_get(&uuid);
    //ogs_uuid_format(id, &uuid);

    nf_instance = ogs_sbi_nf_instance_add();

    snprintf(nf_instance_id, OGS_UUID_FORMATTED_LENGTH + 1, "%s%s-000000%02d%02d%02d",
            OGS_SBI_PREFIX_INSTANCE_ID,OpenAPI_nf_type_ToString(OpenAPI_nf_type_UDM), OpenAPI_nf_type_UDM,peer->group,peer->node);

    nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    }

    peer->udm_nf_instance = nf_instance; 

	ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_UDM);
    ogs_sbi_nf_fsm_init(nf_instance);  

    snprintf(nf_name, sizeof(nf_name), "udm_%d_%d",peer->group,peer->node);
    //sacc_sbi_context_get_nf_info(nf_name, "nrf", "scp",nf_instance);

    sacc_sbi_context_get_nf_info2("udms", nf_name, "nrf", "scp", peer);
    
    nf_instance->time.heartbeat_interval = 0;//不搞心跳,否则NRF会超时释放掉. amf对这些网元搞心跳也麻烦

    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AUSF);

    ogs_sbi_nf_service_t *service = NULL;
    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_UEAU)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_UEAU);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AUSF);
    //}

    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_UECM)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_UECM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_SMF);
    //}

    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_SDM)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_SDM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V2, OGS_SBI_API_V2_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_SMF);
    //}

	ogs_warn("sacc [%s] Try registration with NRF",
                    NF_INSTANCE_ID(nf_instance));

    sacc_nnrf_nfm_send_nf_register(nf_instance);

	return;	
}


void sacc_sbi_register_nrfinstance_for_ausf(sacc_node_t *peer){
    ogs_sbi_nf_instance_t *nf_instance = NULL;
	ogs_sbi_nf_service_t *nf_service = NULL;
	
	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];
    int i;

    snprintf(nf_instance_id, OGS_UUID_FORMATTED_LENGTH + 1, "%s%s-000000%02d%02d%02d",OGS_SBI_PREFIX_INSTANCE_ID, OpenAPI_nf_type_ToString(OpenAPI_nf_type_AUSF),OpenAPI_nf_type_AUSF,peer->group,peer->node);
	nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    }
    peer->ausf_nf_instance = nf_instance;
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_AUSF);
    ogs_sbi_nf_fsm_init(nf_instance);  

    /* Build NF instance information. It will be transmitted to NRF. */
    //ogs_sbi_nf_instance_build_default(nf_instance);
    ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    snprintf(nf_name, sizeof(nf_name), "ausf_%d_%d",peer->group,peer->node);
    sacc_sbi_context_get_nf_info(nf_name, "nrf", "scp",nf_instance);
    ogs_info("nf_name:%s,nf_instance->num_of_ipv4:%d",nf_name,nf_instance->num_of_ipv4);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);

    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NAUSF_AUTH)) {
#if 1
        nf_service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NAUSF_AUTH);
        ogs_assert(nf_service);
        ogs_sbi_nf_service_add_version(
                    nf_service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_AMF);
#endif 
    //}

    /* Setup Subscription-Data */ //TODO:得修改
    ogs_sbi_subscription_spec_add(OpenAPI_nf_type_SEPP, NULL);
    ogs_sbi_subscription_spec_add(
            OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NUDM_UEAU);

	ogs_warn("[%s] Try registration with NRF",
                    NF_INSTANCE_ID(nf_instance));

    sacc_nnrf_nfm_send_nf_register(nf_instance);
    return ;
}

void sacc_sbi_register_nrfinstance_for_smf(sacc_node_t *peer){
    ogs_sbi_nf_instance_t *nf_instance = NULL;
    ogs_sbi_nf_service_t *service = NULL;

	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];
    snprintf(nf_instance_id, OGS_UUID_FORMATTED_LENGTH + 1, "%s%s-000000%02d%02d%02d",
        OGS_SBI_PREFIX_INSTANCE_ID,OpenAPI_nf_type_ToString(OpenAPI_nf_type_SMF),OpenAPI_nf_type_SMF,peer->group,peer->node);
	nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    } 
    peer->smf_nf_instance = nf_instance;
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_SMF);
    ogs_sbi_nf_fsm_init(nf_instance);

    /* Build NF instance information. It will be transmitted to NRF. */
    //ogs_sbi_nf_instance_build_default(nf_instance);
    ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    snprintf(nf_name, sizeof(nf_name), "smf_%d_%d",peer->group,peer->node);
    sacc_sbi_context_get_nf_info(nf_name, "nrf", "scp",nf_instance);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);

    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NSMF_PDUSESSION)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NSMF_PDUSESSION);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
    //}

    /* Setup Subscription-Data */ //TODO:可能需要
    // ogs_sbi_subscription_spec_add(OpenAPI_nf_type_SEPP, NULL);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NAMF_COMM);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NPCF_SMPOLICYCONTROL);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NUDM_SDM);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NUDM_UECM);

	ogs_warn("[%s] Try registration with NRF",
                    NF_INSTANCE_ID(nf_instance));

    sacc_nnrf_nfm_send_nf_register(nf_instance);
    return ;
}

void sacc_sbi_register_nrfinstance_for_amf(sacc_node_t *peer){
    ogs_sbi_nf_instance_t *nf_instance = NULL;
	ogs_sbi_nf_service_t *nf_service = NULL;
	
	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];
    int i;

    snprintf(nf_instance_id, OGS_UUID_FORMATTED_LENGTH + 1, "%s%s-000000%02d%02d%02d",
            OGS_SBI_PREFIX_INSTANCE_ID,OpenAPI_nf_type_ToString(OpenAPI_nf_type_AMF), OpenAPI_nf_type_AMF,peer->group,peer->node);

    nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    }
    peer->amf_nf_instance = nf_instance;
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_AMF);
    ogs_sbi_nf_fsm_init(nf_instance);  

    /* Build NF instance information. It will be transmitted to NRF. */
    //ogs_sbi_nf_instance_build_default(nf_instance);
    ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    snprintf(nf_name, sizeof(nf_name), "amf_%d_%d",peer->group,peer->node);
    sacc_sbi_context_get_nf_info(nf_name, "nrf", "scp",nf_instance);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);

    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NAMF_COMM)) {
        nf_service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NAMF_COMM);
        ogs_assert(nf_service);
        ogs_sbi_nf_service_add_version(
                    nf_service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_SMF);
        ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_AMF);
    //}
	ogs_warn("[%s] Try registration with NRF",
                    NF_INSTANCE_ID(nf_instance));

    sacc_nnrf_nfm_send_nf_register(nf_instance);
    return ;
}

ogs_sbi_nf_service_t *sacc_sbi_nf_service_build_default(
        ogs_sbi_nf_instance_t *nf_instance, const char *name)
{
    ogs_sbi_nf_service_t *nf_service = NULL;    
    int i;
    ogs_uuid_t uuid;
    char id[OGS_UUID_FORMATTED_LENGTH + 1];

    OpenAPI_uri_scheme_e scheme = OpenAPI_uri_scheme_NULL;

    ogs_assert(nf_instance);
    ogs_assert(name);

    ogs_uuid_get(&uuid);
    ogs_uuid_format(id, &uuid);

    nf_service = ogs_sbi_nf_service_add(nf_instance, id, name, OpenAPI_uri_scheme_http);
    ogs_assert(nf_service);

    char buf[OGS_ADDRSTRLEN];
	for(i = 0; i < nf_instance->num_of_ipv4; i++ )
	{
        ogs_sockaddr_t *addr = NULL;
        ogs_copyaddrinfo(&addr, nf_instance->ipv4[i]);
	    nf_service->addr[nf_service->num_of_addr].ipv4 = addr;
        nf_service->addr[nf_service->num_of_addr].is_port = true;
		nf_service->addr[nf_service->num_of_addr].port = OGS_PORT(addr);
		nf_service->num_of_addr++;
	}   

    ogs_info("NF Service [%s]", nf_service->name);

    return nf_service;
}

void showsaccnodes(void) {
    printf("\nsacc nodes information:\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    printf("| node   | group  |  state  |   deviceID     | role   |    addr        |\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    char buf[OGS_ADDRSTRLEN];
    for (int i = 1; i <= g_local_node_config.nodeNum && i <=MAX_PEER_NUM; i++) {
        sacc_node_t *node = &g_sacc_nodes[i];
        printf("| %6d | %6d | %7s | %-14s | %-6s | %-14s |\n",
               node->node, node->group, sacc_node_state_ToString(node->state),
               node->deviceId, node->role, OGS_ADDR(node->addr, buf));
    }

    printf("+--------+--------+---------+----------------+--------+----------------+\n");
}

void getnetworkStatus(void) {
    printf("\nsacc nodes information:\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    printf("| node   | group  |  state  |   deviceID     | role   |    addr        |\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    char buf[OGS_ADDRSTRLEN];
    for (int i = 1; i <= g_local_node_config.nodeNum && i <=MAX_PEER_NUM; i++) {
        sacc_node_t *node = &g_sacc_nodes[i];
        printf("| %6d | %6d | %7s | %-14s | %-6s | %-14s |\n",
               node->node, node->group, sacc_node_state_ToString(node->state),
               node->deviceId, node->role, OGS_ADDR(node->addr, buf));
    }

    printf("+--------+--------+---------+----------------+--------+----------------+\n");
}