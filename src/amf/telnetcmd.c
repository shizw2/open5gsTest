#include "telnet.h"
#include "context.h"
#include "license.h"

char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void setCommands(void);
void amf(void);
void showgnb(uint32_t enbID);
void showgnbBriefAll( void );
void showranue( void );
void showranue_sps( void );
void showicpsue( void );

void showue(char* id);
void showueAll(void);
void showueDetail(char * id);
void showLicenseInfo(void);
void getRanNode(int pageSize ,int pageNum);
void amf(void)
{
    printf("this is amf system. \r\n");
}

telnet_command_t g_commands[] = {
    {"showgnb",     (GenericFunc)showgnb,        1, {INTEGER}},
    {"shownf",      (GenericFunc)shownf,         1, {STRING}},
    {"showranue",   (GenericFunc)showranue,      0, {}},
    {"showicpsue",   (GenericFunc)showicpsue,      0, {}},
    {"showLicenseInfo", (GenericFunc)showLicenseInfo,      0, {}},
    {"getRanNode",   (GenericFunc)getRanNode,      2, {INTEGER,INTEGER}},
};
int g_numCommands = sizeof(g_commands) / sizeof(g_commands[0]);

telnet_command_t g_sps_commands[] = {
    {"shownf",      (GenericFunc)shownf,         1, {STRING}},
    {"showranue",   (GenericFunc)showranue,      0, {}},
    {"showue",      (GenericFunc)showue,         1, {STRING}},
};
int g_spsnumCommands = sizeof(g_sps_commands) / sizeof(g_sps_commands[0]);

void setCommands(void){
    telnet_command_t *commands = NULL;
    int numCommands = 0;
    if (is_amf_icps()){
        commands = g_commands;
        numCommands = g_numCommands;
    }else{
        commands = g_sps_commands;
        numCommands = g_spsnumCommands;
    }
    set_telnet_commands(commands, numCommands);
}


void showgnb(uint32_t enbID)
{
    printf("enbID:%d\r\n",enbID);
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
    char buf[OGS_PLMNIDSTRLEN];
    printf("\ngnb Brief All(current %u gnb count):\r\n", ogs_list_count(&amf_self()->gnb_list));
    printf("+---------+---------+----------+----------+--------+\n\r");
    printf("| gnb_id  | plmn_id |  state   |  ta_num  | UECnt  |\n\r");
    printf("+---------+---------+----------+----------+--------+\n\r");
    
    ogs_list_for_each(&amf_self()->gnb_list, gnb) {
        printf("| %-7u | %-7s | %-8d | %-8d | %-6u |\r\n",
               gnb->gnb_id, 
               ogs_plmn_id_to_string(&gnb->plmn_id,buf),
               gnb->state.ng_setup_success,
               gnb->num_of_supported_ta_list, 
               ogs_list_count(&gnb->ran_ue_list));
    }
    
    printf("+---------+---------+----------+----------+--------+\n\r");
    
    printf("\r\n");
    
    return ;
}

