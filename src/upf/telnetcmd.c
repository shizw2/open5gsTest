#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void upf(void);
void showsess(uint32_t id);
void showsessBriefAll( void );
void showsessDetail( uint32_t id );
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
        showsessDetail(id);
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

void showsessDetail( uint32_t id )
{
    upf_sess_t *sess = NULL;
    ogs_pfcp_pdr_t *pdr = NULL;
    ogs_pfcp_far_t *far = NULL;
    ogs_pfcp_urr_t *urr = NULL;
    ogs_pfcp_qer_t *qer = NULL;
    char buf1[OGS_ADDRSTRLEN];
    char buf2[OGS_ADDRSTRLEN];
    int i;
    printf("\r\n");
    sess = upf_sess_find_by_upf_n4_seid(id);
    
    if (sess == NULL){
        printf("can not find upf sess by upf_n4_seid:%d \r\n",id);
        return;
    }
    
    printf("The upf sess(upf_n4_seid=%u) Detail Info is the following: \r\n", id);
    printf("  |--upf_n4_seid        : %lu \r\n", sess->upf_n4_seid);
    printf("  |--smf_n4_seid        : %lu \r\n", sess->smf_n4_f_seid.seid);
    printf("  |--smf_n4_ip          : %s \r\n", ogs_ipv4_to_string(sess->smf_n4_f_seid.ip.addr));
    printf("  |--pfcp      : \r\n");    
    printf("      |--pdr_cnt           : %d \r\n", ogs_list_count(&sess->pfcp.pdr_list));
    ogs_list_for_each(&sess->pfcp.pdr_list, pdr){
        printf("          **************pdr id: %d***********\r\n",pdr->id);     
        printf("          |--teid              : %d \r\n", pdr->teid);
        printf("          |--src_if            : %d \r\n", pdr->src_if);
        printf("          |--dnn               : %s \r\n", pdr->dnn);
        printf("          |--f_teid            :  \r\n");
        printf("              |--teid             : %d \r\n", pdr->f_teid.teid);
        printf("              |--addr             : %s \r\n", OGS_INET_NTOP(&pdr->f_teid.addr, buf1) );
        printf("              |--addr6            : %s \r\n", OGS_INET_NTOP(&pdr->f_teid.addr6, buf2) );
        printf("          |--chid              : %d \r\n", pdr->chid);
        printf("          |--choose_id         : %d \r\n", pdr->choose_id);
        printf("          |--qfi               : %d \r\n", pdr->qfi);
        printf("          |--num_of_urr        : %d \r\n", pdr->num_of_urr);
        for (i = 0; i < pdr->num_of_urr; i++){
            printf("              |--id                : %d \r\n", pdr->urr[i]->id);
            printf("              |--meas_method       : %d \r\n", pdr->urr[i]->meas_method);
            printf("              |--rep_triggers      : reptri_5:%d reptri_6:%d reptri_7:%d \r\n", pdr->urr[i]->rep_triggers.reptri_5, 
                                                                                                    pdr->urr[i]->rep_triggers.reptri_6, 
                                                                                                    pdr->urr[i]->rep_triggers.reptri_7);
            //printf("              |--meas_info         : %d \r\n", pdr->urr[i]->meas_info);
            printf("              |--meas_period       : %d \r\n", pdr->urr[i]->meas_period);
            printf("              |--vol_threshold     : total:%lu uplink:%lu downlink:%lu\r\n", pdr->urr[i]->vol_threshold.total_volume,
                                                                                                 pdr->urr[i]->vol_threshold.uplink_volume,
                                                                                                 pdr->urr[i]->vol_threshold.downlink_volume);
            printf("              |--vol_quota         : total:%lu uplink:%lu downlink:%lu\r\n", pdr->urr[i]->vol_quota.total_volume,
                                                                                     pdr->urr[i]->vol_quota.uplink_volume,
                                                                                     pdr->urr[i]->vol_quota.downlink_volume);
            printf("              |--event_threshold   : %u\r\n", pdr->urr[i]->event_threshold); 
            printf("              |--event_quota       : %d \r\n", pdr->urr[i]->event_quota);
            printf("              |--time_threshold    : %d \r\n", pdr->urr[i]->time_threshold);
            printf("              |--time_quota        : %d \r\n", pdr->urr[i]->time_quota);
            printf("              |--quota_holding_time: %d \r\n", pdr->urr[i]->quota_holding_time);
            //printf("              |--dropped_dl_traffic_threshold: %d \r\n", pdr->urr[i]->dropped_dl_traffic_threshold);
            printf("              |--quota_validity_time: %d \r\n", pdr->urr[i]->quota_validity_time);
        }
        printf("          |--num_of_flow       : %d \r\n", pdr->num_of_flow);
        printf("          |--num of rule_list  : %d \r\n", ogs_list_count(&pdr->rule_list));
    }
    printf("     |--far_cnt           : %d \r\n", ogs_list_count(&sess->pfcp.far_list));
    ogs_list_for_each(&sess->pfcp.far_list, far){
        printf("          **************far id: %d***********\r\n",far->id);     
        printf("          |--dnn               : %s \r\n", far->dnn);
        printf("          |--dst_if            : %d \r\n", far->dst_if);
        printf("          |--apply_action      : %d \r\n", far->apply_action);
        printf("          |--dst_if            : %d \r\n", far->dst_if);
        printf("          |--num_of_buffered_packet: %d \r\n", far->num_of_buffered_packet);        
        printf("          |--smreq_flags       : %d \r\n", far->smreq_flags.value);
    }
    
    printf("     |--urr_cnt           : %d \r\n", ogs_list_count(&sess->pfcp.urr_list));
    ogs_list_for_each(&sess->pfcp.urr_list, urr){
        printf("          **************urr id: %d***********\r\n",urr->id);     
        printf("          |--meas_method       : %d \r\n", urr->meas_method);
        printf("          |--rep_triggers      : reptri_5:%d reptri_6:%d reptri_7:%d \r\n", urr->rep_triggers.reptri_5, 
                                                                                                urr->rep_triggers.reptri_6, 
                                                                                                urr->rep_triggers.reptri_7);
        //printf("              |--meas_info         : %d \r\n", urr->meas_info);
        printf("          |--meas_period       : %d \r\n", urr->meas_period);
        printf("          |--vol_threshold     : total:%lu uplink:%lu downlink:%lu\r\n", urr->vol_threshold.total_volume,
                                                                                             urr->vol_threshold.uplink_volume,
                                                                                             urr->vol_threshold.downlink_volume);
        printf("          |--vol_quota         : total:%lu uplink:%lu downlink:%lu\r\n", urr->vol_quota.total_volume,
                                                                                 urr->vol_quota.uplink_volume,
                                                                                 urr->vol_quota.downlink_volume);
        printf("          |--event_threshold   : %u\r\n", urr->event_threshold); 
        printf("          |--event_quota       : %d \r\n", urr->event_quota);
        printf("          |--time_threshold    : %d \r\n", urr->time_threshold);
        printf("          |--time_quota        : %d \r\n", urr->time_quota);
        printf("          |--quota_holding_time: %d \r\n", urr->quota_holding_time);
        //printf("        |--dropped_dl_traffic_threshold: %d \r\n", urr->dropped_dl_traffic_threshold);
        printf("          |--quota_validity_time: %d \r\n", urr->quota_validity_time);
    }
    
    printf("     |--qer_cnt           : %d \r\n", ogs_list_count(&sess->pfcp.qer_list));
    ogs_list_for_each(&sess->pfcp.qer_list, qer){
        printf("          **************qer id: %d***********\r\n",qer->id);     
        printf("          |--gate_status       : uplink:%d downlink:%d\r\n", qer->gate_status.uplink,qer->gate_status.downlink);
        printf("          |--mbr               : uplink:%ld downlink:%ld\r\n", qer->mbr.uplink,qer->mbr.downlink);
        printf("          |--gbr               : uplink:%ld downlink:%ld \r\n", qer->gbr.uplink,qer->gbr.downlink);
        printf("          |--qfi               : %d \r\n", qer->qfi);

    }
    printf("\r\n"); 
    
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