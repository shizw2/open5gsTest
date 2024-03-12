#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void amf(void);
void showgnb(uint32_t enbID);
void showgnbBriefAll( void );
void shownf(uint32_t id);
void shownfBriefAll(void);
void showranue( void );

void showue(char* id);
void showueAll(void);
void showueDetail(char * id);

void amf(void)
{
    printf("this is amf system. \r\n");
}

telnet_command_t commands[] = {
    {"showgnb",     (GenericFunc)showgnb,        1, {INTEGER}},
    {"shownf",      (GenericFunc)shownf,         1, {INTEGER}},
    {"showranue",   (GenericFunc)showranue,      0, {}},
};

telnet_command_t sps_commands[] = {
    {"showue",     (GenericFunc)showue,        1, {STRING}},
};

int numCommands = sizeof(commands) / sizeof(commands[0]);
int spsnumCommands = sizeof(sps_commands) / sizeof(sps_commands[0]);

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

	if (is_amf_icps()){
	    if (strcmp(g_chCmdName, "help") == 0){
	        printf("Supported commands:\n");
            for (int i = 0; i < numCommands; i++) {
                printf("- %s\n", commands[i].command);
            }
	    }else if (strcmp(g_chCmdName, "showgnb") == 0){
	        showgnb(dwPara1);
	    }else if (strcmp(g_chCmdName, "shownf") == 0){
	        shownf(dwPara1);
	    }else if (strcmp(g_chCmdName, "showranue") == 0){
	        showranue();
	    }  
	    else{
	        printf("the command not support\r\n");
	    }
	}else{
		if (strcmp(g_chCmdName, "help") == 0){
	        printf("Supported commands:\n");
            for (int i = 0; i < spsnumCommands; i++) {
                printf("- %s\n", sps_commands[i].command);
            }
	    }else if (strcmp(g_chCmdName, "showue") == 0){
	        showue((char*)g_tCmdPara[0].abCont);
	    }else{
	        printf("the command not support\r\n");
	    }
	}
    
    return;
}

void showgnb(uint32_t enbID)
{
    if(enbID == 0 )
    {
        showgnbBriefAll();
    }
    else
    {
        //showgnbDetail(enbID);
    }

    return;
}

void showgnbBriefAll( void )
{
    amf_gnb_t *gnb = NULL;
    
    printf("\ngnb Brief All(current %u gnb count):\r\n", ogs_list_count(&amf_self()->gnb_list));
    printf("+---------+----------------+----------+----------+--------+\n\r");
    printf("| gnb_id  |    plmn_id     |  state   |  ta_num  | UECnt  |\n\r");
    printf("+---------+----------------+----------+----------+--------+\n\r");
    
    ogs_list_for_each(&amf_self()->gnb_list, gnb) {
        printf("| %-7u | MCC:%dMNC:%-3d | %-8d | %-8d | %-6u |\r\n",
               gnb->gnb_id, 
               ogs_plmn_id_mcc(&gnb->plmn_id),
               ogs_plmn_id_mnc(&gnb->plmn_id),
               gnb->state.ng_setup_success,
               gnb->num_of_supported_ta_list, 
               ogs_list_count(&gnb->ran_ue_list));
    }
    
    printf("+---------+----------------+----------+----------+--------+\n\r");
    
    printf("\r\n");
    
    return ;
}

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
    amf_ue_t *ue = NULL;
    char buffer[20] = {};
    struct tm tm;
	
    printf("\namf ue Brief All(current %u ue count):\r\n", ogs_list_count(&amf_self()->amf_ue_list));
    printf("+----------------------+---------------+---------------------+---------------------------+---------------------+\n\r");
    printf("|         supi         | register_type |          tai        |            cgi --         |      timestamp      |\n\r");
    printf("+----------------------+---------------+---------------------+---------------------------+---------------------+\n\r");
    
    ogs_list_for_each(&amf_self()->amf_ue_list, ue) {
		time_t time = ogs_time_sec(ue->ue_location_timestamp);
		struct tm *timeInfo = localtime(&time);  
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);    
        printf("| %-15s | %-13u | MCC:%dMNC:%-3dTAC:%d | MCC:%dMNC:%-3dCELL:%lu | %s |\r\n",
		   ue->supi,
           ue->nas.registration.value,
           ogs_plmn_id_mcc(&ue->nr_tai.plmn_id),
           ogs_plmn_id_mnc(&ue->nr_tai.plmn_id),
           ue->nr_tai.tac.v,
           ogs_plmn_id_mcc(&ue->nr_cgi.plmn_id),
           ogs_plmn_id_mnc(&ue->nr_cgi.plmn_id),
           ue->nr_cgi.cell_id,
           //ue->ue_ambr.downlink,
           //ue->ue_ambr.uplink,
           buffer);        
    }
    
    printf("+----------------------+---------------+---------------------+---------------------------+---------------------+\n\r");
    printf("\r\n");
    
    return ;
}

