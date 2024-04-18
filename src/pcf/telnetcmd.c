#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void showue(char *id);
void showueBriefAll( void );
void showueDetail( char * id );
void setCommands(void);

telnet_command_t g_commands[] = {
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
    pcf_ue_t *ue = NULL;
    char buffer[20] = {};
    char buffer2[20] = {};
    struct tm tm;
	
    printf("\npcf ue Brief All(current %u ue count):\r\n", ogs_list_count(&pcf_self()->pcf_ue_list));
    printf("+--------+----------------------+----------+-------------+----------------------+\n\r");
    printf("|   id   |         supi         |   plmn   |    amf_id   |         gpsi         |\n\r");
    printf("+--------+----------------------+----------+-------------+----------------------+\n\r");
    
    ogs_list_for_each(&pcf_self()->pcf_ue_list, ue) {
        ogs_plmn_id_to_string(&ue->guami.plmn_id, buffer);
		printf("| %-6s | %-15s | %-8s | %-11s | %-20s |\r\n",
		   ue->association_id,
           ue->supi,
           buffer,           
           ogs_amf_id_to_string(&ue->guami.amf_id),
           ue->gpsi
           );        
    }
    
    printf("+--------+----------------------+----------+-------------+----------------------+\n\r");
    printf("\r\n");
    
    return ;
}

void showueDetail( char *id )
{
    pcf_ue_t *ue = NULL;
    char buffer[20] = {};

    ue = pcf_ue_find_by_association_id(id);
    
    if (ue == NULL){
		printf("cat not find pcf ue by id:%s\r\n",id);
		return;
	}
    
    ogs_plmn_id_to_string(&ue->guami.plmn_id, buffer);
    

    
	printf("The pcf ue(ctx_id=%s) detail info is the following: \r\n", id);
	printf("  |--association_id     : %s \r\n", ue->association_id);   
	printf("  |--supi               : %s \r\n", ue->supi);
	printf("  |--notification_uri   : %s \r\n", ue->notification_uri);
	printf("  |--gpsi               : %s \r\n", ue->gpsi);
	printf("  |--access_type        : %s(%d) \r\n", OpenAPI_access_type_ToString(ue->access_type),ue->access_type);
    printf("  |--pei                : %s \r\n", ue->pei);
    printf("  |--plmn_id            : %s \r\n", buffer);
    printf("  |--amf_id             : %s \r\n", ogs_amf_id_to_string(&ue->guami.amf_id));   
    printf("  |--rat_type           : %s(%d) \r\n", OpenAPI_rat_type_ToString(ue->rat_type),ue->rat_type);
    printf("  |--am_policy_control_features: %ld \r\n", ue->am_policy_control_features);
    if (ue->subscribed_ue_ambr != NULL){
        printf("  |--uplink             : %s \r\n", ogs_sbi_bitrate_to_string(ue->subscribed_ue_ambr->uplink, OGS_SBI_BITRATE_BPS));
        printf("  |--downlink           : %s \r\n", ogs_sbi_bitrate_to_string(ue->subscribed_ue_ambr->downlink, OGS_SBI_BITRATE_BPS));
    }
    return ;
}
