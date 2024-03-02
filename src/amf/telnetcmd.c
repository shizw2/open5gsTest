#include "telnet.h"
#include "context.h"
char                g_chCmdName[128] = {0};
T_pttCmdParas       g_tCmdPara[128];
void amf(void);
void showgnb(uint32_t enbID);
void showgnbBriefAll( void );

void amf(void)
{
    printf("this is amf system. \r\n");
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

    if (strcmp(g_chCmdName, "amf") == 0)
    {
        amf();
    }
    else if (strcmp(g_chCmdName, "showgnb") == 0)
    {
        showgnb(dwPara1);
    }  
    else
    {
        printf("the command not support\r\n");
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