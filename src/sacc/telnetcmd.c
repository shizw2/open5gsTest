#include "telnet.h"
#include "context.h"
#include "ogs-sbi.h"
#include "sacc-handler.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void setCommands(void);
void show_node(int node);
void show_nodes(void);
void show_node_details(int node_id);
void getnetworkStatus(void);
void print_supiRanges(ogs_supi_range_t *supiRanges);
void print_ipRanges(ogs_ip_range_t *ipRanges);

telnet_command_t g_commands[] = {
    {"shownf",          (GenericFunc)shownf,         1, {STRING}},
    {"show_node",      (GenericFunc)show_node,      1, {INTEGER}},  
    {"getnetworkStatus",(GenericFunc)getnetworkStatus,      0, {}},    
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

void setCommands(void){
    set_telnet_commands(g_commands, g_numCommands);
}

extern sacc_node_t g_sacc_nodes[MAX_PEER_NUM+1];

void show_node(int node){
    if(node == 0 ){
        show_nodes();
    }else{
        show_node_details(node);
    }

    return;
}

void show_nodes(void) {
    printf("\nsacc nodes information:\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    printf("| node   | group  |  state  |   deviceID     | role   |    addr        |\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    char buf[OGS_ADDRSTRLEN];
    for (int i = 1; i <= sacc_self()->nodeNum && i <=MAX_PEER_NUM; i++) {
        sacc_node_t *node = &g_sacc_nodes[i];
        printf("| %6d | %6d | %7s | %-14s | %-6s | %-14s |\n",
               node->node, node->group, sacc_node_state_ToString(node->state),
               node->deviceId, node->role, OGS_ADDR(node->ipv4[0], buf));
    }

    printf("+--------+--------+---------+----------------+--------+----------------+\n");
}

void show_node_details(int node_id) {
    char buf[OGS_ADDRSTRLEN];

    if (node_id > MAX_PEER_NUM){
        printf("node_id is not exist.\n");
        return;
    }
    sacc_node_t *node = &g_sacc_nodes[node_id];

    if (!node) {
        printf("Node details are NULL.\n");
        return;
    }

    printf("The node detail info is the following: \r\n");
    printf("  |--group              : %d \r\n", node->group);
    printf("  |--node               : %d \r\n", node->node);
    printf("  |--priority           : %d \r\n", node->priority);
    printf("  |--state              : %s \r\n", sacc_node_state_ToString(node->state));
    printf("  |--deviceId           : %s \r\n", node->deviceId);
    printf("  |--role               : %s \r\n", node->role);
    printf("  |--heartbeatLost      : %d \r\n", node->heartbeatLost);
    //printf("  |--address            : %s \r\n", OGS_ADDR(node->addr, buf));
    printf("  |--URI                : %s \r\n", node->uri);

    printf("  |--IPv4 addresses     : \r\n");
    for (int i = 0; i < node->num_of_ipv4; i++) {
        printf("    |--%d                : %s \r\n", i + 1, OGS_ADDR(node->ipv4[i], buf));
    }

    printf("  |--IPv6 addresses     : \r\n");
    for (int i = 0; i < node->num_of_ipv6; i++) {
        printf("    |--%d                : %s \r\n", i + 1, OGS_ADDR(node->ipv6[i], buf));
    }
    printf("  |--supiRanges         : \r\n");
    print_supiRanges(&node->supiRanges);
    printf("  |--staticIPRanges     : \r\n");
    print_ipRanges(&node->staticIPRanges);

    // Print NF instances details if they exist
    if (node->amf_nf_instance) {
        printf("  |--AMF NF Instance   : \r\n");
        printf("    |--ID               : %s \r\n", node->amf_nf_instance->id);
        // Add more details as needed
    }
    if (node->smf_nf_instance) {
        printf("  |--SMF NF Instance   : \r\n");
        printf("    |--ID               : %s \r\n", node->smf_nf_instance->id);
        // Add more details as needed
    }
    if (node->udm_nf_instance) {
        printf("  |--UDM NF Instance   : \r\n");
        printf("    |--ID               : %s \r\n", node->udm_nf_instance->id);
        // Add more details as needed
    }
    if (node->ausf_nf_instance) {
        printf("  |--AUSF NF Instance  : \r\n");
        printf("    |--ID               : %s \r\n", node->ausf_nf_instance->id);
        // Add more details as needed
    }
}

void getnetworkStatus(void) {
    cJSON *root = cJSON_CreateObject();
    cJSON *nodeInfo = cJSON_CreateArray();

    int maxNum = sacc_self()->nodeNum;
    int currNum = 0;

    for (int i = 1; i <= sacc_self()->nodeNum && i <=MAX_PEER_NUM; i++) {
        sacc_node_t *node = &g_sacc_nodes[i];
        cJSON *json_object = cJSON_CreateObject();

        const char *object_type = (node->group == sacc_self()->group && node->node == sacc_self()->node) ? "local" : "peer";
        cJSON_AddStringToObject(json_object, "object", object_type);
        cJSON_AddNumberToObject(json_object, "group", node->group);
        cJSON_AddNumberToObject(json_object, "node", node->node);
        if (node->state == SACC_PEER_STATE_ONLINE){
            currNum++;
        }
        char buf[OGS_ADDRSTRLEN];
        if (node->ipv4[0] != NULL) {
            cJSON_AddStringToObject(json_object, "ip", ogs_inet_ntop(node->ipv4[0], buf, sizeof(buf)));
        }
        cJSON_AddStringToObject(json_object, "deviceSeq", node->deviceId);
        cJSON_AddStringToObject(json_object, "status", sacc_node_state_ToString(node->state));

        cJSON_AddItemToArray(nodeInfo, json_object);
    }

    cJSON_AddNumberToObject(root, "maxNum", maxNum);
    cJSON_AddNumberToObject(root, "currNum", currNum);
    cJSON_AddItemToObject(root, "nodeInfo", nodeInfo);

    char *json_string = cJSON_Print(root);
    printf("%s\n", json_string);

    // 释放 cJSON 对象
    cJSON_Delete(root);
    ogs_free(json_string);
}