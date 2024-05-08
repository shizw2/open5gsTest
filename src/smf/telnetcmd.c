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

void print_pdr(ogs_pfcp_pdr_t *pdr);
void print_far(ogs_pfcp_far_t *far);
void print_bearer(smf_bearer_t *bearer);

telnet_command_t g_commands[] = {
    {"shownf",      (GenericFunc)shownf,         1, {STRING}},
    {"showue",      (GenericFunc)showue,         1, {STRING}},
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
		printf("| %-20s | %-20s | %-20s | %-20s | %-11d |\r\n",
		   ue->supi,
           ue->imsi_bcd,
           ue->msisdn_bcd,
           ue->imeisv_bcd,
           ogs_list_count(&ue->sess_list));        
    }
    
    printf("+----------------------+----------------------+----------------------+----------------------+-------------+\n\r");
    printf("\r\n");
    
    return ;
}

void showueDetail( char * supi )
{
    smf_ue_t   *ue = NULL;
    smf_sess_t *sess = NULL;
    smf_bearer_t *bearer = NULL;
    ogs_pfcp_pdr_t *pdr = NULL;
    ogs_pfcp_far_t *far = NULL;
    char buf[OGS_ADDRSTRLEN];
    char addrInfo[OGS_ADDRSTRLEN] = {0};
    char buf1[OGS_ADDRSTRLEN];
    char buf2[OGS_ADDRSTRLEN];
    char buffer[20] = {};
    ue = smf_ue_find_by_supi(supi);
   
    if (NULL == ue){
        printf("can't find ue by supi:%s\r\n",supi);
        return;
    }

    ogs_list_for_each(&ue->sess_list, sess){
        printf("The ue %s smf_sess %d Detail Info is the following: \r\n",supi,sess->index);
        //printf("  |--sbi                : %p \r\n", sess->sbi);
        printf("  |--index              : %u \r\n", sess->index);
        printf("  |--smf_n4_seid_node   : %d \r\n", *sess->smf_n4_seid_node);
        printf("  |--sm_data            :  \r\n");
        printf("     |--gx_ccr_init_in_flight   : %d \r\n", sess->sm_data.gx_ccr_init_in_flight);
        printf("     |--gx_cca_init_err         : %u \r\n", sess->sm_data.gx_cca_init_err);
        printf("     |--gy_ccr_init_in_flight   : %d \r\n", sess->sm_data.gy_ccr_init_in_flight);
        printf("     |--gy_cca_init_err         : %u \r\n", sess->sm_data.gy_cca_init_err);
        printf("     |--gx_ccr_term_in_flight   : %d \r\n", sess->sm_data.gx_ccr_term_in_flight);
        printf("     |--gx_cca_term_err         : %u \r\n", sess->sm_data.gx_cca_term_err);
        printf("     |--gy_ccr_term_in_flight   : %d \r\n", sess->sm_data.gy_ccr_term_in_flight);
        printf("     |--gy_cca_term_err         : %u \r\n", sess->sm_data.gy_cca_term_err);
        printf("     |--s6b_str_in_flight       : %d \r\n", sess->sm_data.s6b_str_in_flight);
        printf("     |--s6b_sta_err             : %u \r\n", sess->sm_data.s6b_sta_err);
        printf("  |--epc                : %d \r\n", sess->epc);
        //printf("  |--pfcp               : %p \r\n", sess->pfcp);
        printf("  |--smpolicycontrol_features   : %lu \r\n", sess->smpolicycontrol_features);
        printf("  |--smf_n4_teid        : %u \r\n", sess->smf_n4_teid);
        printf("  |--sgw_s5c_teid       : %u \r\n", sess->sgw_s5c_teid);
        printf("  |--sgw_s5c_ip         : %s \r\n", ogs_ipv4_to_string(sess->sgw_s5c_ip.addr));
        printf("  |--smf_n4_seid        : %lu \r\n", sess->smf_n4_seid);
        printf("  |--upf_n4_seid        : %lu \r\n", sess->upf_n4_seid);
        printf("  |--upf_n3_teid        : %u \r\n", sess->upf_n3_teid);
        if (sess->upf_n3_addr != NULL){
            sprintf(addrInfo,"%s:%d",OGS_ADDR(sess->upf_n3_addr, buf), OGS_PORT(sess->upf_n3_addr));
            printf("  |--upf_n3_addr        : %s \r\n", addrInfo);
        }
        if (sess->upf_n3_addr6 != NULL){
            sprintf(addrInfo,"%s:%d",OGS_ADDR(sess->upf_n3_addr6, buf), OGS_PORT(sess->upf_n3_addr6));
            printf("  |--upf_n3_addr6       : %s \r\n", addrInfo);
        }
        printf("  |--gnb_n3_teid        : %u \r\n", sess->gnb_n3_teid);
        printf("  |--gnb_n3_ip          : %s \r\n", ogs_ipv4_to_string(sess->gnb_n3_ip.addr));
        printf("  |--gx_sid             : %s \r\n", sess->gx_sid);
        printf("  |--gy_sid             : %s \r\n", sess->gy_sid);
        printf("  |--s6b_sid            : %s \r\n", sess->s6b_sid);
        //printf("  |--qfi_pool           : %p \r\n", sess->qfi_pool);
        printf("  |--sm_context_ref     : %s \r\n", sess->sm_context_ref);
        printf("  |--psi                : %u \r\n", sess->psi);
        printf("  |--pti                : %u \r\n", sess->pti);
        printf("  |--sm_context_status_uri      : %s \r\n", sess->sm_context_status_uri);
        //printf("  |--namf.client        : %p \r\n", sess->namf.client);
        printf("  |--policy_association_id      : %s \r\n", sess->policy_association_id);
        printf("  |--up_cnx_state       : %d \r\n", sess->up_cnx_state);
        printf("  |--plmn_id            : %s \r\n", ogs_plmn_id_to_string(&sess->plmn_id,buf));
        printf("  |--nr_tai             : MCC:%d,MNC:%-3dTAC:%d \r\n", ogs_plmn_id_mcc(&sess->nr_tai.plmn_id),
                                                                   ogs_plmn_id_mnc(&sess->nr_tai.plmn_id),
                                                                   sess->nr_tai.tac.v);
        printf("  |--nr_cgi             : MCC:%d,MNC:%-3dCELL:%lu \r\n", ogs_plmn_id_mcc(&sess->nr_cgi.plmn_id),
                                                                           ogs_plmn_id_mnc(&sess->nr_cgi.plmn_id),
                                                                           sess->nr_cgi.cell_id);
        time_t time = ogs_time_sec(sess->ue_location_timestamp);
        struct tm *timeInfo = localtime(&time);  
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);    
        printf("  |--ue_location_timestamp      : %s \r\n", buffer);

        printf("  |--pcf_id             : %s \r\n", sess->pcf_id);
        printf("  |--serving_nf_id      : %s \r\n", sess->serving_nf_id);

        printf("  |--integrity_protection.mbr_dl      : %u \r\n", sess->integrity_protection.mbr_dl);
        printf("  |--integrity_protection.mbr_ul      : %u \r\n", sess->integrity_protection.mbr_ul);

        printf("  |--s_nssai            : SST:%d SD:0x%x \r\n", sess->s_nssai.sst,sess->s_nssai.sd.v);
        printf("  |--mapped_hplmn       : SST:%d SD:0x%x \r\n", sess->mapped_hplmn.sst,sess->mapped_hplmn.sd.v);


        //printf("  |--session.ue_ip      : %s \r\n", ogs_session_ue_ip_to_string(sess->session.ue_ip));
        //printf("  |--session.ambr.mbr_dl       : %u \r\n", sess->session.ambr.mbr_dl);
        //printf("  |--session.ambr.mbr_ul       : %u \r\n", sess->session.ambr.mbr_ul);

        printf("  |--ue_session_type    : %u \r\n", sess->ue_session_type);
        printf("  |--ue_ssc_mode        : %u \r\n", sess->ue_ssc_mode);

        if (sess->ipv4 != NULL)
            printf("  |--ipv4               : %s \r\n", OGS_INET_NTOP(&sess->ipv4->addr, buf1));
        
        if (sess->ipv6 != NULL)
            printf("  |--ipv6               : %s \r\n", OGS_INET6_NTOP(&sess->ipv6->addr, buf2));

        printf("  |--gtp_rat_type       : %u \r\n", sess->gtp_rat_type);
        printf("  |--sbi_rat_type       : %u \r\n", sess->sbi_rat_type);

        printf("  |--gtp.version        : %u \r\n", sess->gtp.version);
        printf("  |--gtp.create_session_response_apn_ambr: %d \r\n", sess->gtp.create_session_response_apn_ambr);
        printf("  |--gtp.create_session_response_bearer_qos: %d \r\n", sess->gtp.create_session_response_bearer_qos);
       
        //printf("  |--nas.ue_epco               : %p \r\n", sess->nas.ue_epco);
        //printf("  |--policy.pcc_rule           : %p \r\n", sess->policy.pcc_rule);
        printf("  |--policy.num_of_pcc_rule    : %d \r\n", sess->policy.num_of_pcc_rule);
        printf("  |--paging.n1n2message_location : %s \r\n", sess->paging.n1n2message_location);
        printf("  |--ngap_state.pdu_session_resource_release : %d \r\n", sess->ngap_state.pdu_session_resource_release);
        //handover
        
        printf("  |--charging.id        : %u \r\n", sess->charging.id);
        printf("  |--cp2up_pdr          :  \r\n");
        print_pdr(sess->cp2up_pdr);
        printf("  |--up2cp_pdr          :  \r\n");
        print_pdr(sess->up2cp_pdr);
        printf("  |--cp2up_far          :  \r\n");
        print_far(sess->cp2up_far);
        printf("  |--up2cp_far          :  \r\n");
        print_far(sess->up2cp_far);
        printf("  |--bearer_num         :  %d\r\n", ogs_list_count(&sess->bearer_list));
        int bearer_idx = 0;
        ogs_list_for_each(&sess->bearer_list, bearer){
            printf("  |--bearer_info %d     :  \r\n", bearer_idx);
            print_bearer(bearer);
            bearer_idx++;
        }
        //printf("  |--pdr_to_modify_list        : %p \r\n", sess->pdr_to_modify_list);
        //printf("  |--qos_flow_to_modify_list   : %p \r\n", sess->qos_flow_to_modify_list);
        //printf("  |--gnode                     : %p \r\n", sess->gnode);
        //printf("  |--pfcp_node                 : %p \r\n", sess->pfcp_node);
        printf("  |--n1_released        : %d \r\n", sess->n1_released);
        printf("  |--n2_released        : %d \r\n", sess->n2_released);
    }
}

