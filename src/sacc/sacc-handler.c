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

#include "sbi-path.h"
#include "sacc-handler.h"


//#include "sacc.h"
#include "context.h"
#include "sbi-path.h"

extern sacc_node_t g_sacc_nodes[MAX_PEER_NUM+1];

void sacc_scan(void) {
    int n;

    if (!sacc_self()->enable) {
        ogs_info("SACC scan is disabled.");
        return;
    }

    //ogs_info("SACC scanning for nodes...");
        
    for (n = 1; n <= sacc_self()->nodeNum && n < MAX_PEER_NUM; n++){
        // if (g_sacc_nodes[n].state == SACC_PEER_STATE_ONLINE){//激活的不再探测
        //     continue;
        // }

        if (n == sacc_self()->node){//跳过本节点
            continue;
        }

        g_sacc_nodes[n].heartbeatLost++;
        if (g_sacc_nodes[n].heartbeatLost >MAC_HEARTBEAT_LOST_CNT*(1+2+4)){
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

                if (g_sacc_nodes[n].amf_nf_instance){
                    ogs_info("send de-register %s[%s] to nrf.", OpenAPI_nf_type_ToString(g_sacc_nodes[n].amf_nf_instance->nf_type),g_sacc_nodes[n].amf_nf_instance->id);
                    sacc_nnrf_nfm_send_nf_de_register(g_sacc_nodes[n].amf_nf_instance);
                }                
            }
        }
       
        //ogs_info("node %d send sacc handshake to node %d.",sacc_self()->node, g_sacc_nodes[n].node);
        //sacc_send_request(SACC_MSG_TYPE_HANDSHAKE , &g_sacc_nodes[n]); 
        // 检查是否需要发送握手
        // 未断链或断链次数小于5次，正常发送握手
        // 断链5次的节点，在计数器%2=0的时候再发送握手，即变为10秒一次
        // 断链10次的节点，在计数器%4=0的时候发送握手，即变为20秒一次
        if (g_sacc_nodes[n].heartbeatLost <= MAC_HEARTBEAT_LOST_CNT ||
            (g_sacc_nodes[n].heartbeatLost > MAC_HEARTBEAT_LOST_CNT && 
             (g_sacc_nodes[n].heartbeatLost % 2 == 0 && g_sacc_nodes[n].heartbeatLost <= (MAC_HEARTBEAT_LOST_CNT * 2))) ||
            (g_sacc_nodes[n].heartbeatLost > (MAC_HEARTBEAT_LOST_CNT * 2) && 
             g_sacc_nodes[n].heartbeatLost % 4 == 0)) {
            ogs_info("Node %d send sacc handshake to node %d(heartbeatLost:%d).", sacc_self()->node, g_sacc_nodes[n].node,g_sacc_nodes[n].heartbeatLost);
            sacc_send_request(SACC_MSG_TYPE_HANDSHAKE, &g_sacc_nodes[n]);
        }
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

    snprintf(msg_data.deviceId, sizeof(msg_data.deviceId), "%s", sacc_self()->deviceSeq);
    snprintf(msg_data.group, sizeof(msg_data.group), "%d", sacc_self()->group);
    snprintf(msg_data.node, sizeof(msg_data.node), "%d", sacc_self()->node);
    OGS_ADDR(g_sacc_nodes[sacc_self()->node].addr, msg_data.serviceIp);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    //if (msg_type == SACC_MSG_TYPE_HANDSHAKE){
    message.h.uri = peer->uri;
    //}else if (msg_type == SACC_MSG_TYPE_HEARDBEAT){
    //    message.h.uri = peer->heartbeat_uri;
    //}

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

    ogs_info("node %d receive sacc %s request from node:%s deviceId:%s group:%s serviceIp:%s",sacc_self()->node, sacc_msg_ToString(msg_type), recv_msg_Data->node, recv_msg_Data->deviceId, recv_msg_Data->group, recv_msg_Data->serviceIp);

    if (node > sacc_self()->nodeNum || node < 1){
        ogs_info("incomming node %d is out of range %d, ignore it.",node, sacc_self()->nodeNum);
        return false;
    }

    if (group != sacc_self()->group){
        ogs_info("incomming node %d has different group %d with this node %d's group %d, ignore it.",node,group,sacc_self()->node,sacc_self()->group);
        return false;
    }

    if (strcmp(sacc_self()->role, SACC_NODE_ROLE_T2) != 0){
        ogs_info("this node %d is not work in %s mode, ignore it.",sacc_self()->node, SACC_NODE_ROLE_T2);
        return false;
    }

    //g_sacc_nodes[node].state = SACC_PEER_STATE_ONLINE; //收到对端请求,并不需要修改状态
    //g_sacc_nodes[node].heartbeatLost = 0;//重置心跳丢失计数

    snprintf(msg_data.deviceId, sizeof(msg_data.deviceId), "%s", g_sacc_nodes[sacc_self()->node].deviceId);
    snprintf(msg_data.group, sizeof(msg_data.group), "%d", sacc_self()->group);
    snprintf(msg_data.node, sizeof(msg_data.node), "%d", sacc_self()->node);
    OGS_ADDR(g_sacc_nodes[sacc_self()->node].addr, msg_data.serviceIp);
    snprintf(msg_data.result, sizeof(msg_data.result), "OK");

    int nf_num = 0;
    snprintf(msg_data.nfInstanceIds[nf_num].nf_type, sizeof(msg_data.nfInstanceIds[0].nf_type), "%s", OpenAPI_nf_type_ToString(g_sacc_nodes[sacc_self()->node].amf_nf_instance->nf_type));
    snprintf(msg_data.nfInstanceIds[nf_num++].id, sizeof(msg_data.nfInstanceIds[0]), "%s", g_sacc_nodes[sacc_self()->node].amf_nf_instance->id);

    snprintf(msg_data.nfInstanceIds[nf_num].nf_type, sizeof(msg_data.nfInstanceIds[0].nf_type), "%s", OpenAPI_nf_type_ToString(g_sacc_nodes[sacc_self()->node].smf_nf_instance->nf_type));
    snprintf(msg_data.nfInstanceIds[nf_num++].id, sizeof(msg_data.nfInstanceIds[0]), "%s", g_sacc_nodes[sacc_self()->node].smf_nf_instance->id);

    // snprintf(msg_data.nfInstanceIds[nf_num].nf_type, sizeof(msg_data.nfInstanceIds[0].nf_type), "%s", OpenAPI_nf_type_ToString(g_sacc_nodes[sacc_self()->node].udm_nf_instance->nf_type));
    // snprintf(msg_data.nfInstanceIds[nf_num++].id, sizeof(msg_data.nfInstanceIds[0]), "%s", g_sacc_nodes[sacc_self()->node].udm_nf_instance->id);

    // snprintf(msg_data.nfInstanceIds[nf_num].nf_type, sizeof(msg_data.nfInstanceIds[0].nf_type), "%s", OpenAPI_nf_type_ToString(g_sacc_nodes[sacc_self()->node].ausf_nf_instance->nf_type));
    // snprintf(msg_data.nfInstanceIds[nf_num++].id, sizeof(msg_data.nfInstanceIds[0]), "%s", g_sacc_nodes[sacc_self()->node].ausf_nf_instance->id);

    msg_data.nfNum = nf_num;
    sendmsg.sacc_msg_Data = &msg_data;
    sendmsg.http.location = recvmsg->h.uri;

    ogs_info("node %d send sacc %s response to node:%s.",sacc_self()->node, sacc_msg_ToString(msg_type), recvmsg->sacc_msg_Data->node);

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

    ogs_info("node %d receive sacc %s response, peer node info: deviceId:%s group:%s node:%s serviceIp:%s result:%s",sacc_self()->node, sacc_msg_ToString(msg_type),recv_msg_Data->deviceId,recv_msg_Data->group,recv_msg_Data->node,recv_msg_Data->serviceIp, recv_msg_Data->result);

    if (node > sacc_self()->nodeNum || node < 1){
        ogs_info("incomming node %d is out of range, ignore it.",node);
        return false;
    }

    if (group != sacc_self()->group){
        ogs_info("incomming node %d has different group %d with this node %d's group %d, ignore it.",node,group,sacc_self()->node,sacc_self()->group);
        return false;
    }

    if (strcmp(recv_msg_Data->result, "OK") != 0){
        ogs_info("incomming node %d sacc %s response result is not OK, ignore it.",node, sacc_msg_ToString(msg_type));
        g_sacc_nodes[node].state = SACC_PEER_STATE_OFFLINE;
        return false;
    }

    // 遍历nfInstanceIds数组，将ID赋值给对应的NF实例
    for (int i = 0; i < recv_msg_Data->nfNum; ++i) {
        if (strcmp(recv_msg_Data->nfInstanceIds[i].nf_type, "AMF") == 0) {
            ogs_sbi_nf_instance_set_id(g_sacc_nodes[node].amf_nf_instance, recv_msg_Data->nfInstanceIds[i].id);
        } else if (strcmp(recv_msg_Data->nfInstanceIds[i].nf_type, "SMF") == 0) {
            ogs_sbi_nf_instance_set_id(g_sacc_nodes[node].smf_nf_instance, recv_msg_Data->nfInstanceIds[i].id);
        }// } else if (strcmp(recv_msg_Data->nfInstanceIds[i].nf_type, "UDM") == 0) {
        //     ogs_sbi_nf_instance_set_id(g_sacc_nodes[node].udm_nf_instance, recv_msg_Data->nfInstanceIds[i].id);
        // } else if (strcmp(recv_msg_Data->nfInstanceIds[i].nf_type, "AUSF") == 0) {
        //     ogs_sbi_nf_instance_set_id(g_sacc_nodes[node].ausf_nf_instance, recv_msg_Data->nfInstanceIds[i].id);
        // }
    }   

    if (g_sacc_nodes[node].state == SACC_PEER_STATE_OFFLINE){
        g_sacc_nodes[node].state = SACC_PEER_STATE_ONLINE;
        //sacc_nnrf_nfm_send_nf_register(g_sacc_nodes[node].udm_nf_instance);
        //sacc_nnrf_nfm_send_nf_register(g_sacc_nodes[node].ausf_nf_instance);
        sacc_nnrf_nfm_send_nf_register(g_sacc_nodes[node].smf_nf_instance);
        sacc_nnrf_nfm_send_nf_register(g_sacc_nodes[node].amf_nf_instance);
    }else{
        //sacc_nnrf_nfm_send_nf_update(g_sacc_nodes[node].udm_nf_instance);
        //sacc_nnrf_nfm_send_nf_update(g_sacc_nodes[node].ausf_nf_instance);
        sacc_nnrf_nfm_send_nf_update(g_sacc_nodes[node].smf_nf_instance);
        sacc_nnrf_nfm_send_nf_update(g_sacc_nodes[node].amf_nf_instance);
    }

    // 减少握手周期. 内部实现,不需要按方案中连续三次应答再调整周期，那会复杂很多
    if (g_sacc_nodes[node].heartbeatLost >= (MAC_HEARTBEAT_LOST_CNT * 2)) {
        // 从20秒调整为10秒
        g_sacc_nodes[node].heartbeatLost = MAC_HEARTBEAT_LOST_CNT;
    } else {// 从10秒调整为5秒
        g_sacc_nodes[node].heartbeatLost = 0;
    }
    return true;
}

