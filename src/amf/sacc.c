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

        rv = ogs_addaddrinfo(&g_sacc_nodes[n].addr,AF_INET, ip, g_local_node_config.port, 0);
        ogs_assert(rv == OGS_OK);
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

        ogs_info("node %d send sacc handshake to node %d.",g_local_node_config.node, g_sacc_nodes[n].node);
        sacc_send_request(SACC_MSG_TYPE_HANDSHAKE , &g_sacc_nodes[n]); 
    }
}

void sacc_heartbeat(void) {
    int n;
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

        ogs_info("node %d send sacc heartbeat to node %d.",g_local_node_config.node, g_sacc_nodes[n].node);
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

    ogs_info("node %d receive sacc %s request from node:%s deviceId:%s group:%s serviceIp:%s",g_local_node_config.node, sacc_msg_ToString(msg_type), recv_msg_Data->node, recv_msg_Data->deviceId, recv_msg_Data->group, recv_msg_Data->serviceIp);

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

    ogs_info("node %d send sacc %s response to node:%s.",g_local_node_config.node, sacc_msg_ToString(msg_type), recvmsg->sacc_msg_Data->node);

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
        if (amf_self()->icps_port == 9777){//TODO:测试，待删除
            ogs_info("try register UDM to nrf.");
            sacc_sbi_construct_nrfinstance_for_udm();
        }
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

void sacc_sbi_construct_nrfinstance_for_udm(void)
{
    ogs_sbi_nf_instance_t *nf_instance = NULL;
	ogs_sbi_nf_service_t *nf_service = NULL;
	int i = 0;
	bool handled;
	
	nf_instance = ogs_sbi_nf_instance_find((char*)OGS_SBI_UDM_INSTANCE_ID);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)OGS_SBI_UDM_INSTANCE_ID);
        ogs_sbi_nf_fsm_init(nf_instance);//TODO:待修改
        //ogs_info("UDM[%s] (Construct) NF registered", nf_instance->id);

    } else {
        //OGS_FSM_TRAN(&nf_instance->sm, amf_nf_state_registered);//TODO:待修改
        // ogs_fsm_dispatch(&nf_instance->sm, NULL);

        // ogs_warn("UDM[%s] (NRF-Construct) NF has already been added",
        //         (char*)OGS_SBI_UDM_INSTANCE_ID);

    }

	nf_instance->nf_type = OpenAPI_nf_type_UDM;
	nf_instance->nf_status = OpenAPI_nf_status_REGISTERED;

	nf_instance->priority = 0;
	nf_instance->capacity = 100;
	nf_instance->load = 0;

	
	//ogs_sbi_context_get_nf_addr(nf_instance, "udm"); //TODO:测试，要从T2文件中获取地址
    ogs_sbi_nf_instance_build_default(nf_instance); //TODO:先默认设置一个
    nf_instance->time.heartbeat_interval = 0;//不搞心跳,否则NRF会超时释放掉. amf对这些网元搞心跳也麻烦

	// nf_service = ogs_sbi_nf_service_add(nf_instance, (char*)OGS_SBI_UDM_INSTANCE_ID,
    //             (char*)OGS_SBI_SERVICE_NAME_NUDM_UEAU, OpenAPI_uri_scheme_http);
    // ogs_assert(nf_service);

	// ogs_sbi_nf_service_add_version(nf_service, (char*)OGS_SBI_API_V1,
    //             (char*)OGS_SBI_API_V1_0_0, NULL);
    // ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_AUSF);
    
	// for(i = 0; i < nf_instance->num_of_ipv4; i++ )
	// {
	//     nf_service->addr[nf_service->num_of_addr].ipv4 = nf_instance->ipv4[i];
	// 	nf_service->addr[nf_service->num_of_addr].port = 7777;
	// 	nf_service->num_of_addr++;
	// }
	
	// nf_service->priority = 0;
	// nf_service->capacity = 100;
	// nf_service->load = 0;

	// nf_service = ogs_sbi_nf_service_add(nf_instance, (char*)OGS_SBI_UDM_INSTANCE_ID,
    //             (char*)OGS_SBI_SERVICE_NAME_NUDM_UECM, OpenAPI_uri_scheme_http);
    // ogs_assert(nf_service);

	// ogs_sbi_nf_service_add_version(nf_service, (char*)OGS_SBI_API_V1,
    //             (char*)OGS_SBI_API_V1_0_0, NULL);
    // ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_AMF);
    
	// for(i = 0; i < nf_instance->num_of_ipv4; i++ )
	// {
	//     nf_service->addr[nf_service->num_of_addr].ipv4 = nf_instance->ipv4[i];
	// 	nf_service->addr[nf_service->num_of_addr].port = 7777;
	// 	nf_service->num_of_addr++;
	// }
	
	// nf_service->priority = 0;
	// nf_service->capacity = 100;
	// nf_service->load = 0;

	// nf_service = ogs_sbi_nf_service_add(nf_instance, (char*)OGS_SBI_UDM_INSTANCE_ID,
    //             (char*)OGS_SBI_SERVICE_NAME_NUDM_SDM, OpenAPI_uri_scheme_http);
    // ogs_assert(nf_service);

	// ogs_sbi_nf_service_add_version(nf_service, (char*)OGS_SBI_API_V2,
    //             (char*)OGS_SBI_API_V2_0_0, NULL);
    // ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_AMF);
	// ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_SMF);
    
	// for(i = 0; i < nf_instance->num_of_ipv4; i++ )
	// {
	//     nf_service->addr[nf_service->num_of_addr].ipv4 = nf_instance->ipv4[i];
	// 	nf_service->addr[nf_service->num_of_addr].port = 7777;
	// 	nf_service->num_of_addr++;
	// }

	// nf_service->priority = 0;
	// nf_service->capacity = 100;
	// nf_service->load = 0;

    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AUSF);

    ogs_sbi_nf_service_t *service = NULL;
    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_UEAU)) {
        service = ogs_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_UEAU);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AUSF);
    //}

    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_UECM)) {
        service = ogs_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_UECM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_SMF);
    //}

    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_SDM)) {
        service = ogs_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_SDM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V2, OGS_SBI_API_V2_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_SMF);
    //}

	

	ogs_warn("[%s] Try registration with NRF",
                    NF_INSTANCE_ID(ogs_sbi_self()->nf_instance));

    sacc_nnrf_nfm_send_nf_register(nf_instance);

    // handled = ogs_sbi_client_associate(nf_instance);

	// if (!handled)
	// {
    //     ogs_error("[%s] Cannot associate NF EndPoint", nf_instance->id);
    //     AMF_NF_INSTANCE_CLEAR("NRF-notify", nf_instance);
    //     return;
    // }
    // else
    // {
    //     ogs_error("[%s] UDM associate NF EndPoint", nf_instance->id);
    // }
	return;
	
}