void showranue( void )
{
    if (is_amf_icps()){

        amf_gnb_t *gnb = NULL;
        ran_ue_t *ran_ue = NULL;
        
        printf("\ngnb Brief All(current %u gnb count):\r\n", ogs_list_count(&amf_self()->gnb_list));
        printf("+---------+----------------+----------------+--------+----------------------+------------+\n\r");
        printf("| gnb_id  | ran_ue_ngap_id | amf_ue_ngap_id | sps_no |         supi         |    tmsi    |\n\r");
        printf("+---------+----------------+----------------+--------+----------------------+------------+\n\r");
        
        ogs_list_for_each(&amf_self()->gnb_list, gnb) {
            ogs_list_for_each(&gnb->ran_ue_list, ran_ue) {
                printf("| %-7u | %-14lu | %-14lu | %-6d | %-15s | %-10u |\r\n",
                   gnb->gnb_id, 
                   ran_ue->ran_ue_ngap_id,
                   ran_ue->amf_ue_ngap_id,
                   ran_ue->sps_no,
                   ran_ue->supi,
                   ran_ue->m_tmsi);
            }
        }
        
        printf("+---------+----------------+----------------+--------+----------------------+------------+\n\r");
       
        printf("\r\n");
        }else{
        showranue_sps();
    }
    return ;
}
void showranue_sps( void )
{
    
    ran_ue_t *ran_ue = NULL;
    amf_ue_t *ue = NULL;    
    
    printf("+-----------+----------------+----------------+-----------+----------------------+------------+\n\r");
    printf("|     id    | ran_ue_ngap_id | amf_ue_ngap_id | amf_ue_id | amf_ue_ngap_id_icps  |    tmsi    |\n\r");
    printf("+-----------+----------------+----------------+-----------+----------------------+------------+\n\r");
    
    
    ogs_list_for_each(&amf_self()->amf_ue_list, ue) {
        ran_ue = ran_ue_find_by_id(ue->ran_ue_id);
        if(ran_ue){
            if (ran_ue->amf_ue_ngap_id_icps) {
                printf("| %-9d | %-14lu | %-14lu | %-9d | %-20lu | %-10u |\r\n",
                   ran_ue->id, 
                   ran_ue->ran_ue_ngap_id,
                   ran_ue->amf_ue_ngap_id,
                   ran_ue->amf_ue_id,
                   *(ran_ue->amf_ue_ngap_id_icps),
                   ran_ue->m_tmsi);
                } else {
                printf("| %-9d | %-14lu | %-14lu | %-9d | %-20s | %-10u |\r\n",
                   ran_ue->id, 
                   ran_ue->ran_ue_ngap_id,
                   ran_ue->amf_ue_ngap_id,
                   ran_ue->amf_ue_id,
                   "NULL",
                   ran_ue->m_tmsi);
                }
           }
    }
    
    
    printf("+-----------+----------------+----------------+-----------+----------------------+------------+\n\r");
   
    printf("\r\n");
    
    return ;
}
void showicpsue( void )
{

    if (!is_amf_icps())return;    
    icps_ue_spsno_t *icps_ue = NULL,*next = NULL;
    
    printf("\nicps ue Brief All(current %u ue count):\r\n", ogs_list_count(&amf_self()->icps_ue_list));
    
    printf("+-----------+----------------------+----------------+\n\r");
    printf("|     id    | supi                 | sps_no         |\n\r");
    printf("+-----------+----------------------+----------------+\n\r");
    
    ogs_list_for_each_safe(&amf_self()->icps_ue_list, next, icps_ue) {       
       
         if(icps_ue){
            if (icps_ue->supi) {
                printf("| %-9d | %-20s | %-14d |\r\n",
                   icps_ue->id, 
                   icps_ue->supi,
                   icps_ue->sps_id);
                } else {
                printf("| %-9d | %-20s | %-14d |\r\n",
                   icps_ue->id,
                   "NULL",
                   icps_ue->sps_id);
                }
        }
    }
    
    
    printf("+-----------+----------------------+----------------+\n\r");
   
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
	char buf1[OGS_PLMNIDSTRLEN];
    char buf2[OGS_PLMNIDSTRLEN];
    printf("\namf ue Brief All(current %u ue count):\r\n", ogs_list_count(&amf_self()->amf_ue_list));
    printf("+----------------------+---------------+----------------------+---------------------------+----------------+---------------------+\n\r");
    printf("|         supi         | register_type |          tai         |            cgi            |CM_CONNECTED(ue)|      timestamp      |\n\r");
    printf("+----------------------+---------------+----------------------+---------------------------+----------------+---------------------+\n\r");
    
    ogs_list_for_each(&amf_self()->amf_ue_list, ue) {
		time_t time = ogs_time_sec(ue->ue_location_timestamp);
		struct tm *timeInfo = localtime(&time);  
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);    
        printf("| %-15s | %-13u | plmn_id:%s,tac:%-2d | plmn_id:%s,cell:%-6lu | %15d| %s |\r\n",
		   ue->supi,
           ue->nas.registration.value,
           ogs_plmn_id_to_string(&ue->nr_tai.plmn_id,buf1),
           ue->nr_tai.tac.v,
           ogs_plmn_id_to_string(&ue->nr_cgi.plmn_id,buf2),
           ue->nr_cgi.cell_id,
           CM_CONNECTED(ue),
           //ue->ue_ambr.downlink,
           //ue->ue_ambr.uplink,
           buffer);        
    }
    
    printf("+----------------------+---------------+----------------------+---------------------------+----------------+---------------------+\n\r");
    printf("\r\n");
    
    return ;
}

