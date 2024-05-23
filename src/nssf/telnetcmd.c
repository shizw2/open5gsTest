#include "telnet.h"
#include "context.h"
#include "ogs-sbi.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void setCommands(void);
void shownsi( void );

telnet_command_t g_commands[] = {
    {"shownf",      (GenericFunc)shownf,         1, {STRING}},
    {"shownsi",     (GenericFunc)shownsi,        0, {}},
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

void setCommands(void){
    set_telnet_commands(g_commands, g_numCommands);
}

void shownsi( void )
{
    nssf_nsi_t *nsi = NULL;
    char buf[OGS_ADDRSTRLEN];

    printf("\nnssf nsi Brief All(current %u nsi count):\r\n", ogs_list_count(&nssf_self()->nsi_list));
    printf("+--------+----------------------+----------------------+\n\r");
    printf("| nsi_id |        nrf_id        |       s_nssai        |\n\r");
    printf("+--------+----------------------+----------------------+\n\r");
    
    ogs_list_for_each(&nssf_self()->nsi_list, nsi) {
        //char addrStr[20];
        char sstSdStr[20];
        
        //snprintf(addrStr, sizeof(addrStr), "[%s]:%d", OGS_ADDR(nsi->addr, buf), OGS_PORT(nsi->addr));
        snprintf(sstSdStr, sizeof(sstSdStr), "SST:%d SD:0x%x", nsi->s_nssai.sst, nsi->s_nssai.sd.v);

        printf("| %-6s | %-20s | %-20s |\r\n",
		   nsi->nsi_id,
           nsi->nrf_id,
           sstSdStr);     
    }	
    
    printf("+--------+----------------------+----------------------+\n\r");
    printf("\r\n");
    
    return ;
}