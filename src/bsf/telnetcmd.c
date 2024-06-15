#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];

void showsess(char * id);
void showsessBriefAll( void );
void showsessDetail( char *id );
void setCommands(void);

telnet_command_t g_commands[] = {
    {"shownf",        (GenericFunc)shownf,           1, {STRING}},   
    {"showsess",      (GenericFunc)showsess,         1, {STRING}},
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

void setCommands(void){    
    set_telnet_commands(g_commands, g_numCommands);
}


void showsess(char * id)
{
    if(id == NULL || strlen(id) == 0){
        showsessBriefAll();
    }else{
        showsessDetail(id);
    }

    return;
}

void showsessBriefAll( void )
{
    bsf_sess_t *sess = NULL;
    char buf1[OGS_ADDRSTRLEN];
    char buf2[OGS_ADDRSTRLEN];
    
    printf("\nbsf sess Brief All(current %u sess count):\r\n", ogs_list_count(&bsf_self()->sess_list));
    printf("+-------------+----------------------+----------------------+----------------+----------------------------------------+----------+\n\r");
    printf("|  binding_id |         supi         |         gpsi         |     ipv4       |                 ipv6                   |    dnn   |\n\r");
    printf("+-------------+----------------------+----------------------+----------------+----------------------------------------+----------+\n\r");
    
    ogs_list_for_each(&bsf_self()->sess_list, sess) {
        printf("| %-11s | %-20s | %-20s | %-14s | %-38s | %-8s |\r\n",
               sess->binding_id,
               sess->supi,
               sess->gpsi,
               sess->ipv4addr_string,
               sess->ipv6prefix_string,
               sess->dnn);
    }
    
    printf("+-------------+----------------------+----------------------+----------------+----------------------------------------+----------+\n\r");
    printf("\r\n");
    
    return ;
}

void showsessDetail( char *id )
{
    bsf_sess_t *sess = NULL;
    char buf1[OGS_ADDRSTRLEN];
    char buf2[OGS_ADDRSTRLEN];
    int i;
    int pdr_index = 0;
    OpenAPI_lnode_t *node = NULL;
    
    printf("\r\n");
    sess = bsf_sess_find_by_binding_id(id);
    
    if (sess == NULL){
        printf("can not find bsf sess by binding_id:%s \r\n",id);
        return;
    }
    
    printf("The bsf sess(binding_id=%s) Detail Info is the following: \r\n", id);
    printf("  |--binding_id        : %s \r\n", sess->binding_id);
    printf("  |--supi              : %s \r\n", sess->supi);
    printf("  |--gpsi              : %s \r\n",  sess->gpsi);
    printf("  |--ipv6prefix_string : %s \r\n",  sess->ipv6prefix_string);
    printf("  |--s_nssai           : SST:%d SD:0x%x \r\n", sess->s_nssai.sst,sess->s_nssai.sd.v);
    printf("  |--dnn               : %s \r\n",  sess->dnn);
    printf("  |--pcf_fqdn          : %s \r\n",  sess->pcf_fqdn);
    printf("  |--num_of_pcf_ip     : %d \r\n",  sess->num_of_pcf_ip);
    for (i = 0; i < sess->num_of_pcf_ip; i++){
        printf("    |--addr        : %s \r\n",  sess->pcf_ip[i].addr);
        printf("    |--addr6       : %s \r\n",  sess->pcf_ip[i].addr6);
        printf("    |--is_port     : %d \r\n",  sess->pcf_ip[i].is_port);
        printf("    |--port        : %d \r\n",  sess->pcf_ip[i].port);
    }

    if (sess->ipv4_frame_route_list) {
        printf("  |--ipv4_frame_route_list: \r\n");
        OpenAPI_list_for_each(sess->ipv4_frame_route_list, node) {
            OpenAPI_frame_route_info_t *route = node->data;
            if (i >= OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI) break;
            if (!route) continue;
            printf("    |--ipv4_mask        : %s \r\n",  route->ipv4_mask);
        }
    }    

    if (sess->ipv6_frame_route_list) {
        printf("  |--ipv6_frame_route_list: \r\n");
        OpenAPI_list_for_each(sess->ipv6_frame_route_list, node) {
            OpenAPI_frame_route_info_t *route = node->data;
            if (i >= OGS_MAX_NUM_OF_FRAMED_ROUTES_IN_PDI) break;
            if (!route) continue;
            printf("    |--ipv6_prefix      : %s \r\n",  route->ipv6_prefix);
        }
    }        
    printf("  |--management_features: %ld \r\n",  sess->management_features);
    printf("\r\n"); 
    
    return ;
}