void showueDetail( char * supi )
{
    amf_ue_t *ue = NULL;
    char buffer[20] = {};
    struct tm tm;
	int i;
	char buf1[OGS_PLMNIDSTRLEN];
    
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
    printf("  |--num_of_msisdn      : %d \r\n", ue->num_of_msisdn);
    for (i = 0; i < ue->num_of_msisdn; i++){
        printf("    |--msisdn         : %s \r\n", ue->msisdn[i]);
    }

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
		printf("    |--plmn_id          : %s \r\n", ogs_plmn_id_to_string(&ue->guami->plmn_id,buf1));
	}
	printf("  |--nr_tai             : plmn_id:%s,TAC:%d \r\n", ogs_plmn_id_to_string(&ue->nr_tai.plmn_id,buf1),
															       ue->nr_tai.tac.v);
	printf("  |--nr_cgi             : plmn_id:%s,CELL:%lu \r\n", ogs_plmn_id_to_string(&ue->nr_cgi.plmn_id,buf1),
																       ue->nr_cgi.cell_id);

	printf("  |--last_visited_plmn_id: %s \r\n", ogs_plmn_id_to_string(&ue->last_visited_plmn_id,buf1));
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
    printf("  |--CM_status     :   \r\n");
	printf("    |--CM_CONNECTED(amf_ue): %d  \r\n", CM_CONNECTED(ue));
	printf("    |--CM_IDLE(amf_ue)     : %d  \r\n", CM_IDLE(ue));
	
	printf("\r\n");
    
    return ;
}

// void showLicenseInfo(void){
//     char errorMsg[100];
//     size_t errorMsgSize = sizeof(errorMsg);
//     bool result = dsCheckLicense(errorMsg, errorMsgSize);
//     if (!result) {        
//         printf("error: %s\n", errorMsg);
//         return ;
//     }
// /*      "licBaseStart": "2024-09-19",
//         "licBaseExpire": "2025-09-19",
//         "licBaseDays": "366",
//         "licBaseValid": "valid",
//         "licBaseCreate": "2024-09-19 14:51:17",
//         "licBaseType": "debug",
//         "licBaseToday": "2024-09-29",
//         "licBaseCustomer": "XXX",
//         "licBaseSerialno": "20240919T145117cf230762ac7b85270434b461b5e3b5",
//         "maximumRanNodes": 3,
//         "maximumSubscriptions": 10000,
//         "maximumRegistrations": 8*/
//     printf("licBaseStart:%s,licBaseExpire:%s,licBaseDays:%lu,licBaseValid:%s,licBaseCreate:%s,licBaseType:%s,
// licBaseToday:%s,licBaseCustomer:%s,licBaseSerialno:%s,maximumRanNodes:%d,maximumSubscriptions:%d,maximumRegistrations:%d\r\n", 
//                         timestampToStringDate(getLicenseCreateTime()),
//                         timestampToStringDate(getLicenseExpireTime()),
//                         getLicenseDurationTime()/86400,
//                         result?"valid":"invalid", 
//                         timestampToString(getLicenseCreateTime()), 
//                         "debug",                 
//                         timestampToString(getLicenseCreateTime()), 
//                         getLicenseCustomer(),
//                         getLicenseSerialno(),
//                         getLicenseRanNodes(),
//                         getLicenseSubscriptions(),
//                         getLicenseUeNum());
// }

