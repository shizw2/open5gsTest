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
    ausf_ue_t *ue = NULL;
    char buffer[20] = {};
    char buffer2[20] = {};
    struct tm tm;
	
    printf("\nausf ue Brief All(current %u ue count):\r\n", ogs_list_count(&ausf_self()->ausf_ue_list));
    printf("+--------+----------------------+------------------------+------------------------------------+\n\r");
    printf("| ctx_id |         supi         |       auth_result      |        serving_network_name        |\n\r");
    printf("+--------+----------------------+------------------------+------------------------------------+\n\r");
    
    ogs_list_for_each(&ausf_self()->ausf_ue_list, ue) {
        printf("| %-6s | %-15s | %-23s | %-20s |\r\n",
		   ue->ctx_id,
           ue->supi,
           OpenAPI_auth_result_ToString(ue->auth_result),
           ue->serving_network_name
           );        
    }
    
   printf("+--------+----------------------+------------------------+------------------------------------+\n\r");
    printf("\r\n");
    
    return ;
}

void showueDetail( char *id )
{
    ausf_ue_t *ue = NULL;
    char rand_string[OGS_KEYSTRLEN(OGS_RAND_LEN)];
    char xres_star_string[OGS_KEYSTRLEN(OGS_MAX_RES_LEN)];
    char hxres_star_string[OGS_KEYSTRLEN(OGS_MAX_RES_LEN)];
    char kausf_string[OGS_KEYSTRLEN(OGS_SHA256_DIGEST_SIZE)];
    char kseaf_string[OGS_KEYSTRLEN(OGS_SHA256_DIGEST_SIZE)];
    ue = ausf_ue_find_by_ctx_id(id);
    
    if (ue == NULL){
		printf("cat not find ausf ue by id:%s\r\n",id);
		return;
	}
    
    ogs_hex_to_ascii(ue->rand, OGS_RAND_LEN,rand_string, sizeof(rand_string));
    ogs_hex_to_ascii(ue->xres_star, OGS_MAX_RES_LEN,xres_star_string, sizeof(xres_star_string));
    ogs_hex_to_ascii(ue->hxres_star, OGS_MAX_RES_LEN,hxres_star_string, sizeof(hxres_star_string));
    ogs_hex_to_ascii(ue->kausf, OGS_SHA256_DIGEST_SIZE,kausf_string, sizeof(kausf_string));
    ogs_hex_to_ascii(ue->kseaf, OGS_SHA256_DIGEST_SIZE,kseaf_string, sizeof(kseaf_string));
    
	printf("The ausf ue(ctx_id=%s) detail info is the following: \r\n", id);
	printf("  |--ctx_id             : %s \r\n", ue->ctx_id);   
	printf("  |--supi               : %s \r\n", ue->supi);
	printf("  |--suci               : %s \r\n", ue->suci);
	printf("  |--serving_network_name: %s \r\n", ue->serving_network_name);
	printf("  |--auth_type          : %s(%d) \r\n", OpenAPI_auth_type_ToString(ue->auth_type),ue->auth_type);
    printf("  |--auth_events_url    : %s \r\n", ue->auth_events_url);
    printf("  |--auth_result        : %s(%d) \r\n", OpenAPI_auth_result_ToString(ue->auth_result),ue->auth_result);
    printf("  |--rand               : %s \r\n", rand_string);
    printf("  |--xres_star          : %s \r\n", xres_star_string);
    printf("  |--hxres_star         : %s \r\n", hxres_star_string);
    printf("  |--kausf              : %s \r\n", kausf_string);
    printf("  |--kseaf              : %s \r\n", kseaf_string);
    return ;
}