void showueDetail( char * supi )
{
    amf_ue_t *ue = NULL;
    char buffer[20] = {};
    struct tm tm;
	int i;

	ue = amf_ue_find_by_supi(supi);
 
	if (ue == NULL){
		printf("cat not find amf ue by supi:%s\r\n",supi);
		return;
	}
    

	time_t time = ogs_time_sec(ue->ue_location_timestamp);
	struct tm *timeInfo = localtime(&time);  
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);


	
	printf("The amf ue(supi=%s) detail info is the following: \r\n", supi);
	printf("  |--registration       : %u \r\n", ue->nas.registration.value);   
	printf("  |--suci               : %s \r\n", ue->suci);
	printf("  |--pei                : %s \r\n", ue->pei);
	printf("  |--imeisv_bcd         : %s \r\n", ue->imeisv_bcd);
	if (ue->current.m_tmsi){
		printf("  |--current            :  \r\n");
		printf("    |--m_tmsi           : %u \r\n", *ue->current.m_tmsi);
	}
	if (ue->next.m_tmsi){
		printf("  |--next               :  \r\n");
		printf("    |--m_tmsi           : %u \r\n", *ue->next.m_tmsi);
	}
	if (ue->guami){
		printf("  |--guami              : \r\n");
		printf("    |--amf_id           : region:%d,set1:%d,set2:%d,pointer:%d\r\n", ue->guami->amf_id.region,ue->guami->amf_id.set1,ue->guami->amf_id.set2,ue->guami->amf_id.pointer);
		printf("    |--plmn_id          : MCC:%d,MNC:%d \r\n", ogs_plmn_id_mcc(&ue->guami->plmn_id),ogs_plmn_id_mnc(&ue->guami->plmn_id));
	}
	printf("  |--nr_tai             : MCC:%d,MNC:%-3dTAC:%d \r\n", ogs_plmn_id_mcc(&ue->nr_tai.plmn_id),
															       ogs_plmn_id_mnc(&ue->nr_tai.plmn_id),
															       ue->nr_tai.tac.v);
	printf("  |--nr_cgi             : MCC:%d,MNC:%-3dCELL:%lu \r\n", ogs_plmn_id_mcc(&ue->nr_cgi.plmn_id),
																       ogs_plmn_id_mnc(&ue->nr_cgi.plmn_id),
																       ue->nr_cgi.cell_id);

	printf("  |--last_visited_plmn_id: MCC:%d,MNC:%-3d \r\n", ogs_plmn_id_mcc(&ue->last_visited_plmn_id),ogs_plmn_id_mnc(&ue->last_visited_plmn_id));
	printf("  |--gmm_capability     :   \r\n");
	printf("    |--lte_positioning_protocol_capability: %d  \r\n", ue->gmm_capability.lte_positioning_protocol_capability);
	printf("    |--ho_attach        : %d \r\n", ue->gmm_capability.ho_attach);
	printf("    |--s1_mode          : %d \r\n", ue->gmm_capability.s1_mode);
	printf("  |--security_context_available: %d \r\n", ue->security_context_available);
	printf("  |--mac_failed         : %d \r\n", ue->mac_failed);
	printf("  |--ue_ambr            : downlink:%lu,uplink:%lu \r\n", ue->ue_ambr.downlink,ue->ue_ambr.uplink);
	printf("  |--num_of_slice       : %d \r\n", ue->num_of_slice);
	printf("  |--slice              : \r\n");
	for (i = 0; i < ue->num_of_slice; i++){
        printf("    |--s_nssai          : SST:%d SD:0x%x \r\n", ue->slice[i].s_nssai.sst,ue->slice[i].s_nssai.sd.v);
        printf("    |--num_of_session 	: %d \r\n", ue->slice[i].num_of_session);
	}
	printf("\r\n");
    
    return ;
}