void showLicenseInfo(void) {
    char errorMsg[100];
    size_t errorMsgSize = sizeof(errorMsg);
    time_t currentTime;
    time(&currentTime);

    bool result = dsCheckLicense(errorMsg, errorMsgSize);

    cJSON *root = cJSON_CreateObject();

    // 添加基础许可证信息
    cJSON_AddStringToObject(root, "licBaseStart", timestampToStringDate(getLicenseCreateTime()));
    cJSON_AddStringToObject(root, "licBaseExpire", timestampToStringDate(getLicenseExpireTime()));
    cJSON_AddNumberToObject(root, "licBaseDays", getLicenseDurationTime() / 86400);
    cJSON_AddStringToObject(root, "licBaseValid", result ? "valid" : "invalid");
    cJSON_AddStringToObject(root, "licBaseCreate", timestampToString(getLicenseCreateTime()));
    cJSON_AddStringToObject(root, "licBaseType", "debug");
    cJSON_AddStringToObject(root, "licBaseToday", timestampToString(currentTime));
    cJSON_AddStringToObject(root, "licBaseCustomer", getLicenseCustomer());
    cJSON_AddStringToObject(root, "licBaseSerialno", getLicenseSerialno());

    // 添加最大节点数、订阅数和注册数
    cJSON_AddNumberToObject(root, "maximumRanNodes", getLicenseRanNodes());
    cJSON_AddNumberToObject(root, "maximumSubscriptions", getLicenseSubscriptions());
    cJSON_AddNumberToObject(root, "maximumRegistrations", getLicenseUeNum());

    // 将cJSON对象转换为字符串并打印
    char *json_string = cJSON_Print(root);
    printf("%s\n", json_string);

    // 释放cJSON对象
    cJSON_Delete(root);
    ogs_free(json_string);
}

void getRanNode(int pageSize, int pageNum) {
    amf_gnb_t *gnb = NULL;
    char buf[OGS_PLMNIDSTRLEN];
    int totalRanNodeNum = 0;
    int startIdx = (pageNum == 0 || pageSize == 0) ? 0 : (pageNum - 1) * pageSize;
    int count = 0;

    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        ogs_error("cJSON_CreateObject failed");
        return;
    }

    cJSON *ranNodeList = cJSON_CreateArray();
    if (ranNodeList == NULL) {
        ogs_error("cJSON_CreateArray failed");
        cJSON_Delete(root);
        return;
    }

    ogs_list_for_each(&amf_self()->gnb_list, gnb) {
        if (pageNum == 0 || pageSize == 0 || (count >= startIdx && count < startIdx + pageSize)) {
            cJSON *ranNode = cJSON_CreateObject();
            if (ranNode == NULL) {
                ogs_error("cJSON_CreateObject failed");
                cJSON_Delete(ranNodeList);
                cJSON_Delete(root);
                return;
            }

            cJSON_AddStringToObject(ranNode, "connTime", timestampToString(gnb->createTime));
            cJSON_AddNumberToObject(ranNode, "dura", 10); // TODO: 是啥?
            cJSON_AddStringToObject(ranNode, "ipAddr", OGS_ADDR(gnb->sctp.addr, buf));
            cJSON_AddStringToObject(ranNode, "name", gnb->ran_node_name);

            char nssai[OGS_MAX_NUM_OF_SLICE_SUPPORT] = "";
            int i;
            for (i = 0; i < gnb->supported_ta_list[0].bplmn_list[0].num_of_s_nssai; i++) {
                if (i > 0) {
                    strcat(nssai, ",");  // 在第一个NSSAI之后添加逗号
                }
                char s_nssai[8];  // 用于存储单个NSSAI的临时字符串
                sprintf(s_nssai, "%d-%x", gnb->supported_ta_list[0].bplmn_list[0].s_nssai[i].sst, gnb->supported_ta_list[0].bplmn_list[0].s_nssai[i].sd.v);
                strcat(nssai, s_nssai);  // 将格式化的NSSAI追加到主字符串中
            }
            cJSON_AddStringToObject(ranNode, "nssai", nssai);

            cJSON_AddStringToObject(ranNode, "plmnId", ogs_plmn_id_to_string(&gnb->plmn_id, buf));
            cJSON_AddNumberToObject(ranNode, "ranNodeId", gnb->gnb_id);
            cJSON_AddStringToObject(ranNode, "ranNodeIdType", "GnbId"); // TODO:不知道是啥
            cJSON_AddItemToArray(ranNodeList, ranNode);
        }
        count++;
        totalRanNodeNum++;
    }

    cJSON_AddStringToObject(root, "amfName", amf_self()->amf_name);
    cJSON_AddItemToObject(root, "ranNodeList", ranNodeList);
    cJSON_AddNumberToObject(root, "totalRanNodeNum", totalRanNodeNum);

    char *json_string = cJSON_Print(root);
    printf("%s\n", json_string);

    // 释放cJSON对象
    cJSON_Delete(root);
    ogs_free(json_string);
}