void print_pdr(ogs_pfcp_pdr_t *pdr){
    char buf1[OGS_ADDRSTRLEN];
    char buf2[OGS_ADDRSTRLEN];
    ogs_pfcp_rule_t *rule = NULL;
    int i;
    
    if (NULL == pdr){
        return;
    }
    
    printf("          |--teid              : %d \r\n", pdr->teid);
    printf("          |--src_if            : %s \r\n", ogs_pfcp_interface_get_name(pdr->src_if));
    printf("          |--dnn               : %s \r\n", pdr->dnn);
    printf("          |--f_teid            :  \r\n");
    printf("              |--teid          : %d \r\n", pdr->f_teid.teid);
    printf("              |--addr          : %s \r\n", OGS_INET_NTOP(&pdr->f_teid.addr, buf1) );
    printf("              |--addr6         : %s \r\n", OGS_INET_NTOP(&pdr->f_teid.addr6, buf2) );
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

        printf("              |--meas_info         : mnop:%d,istm:%d,radi:%d,inam:%d,mnop:%d \r\n", pdr->urr[i]->meas_info.mnop,
                                                                                                    pdr->urr[i]->meas_info.istm,
                                                                                                    pdr->urr[i]->meas_info.radi,
                                                                                                    pdr->urr[i]->meas_info.inam,
                                                                                                    pdr->urr[i]->meas_info.mbqe);
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
        printf("              |--dropped_dl_traffic_threshold: downlink_packets:%lu,number_of_bytes_of_downlink_data:%lu \r\n", pdr->urr[i]->dropped_dl_traffic_threshold.downlink_packets,pdr->urr[i]->dropped_dl_traffic_threshold.number_of_bytes_of_downlink_data);
        printf("              |--quota_validity_time: %d \r\n", pdr->urr[i]->quota_validity_time);
    }
    printf("          |--num_of_flow       : %d \r\n", pdr->num_of_flow);
    for (i = 0; i < pdr->num_of_flow; i++){
        printf("              |--flow_description[%d] : %s \r\n", i,pdr->flow_description[i]);
    }
    printf("          |--num of rule_list  : %d \r\n", ogs_list_count(&pdr->rule_list));
    ogs_list_for_each(&pdr->rule_list, rule){
        printf("              |--sdf_filter_id : %d \r\n", rule->sdf_filter_id);
        printf("              |--flags         : %d(bid:%d,fl:%d,spi:%d,ttc:%d,fd:%d) \r\n", rule->flags,rule->bid,rule->fl,rule->spi,rule->ttc,rule->fd);
        printf("              |--proto         : %d \r\n", rule->ipfw.proto);
        printf("              |--ipv4_src      : %d \r\n", rule->ipfw.ipv4_src);
        printf("              |--ipv4_dst      : %d \r\n", rule->ipfw.ipv4_dst);
        printf("              |--ipv6_src      : %d \r\n", rule->ipfw.ipv6_src);
        printf("              |--ipv6_dst      : %d \r\n", rule->ipfw.ipv6_dst);
        printf("              |--port          :SRC:%d-%d DST:%d-%d\r\n",
                                                    rule->ipfw.port.src.low,
                                                    rule->ipfw.port.src.high,
                                                    rule->ipfw.port.dst.low,
                                                    rule->ipfw.port.dst.high);
        printf("              |--SRC           :%08x %08x %08x %08x/%08x %08x %08x %08x\r\n",
                                                    be32toh(rule->ipfw.ip.src.addr[0]),
                                                    be32toh(rule->ipfw.ip.src.addr[1]),
                                                    be32toh(rule->ipfw.ip.src.addr[2]),
                                                    be32toh(rule->ipfw.ip.src.addr[3]),
                                                    be32toh(rule->ipfw.ip.src.mask[0]),
                                                    be32toh(rule->ipfw.ip.src.mask[1]),
                                                    be32toh(rule->ipfw.ip.src.mask[2]),
                                                    be32toh(rule->ipfw.ip.src.mask[3]));
        printf("              |--DST           :%08x %08x %08x %08x/%08x %08x %08x %08x\r\n",
                                                    be32toh(rule->ipfw.ip.dst.addr[0]),
                                                    be32toh(rule->ipfw.ip.dst.addr[1]),
                                                    be32toh(rule->ipfw.ip.dst.addr[2]),
                                                    be32toh(rule->ipfw.ip.dst.addr[3]),
                                                    be32toh(rule->ipfw.ip.dst.mask[0]),
                                                    be32toh(rule->ipfw.ip.dst.mask[1]),
                                                    be32toh(rule->ipfw.ip.dst.mask[2]),
                                                    be32toh(rule->ipfw.ip.dst.mask[3]));
    }
}

