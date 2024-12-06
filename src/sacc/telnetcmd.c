#include "telnet.h"
#include "context.h"
#include "ogs-sbi.h"
#include "sacc-handler.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void setCommands(void);
void showsaccnodes(void);
void getnetworkStatus(void);

telnet_command_t g_commands[] = {
    {"shownf",      (GenericFunc)shownf,         1, {STRING}},
    {"showsaccnodes", (GenericFunc)showsaccnodes,      0, {}},  
    {"getnetworkStatus", (GenericFunc)getnetworkStatus,      0, {}},    
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

void setCommands(void){
    set_telnet_commands(g_commands, g_numCommands);
}

extern sacc_node_t g_sacc_nodes[MAX_PEER_NUM+1];

void showsaccnodes(void) {
    printf("\nsacc nodes information:\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    printf("| node   | group  |  state  |   deviceID     | role   |    addr        |\n");
    printf("+--------+--------+---------+----------------+--------+----------------+\n");
    char buf[OGS_ADDRSTRLEN];
    for (int i = 1; i <= sacc_self()->nodeNum && i <=MAX_PEER_NUM; i++) {
        sacc_node_t *node = &g_sacc_nodes[i];
        printf("| %6d | %6d | %7s | %-14s | %-6s | %-14s |\n",
               node->node, node->group, sacc_node_state_ToString(node->state),
               node->deviceId, node->role, OGS_ADDR(node->addr, buf));
    }

    printf("+--------+--------+---------+----------------+--------+----------------+\n");
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
        if (node->addr != NULL) {
            cJSON_AddStringToObject(json_object, "ip", ogs_inet_ntop(node->addr, buf, sizeof(buf)));
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