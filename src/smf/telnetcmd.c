#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void setCommands(void);
void amf(void);
void showgnb(uint32_t enbID);
void showgnbBriefAll( void );
void showranue( void );

void showue(char* id);
void showueAll(void);
void showueDetail(char * id);


telnet_command_t g_commands[] = {
    {"showue",      (GenericFunc)showue,         1, {INTEGER}},
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

void setCommands(void){    
    set_telnet_commands(g_commands, g_numCommands);
}

void showue(char * id){
    if(id == NULL || strlen(id) == 0){
        showueAll();
    }else{
        showueDetail(id);
    }

    return;
}

void showueAll( void )
{
    smf_ue_t *ue = NULL;
    char buffer[20] = {};
    struct tm tm;
	
    printf("\nsmf ue Brief All(current %u ue count):\r\n", ogs_list_count(&smf_self()->smf_ue_list));
    printf("+----------------------+----------------------+----------------------+----------------------+-------------+\n\r");
    printf("|         supi         |         imsi         |        msisdn        |        imeisv        | session cnt |\n\r");
    printf("+----------------------+----------------------+----------------------+----------------------+-------------+\n\r");
    
    ogs_list_for_each(&smf_self()->smf_ue_list, ue) {
		printf("| %-15s | %-15s | %-15s | %-15s | %d |\r\n",
		   ue->supi,
           ue->imsi_bcd,
           ue->msisdn_bcd,
           ue->imeisv_bcd,
           ogs_list_count(&ue->sess_list));        
    }
    
    printf("+----------------------+---------------+---------------------+---------------------------+---------------------+\n\r");
    printf("\r\n");
    
    return ;
}

void showueDetail( char * supi )
{
    
}