void print_far(ogs_pfcp_far_t *far){
    if (NULL == far){
        return;
    }    
    printf("          |--dnn               : %s \r\n", far->dnn);
    printf("          |--dst_if            : %s \r\n", ogs_pfcp_interface_get_name(far->dst_if));
    printf("          |--apply_action      : %d \r\n", far->apply_action);
    printf("          |--num_of_buffered_packet: %d \r\n", far->num_of_buffered_packet);        
    printf("          |--smreq_flags       : %d \r\n", far->smreq_flags.value);
}

void print_bearer(smf_bearer_t *bearer){
    smf_pf_t *pf = NULL;
    if (NULL == bearer){
        return;
    }       
    
    //printf("  |--to_modify_node     : %p \r\n", bearer->to_modify_node);
    //printf("  |--to_delete_node     : %p \r\n", bearer->to_delete_node);
    printf("    |--dl_pdr             :  \r\n");
    print_pdr(bearer->dl_pdr);
    printf("    |--ul_pdr             :  \r\n");
    print_pdr(bearer->ul_pdr);
    printf("    |--dl_far             :  \r\n");
    print_far(bearer->dl_far);
    printf("    |--ul_far             :  \r\n");
    print_far(bearer->ul_far);
    //printf("  |--urr                : %p \r\n", bearer->urr);
    //printf("  |--qer                : %p \r\n", bearer->qer);
    //printf("  |--qfi_node           : %d \r\n", *bearer->qfi_node);
    printf("    |--qfi                : %d \r\n", bearer->qfi);
    printf("    |--ebi                : %d \r\n", bearer->ebi);
    //printf("  |--pgw_s5u_teid       : %u \r\n", bearer->pgw_s5u_teid);
    //printf("  |--pgw_s5u_addr       : %p \r\n", bearer->pgw_s5u_addr);
    //printf("  |--pgw_s5u_addr6      : %p \r\n", bearer->pgw_s5u_addr6);
    //printf("  |--sgw_s5u_teid       : %u \r\n", bearer->sgw_s5u_teid);
    //printf("  |--sgw_s5u_ip         : %s \r\n", ogs_ip_to_string(bearer->sgw_s5u_ip));
    printf("    |--pcc_rule.name      : %s \r\n", bearer->pcc_rule.name);
    printf("    |--pcc_rule.id        : %s \r\n", bearer->pcc_rule.id);
    printf("    |--qos                : \r\n");
    printf("      |--mbr.uplink       : %lu \r\n", bearer->qos.mbr.uplink);
    printf("      |--mbr.downlink     : %lu \r\n", bearer->qos.mbr.downlink);
    printf("      |--gbr.uplink       : %lu \r\n", bearer->qos.gbr.uplink);
    printf("      |--gbr.downlink     : %lu \r\n", bearer->qos.gbr.downlink);
    printf("      |--index            : %u \r\n", bearer->qos.index);
    printf("      |--arp.priority_level: %u \r\n", bearer->qos.arp.priority_level);
    printf("      |--arp.pre_emption_capability: %u \r\n", bearer->qos.arp.pre_emption_capability);
    printf("      |--arp.pre_emption_vulnerability: %u \r\n", bearer->qos.arp.pre_emption_vulnerability);

    //printf("  |--pf_identifier_pool : %p \r\n", bearer->pf_identifier_pool);
    printf("    |--pf_list num        : %u \r\n", ogs_list_count(&bearer->pf_list));
    int pf_index = 0;
    ogs_list_for_each(&bearer->pf_list, pf){
        printf("    |--pf info %d       :  \r\n", pf_index);
        printf("      |--direction        : %u \r\n", pf->direction);
        printf("      |--identifier       : %u \r\n", pf->identifier);
        printf("      |--flow_description : %s \r\n", pf->flow_description);
        pf_index++;
    }
    printf("    |--pf_to_add_list num : %u \r\n", ogs_list_count(&bearer->pf_to_add_list));
    pf_index = 0;
    ogs_list_for_each(&bearer->pf_to_add_list, pf){
        printf("    |--pf info %d       :  \r\n", pf_index);
        printf("      |--direction        : %u \r\n", pf->direction);
        printf("      |--identifier       : %u \r\n", pf->identifier);
        printf("      |--flow_description : %s \r\n", pf->flow_description);
        pf_index++;
    }
    printf("    |--num_of_pf_to_delete: %u \r\n", bearer->num_of_pf_to_delete);
    //printf("  |--pf_to_delete       : %p \r\n", bearer->pf_to_delete);
    //printf("  |--sess               : %p \r\n", bearer->sess);

}