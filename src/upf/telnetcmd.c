#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void upf(void);
void showsess(uint32_t id);
void showsessBriefAll( void );

void upf(void)
{
    printf("this is upf system. \r\n");
}

void telnet_proc_cmd(char * pabCmd)
{
    uint32_t  dwPara1   = 0;
    uint32_t  dwPara2   = 0;
   
    if (!pttGetCmdParams(pabCmd))
    {
        return;
    }

    dwPara1 = pttGetCmdWord32Value(&g_tCmdPara[0]);
    dwPara2 = pttGetCmdWord32Value(&g_tCmdPara[1]);

    if (strcmp(g_chCmdName, "upf") == 0){
        upf();
    }else if (strcmp(g_chCmdName, "showsess") == 0){
        showsess(dwPara1);
    }else{
        printf("the command not support\r\n");
    }  
    
    return;
}

void showsess(uint32_t id)
{
    if(id == 0 )
    {
        showsessBriefAll();
    }
    else
    {
        //showgnbDetail(enbID);
    }

    return;
}

void showsessBriefAll( void )
{
    upf_sess_t *sess = NULL;
    char buf1[OGS_ADDRSTRLEN];
    char buf2[OGS_ADDRSTRLEN];
    
    printf("\nupf sess Brief All(current %u sess count):\r\n", ogs_list_count(&upf_self()->sess_list));
    printf("+-------------+-------------+----------------+----------------+----------------------------------------+----------+\n\r");
    printf("| upf_n4_seid | smf_n4_seid |   smf_n4_ip    |     ipv4       |                 ipv6                   | apn_dnn  |\n\r");
    printf("+-------------+-------------+----------------+----------------+----------------------------------------+----------+\n\r");
    
    ogs_list_for_each(&upf_self()->sess_list, sess) {
        printf("| %-11lu | %-11ld | %-14s | %-14s | %-38s | %-8s |\r\n",
               sess->upf_n4_seid,
               sess->smf_n4_f_seid.seid,
               sess->smf_n4_f_seid.ip.ipv4 ? ogs_ipv4_to_string(sess->smf_n4_f_seid.ip.addr):"",
               sess->ipv4 ? OGS_INET_NTOP(&sess->ipv4->addr, buf1) : "",
               sess->ipv6 ? OGS_INET6_NTOP(&sess->ipv6->addr, buf2) : "",
               sess->apn_dnn);
    }
    
        printf("+-------------+-------------+----------------+----------------+----------------------------------------+----------+\n\r");
    printf("\r\n");
    
    return ;
}
#if 0
void shownf(uint32_t id){
    if(id == 0 ){
        shownfBriefAll();
    }else{
        //showgnbDetail(id);
    }

    return;
}

void shownfBriefAll(void){
    ogs_sbi_nf_instance_t *nf_instance = NULL;
    char buf[OGS_ADDRSTRLEN];
    
    printf("\nnf instance Brief All(current %u nf count):\r\n", ogs_list_count(&ogs_sbi_self()->nf_instance_list));
    printf("+--------------------------------------+---------+------------+----------+----------+--------------------+\n\r");
    printf("|                 nf_id                | nf_type |    status  | capacity | ref_cnt  |    ipv4_address    |\n\r");
    printf("+--------------------------------------+---------+------------+----------+----------+--------------------+\n\r");

    ogs_list_for_each(&ogs_sbi_self()->nf_instance_list, nf_instance) {
        char addrInfo[OGS_ADDRSTRLEN] = {0};
        if (nf_instance->num_of_ipv4 > 0){
            sprintf(addrInfo,"%s:%d",OGS_ADDR(nf_instance->ipv4[0], buf), OGS_PORT(nf_instance->ipv4[0]));
        }
        printf("| %-36s | %-7s | %-10s | %-8d | %-8u | %-18s |\r\n",nf_instance->id, 
        OpenAPI_nf_type_ToString(nf_instance->nf_type),
        OpenAPI_nf_status_ToString(nf_instance->nf_status),
        nf_instance->capacity,
        nf_instance->reference_count,
        addrInfo); 
    }
    printf("+--------------------------------------+---------+------------+----------+----------+--------------+\n\r");
}

void showranue( void )
{
    amf_gnb_t *gnb = NULL;
    ran_ue_t *ran_ue = NULL;
    
    printf("\ngnb Brief All(current %u gnb count):\r\n", ogs_list_count(&amf_self()->gnb_list));
    printf("+---------+----------------+----------------+--------+----------------------+--------+\n\r");
    printf("| gnb_id  | ran_ue_ngap_id | amf_ue_ngap_id | sps_no |         supi         |  tmsi  |\n\r");
    printf("+---------+----------------+----------------+--------+----------------------+--------+\n\r");
    
    ogs_list_for_each(&amf_self()->gnb_list, gnb) {
        ogs_list_for_each(&gnb->ran_ue_list, ran_ue) {
            printf("| %-7u | %-14d | %-14lu | %-6d | %-15s | %-6u |\r\n",
               gnb->gnb_id, 
               ran_ue->ran_ue_ngap_id,
               ran_ue->amf_ue_ngap_id,
               ran_ue->sps_no,
               ran_ue->supi,
               ran_ue->m_tmsi);
        }
    }
    
    printf("+---------+----------------+----------------+--------+----------------------+\n\r");
    
    printf("\r\n");
    
    return ;
}
#endif