//参考ogs_nnrf_nfm_send_nf_register
bool sacc_nnrf_nfm_send_nf_register(ogs_sbi_nf_instance_t *nf_instance)
{
    bool rc;
    ogs_sbi_request_t *request = NULL;

    ogs_assert(nf_instance);

    ogs_info("sacc_nnrf_nfm_send_nf_register, nf-type:%s,id:%s.", OpenAPI_nf_type_ToString(nf_instance->nf_type),nf_instance->id);

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

bool sacc_nnrf_nfm_send_nf_update(ogs_sbi_nf_instance_t *nf_instance)
{
    bool rc;
    ogs_sbi_request_t *request = NULL;

    ogs_assert(nf_instance);

    request = sacc_nnrf_nfm_build_update(nf_instance);
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

ogs_sbi_request_t *sacc_nnrf_nfm_build_update(ogs_sbi_nf_instance_t *nf_instance)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    OpenAPI_list_t *PatchItemList = NULL;
    OpenAPI_patch_item_t StatusItem;
    OpenAPI_patch_item_t LoadItem;

    ogs_assert(nf_instance);
    ogs_assert(nf_instance->id);

    memset(&StatusItem, 0, sizeof(StatusItem));
    memset(&LoadItem, 0, sizeof(LoadItem));

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_PATCH;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NNRF_NFM;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] =
        (char *)OGS_SBI_RESOURCE_NAME_NF_INSTANCES;
    message.h.resource.component[1] = nf_instance->id;

    message.http.content_type = (char *)OGS_SBI_CONTENT_PATCH_TYPE;

    PatchItemList = OpenAPI_list_create();
    if (!PatchItemList) {
        ogs_error("No PatchItemList");
        goto end;
    }

    StatusItem.op = OpenAPI_patch_operation_replace;
    StatusItem.path = (char *)OGS_SBI_PATCH_PATH_NF_STATUS;
    StatusItem.value = OpenAPI_any_type_create_string(
        OpenAPI_nf_status_ToString(OpenAPI_nf_status_REGISTERED));
    if (!StatusItem.value) {
        ogs_error("No status item.value");
        goto end;
    }

    OpenAPI_list_add(PatchItemList, &StatusItem);

    LoadItem.op = OpenAPI_patch_operation_replace;
    LoadItem.path = (char *)OGS_SBI_PATCH_PATH_LOAD;
    LoadItem.value = OpenAPI_any_type_create_number(nf_instance->load);
    if (!LoadItem.value) {
        ogs_error("No load item.value");
        goto end;
    }

    OpenAPI_list_add(PatchItemList, &LoadItem);

    message.PatchItemList = PatchItemList;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

end:
    if (LoadItem.value)
        OpenAPI_any_type_free(LoadItem.value);
    if (StatusItem.value)
        OpenAPI_any_type_free(StatusItem.value);
    if (PatchItemList)
        OpenAPI_list_free(PatchItemList);

    return request;
}
