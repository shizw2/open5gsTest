#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void showue(char *id);
void showueBriefAll( void );
void showueDetail( char * id );
void setCommands(void);

telnet_command_t g_commands[] = {
    {"shownf",      (GenericFunc)shownf,         1, {STRING}},
    {"showue",      (GenericFunc)showue,         1, {STRING}},
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

void setCommands(void){    
    set_telnet_commands(g_commands, g_numCommands);
}


void showue(char * id)
{
    if(id == NULL || strlen(id) == 0){
        showueBriefAll();
    }else{
        showueDetail(id);
    }

    return;
}

void showueBriefAll( void )
{
    udm_ue_t *ue = NULL;
    char buffer[20] = {};
    char buffer2[20] = {};
    struct tm tm;
	
    printf("\nsmf ue Brief All(current %u ue count):\r\n", ogs_list_count(&udm_self()->udm_ue_list));
    printf("+--------+----------------------+----------------------+-------------+-------------------------------------+\n\r");
    printf("| ctx_id |         supi         |        plmn          |    amf_id   |            srv_network_name         |\n\r");
    printf("+--------+----------------------+----------------------+-------------+-------------------------------------+\n\r");
    
    ogs_list_for_each(&udm_self()->udm_ue_list, ue) {
        ogs_plmn_id_to_string(&ue->guami.plmn_id, buffer);
		printf("| %-6s | %-15s | %-15s | %-11s | %s |\r\n",
		   ue->ctx_id,
           ue->supi,
           buffer,           
           ogs_amf_id_to_string(&ue->guami.amf_id),
           ue->serving_network_name
           );        
    }
    
    printf("+--------+----------------------+----------------------+-------------+-------------------------------------+\n\r");
    printf("\r\n");
    
    return ;
}

void showueDetail( char *id )
{
    udm_ue_t *ue = NULL;
    char buffer[20] = {};
    char sqn_string[OGS_KEYSTRLEN(OGS_SQN_LEN)];
    char rand_string[OGS_KEYSTRLEN(OGS_RAND_LEN)];
    char k_string[OGS_KEYSTRLEN(OGS_KEY_LEN)];
    char opc_string[OGS_KEYSTRLEN(OGS_KEY_LEN)];
    char amf_string[OGS_KEYSTRLEN(OGS_AMF_LEN)];
    
    ue = udm_ue_find_by_ctx_id(id);
    
    if (ue == NULL){
		printf("cat not find udm ue by id:%s\r\n",id);
		return;
	}
    
    ogs_plmn_id_to_string(&ue->guami.plmn_id, buffer);
    
    ogs_hex_to_ascii(ue->sqn, OGS_SQN_LEN, sqn_string, sizeof(sqn_string));
    ogs_hex_to_ascii(ue->rand, OGS_RAND_LEN,rand_string, sizeof(rand_string));
    ogs_hex_to_ascii(ue->k, OGS_RAND_LEN,k_string, sizeof(k_string));
    ogs_hex_to_ascii(ue->opc, OGS_RAND_LEN,opc_string, sizeof(opc_string));
    ogs_hex_to_ascii(ue->amf, OGS_RAND_LEN,amf_string, sizeof(amf_string));
    
	printf("The udm ue(ctx_id=%s) detail info is the following: \r\n", id);
	printf("  |--ctx_id             : %s \r\n", ue->ctx_id);   
	printf("  |--supi               : %s \r\n", ue->supi);
	printf("  |--suci               : %s \r\n", ue->suci);
	printf("  |--serving_network_name: %s \r\n", ue->serving_network_name);
	printf("  |--ausf_instance_id   : %s \r\n", ue->ausf_instance_id);
    printf("  |--amf_instance_id    : %s \r\n", ue->amf_instance_id);
    printf("  |--dereg_callback_uri : %s \r\n", ue->dereg_callback_uri);
    printf("  |--plmn_id            : %s \r\n", buffer);
    printf("  |--amf_id             : %s \r\n", ogs_amf_id_to_string(&ue->guami.amf_id));
    printf("  |--k                  : %s \r\n", k_string);
    printf("  |--opc                : %s \r\n", opc_string);
    printf("  |--sqn                : %s \r\n", sqn_string);
    printf("  |--amf                : %s \r\n", amf_string);
    printf("  |--rand               : %s \r\n", rand_string);
    printf("  |--auth_type          : %s(%d) \r\n", OpenAPI_auth_type_ToString(ue->auth_type),ue->auth_type);
    printf("  |--rat_type           : %s(%d) \r\n", OpenAPI_rat_type_ToString(ue->rat_type),ue->rat_type);
    return ;
}
