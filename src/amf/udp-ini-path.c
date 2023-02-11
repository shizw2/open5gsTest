#include "udp-ini-path.h"

extern int g_sps_id;

pkt_fwd_tbl_t g_pkt_fwd_tbl;//保存激活的模块信息,
pkt_fwd_tbl_t *g_pt_pkt_fwd_tbl  = &g_pkt_fwd_tbl;

int udp_ini_open(void)
{
    ogs_socknode_t *node = NULL;    
    ogs_sock_t *udp;
    char buf[OGS_ADDRSTRLEN];


    int rv,i;
	ogs_sockaddr_t  *internel_addr;
	ogs_sockaddr_t  *icps_addr;

	if (is_amf_icps())
	{
		rv = ogs_getaddrinfo(&internel_addr, AF_INET, "128.128.128.127", amf_self()->icps_port, 0);
		ogs_assert(rv == OGS_OK);

		amf_self()->udp_node = ogs_socknode_new(internel_addr);
		ogs_assert(amf_self()->udp_node);
		
		/*设置sps的信息,用于发送消息*/
		for (i = 1; i <= amf_self()->spsnum && i <= MAX_SPS_NUM; i++)
		{
			char ipstring[20] = {0};
			ogs_snprintf(ipstring, sizeof(ipstring), "128.128.128.%u", i);

			rv = ogs_getaddrinfo(&internel_addr, AF_INET, ipstring, amf_self()->icps_port, 0);
			ogs_assert(rv == OGS_OK);

			amf_self()->sps_nodes[i] = ogs_socknode_new(internel_addr);
			ogs_assert(amf_self()->sps_nodes[i]);
		}
	}
	else/*sps*/
	{
		char ipstring[20] = {0};
		ogs_snprintf(ipstring, sizeof(ipstring), "128.128.128.%u", g_sps_id);

		rv = ogs_getaddrinfo(&internel_addr, AF_INET, ipstring, amf_self()->icps_port, 0);
		ogs_assert(rv == OGS_OK);

		amf_self()->udp_node = ogs_socknode_new(internel_addr);
		ogs_assert(amf_self()->udp_node);
	
	    /*设置icps的信息,用于发送消息*/
		rv = ogs_getaddrinfo(&icps_addr, AF_INET, "128.128.128.127", amf_self()->icps_port, 0);
		ogs_assert(rv == OGS_OK);

		amf_self()->icps_node = ogs_socknode_new(icps_addr);
		ogs_assert(amf_self()->icps_node);
	}



    /*ogs_ipv4_from_string(&internel_ipv4.addr,"128.128.128.127");
	internel_ipv4.len = OGS_IPV4_LEN;
    internel_ipv4.ipv4 =1;
    ogs_ip_to_sockaddr(&internel_ipv4,self.icps_port,&icps_addr);
    self.icps_node = ogs_socknode_new(icps_addr);*/

#if 0
    ogs_list_for_each(&amf_self()->icps_list, node) {
        udp = ogs_udp_server(node->addr, node->option);
        if (udp) {
            ogs_info("udp_server() [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));

            node->sock = udp;
        }else{ 
            ogs_error("udp_server() error [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));
            return OGS_ERROR;
        }

        node->poll = ogs_pollset_add(ogs_app()->pollset,
                OGS_POLLIN, udp->fd, icps_server_recv_cb, udp);
        ogs_assert(node->poll);
    }
#endif
    
    node = amf_self()->udp_node;
    udp = ogs_udp_server(node->addr, node->option);
    if (udp) {
        ogs_info("udp_server() [%s]:%d",
                OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));

        node->sock = udp;
    }else{ 
        ogs_error("udp_server() error [%s]:%d",
                OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));
        return OGS_ERROR;
    }

    node->poll = ogs_pollset_add(ogs_app()->pollset,
            OGS_POLLIN, udp->fd, icps_server_recv_cb, udp);
    ogs_assert(node->poll);
	
	if (is_amf_sps())
	{
		//set timer
		amf_self()->t_hand_shake_interval = ogs_timer_add(ogs_app()->timer_mgr,amf_timer_internel_heart_beat_timer_expire,udp);
		ogs_timer_start(amf_self()->t_hand_shake_interval,ogs_time_from_sec(UDP_INI_HEART_BEAT_INTERVAL));
	}
    else
    {
        amf_self()->t_hand_shake_check = ogs_timer_add(ogs_app()->timer_mgr,amf_timer_internel_heart_beat_check_expire,udp);
		ogs_timer_start(amf_self()->t_hand_shake_check,ogs_time_from_sec(UDP_INI_HEART_BEAT_INTERVAL));
    }
	
    return OGS_OK;
}

void udp_ini_close(void)
{
    ogs_socknode_free(amf_self()->udp_node);
    if (is_amf_icps())
    {
        int i;

        for (i = 1; i <= amf_self()->spsnum && i <= MAX_SPS_NUM; i++)
        {
            ogs_socknode_free(amf_self()->sps_nodes[i]);
        }

        if (amf_self()->t_hand_shake_check)
        {   
            ogs_timer_delete(amf_self()->t_hand_shake_check);
        }

    }
    else
    {
        ogs_socknode_free(amf_self()->icps_node);

        if (amf_self()->t_hand_shake_interval)
        {   
            ogs_timer_delete(amf_self()->t_hand_shake_interval);
        }
    }

    ogs_socknode_remove_all(&amf_self()->icps_list);
    ogs_socknode_remove_all(&amf_self()->icps_list6);
}

void icps_server_recv_cb(short when, ogs_socket_t fd, void *data)
{
    int rv;

    ssize_t size;
    amf_event_t *e = NULL;
    ogs_pkbuf_t *pkbuf = NULL;
    ogs_sockaddr_t from;
  

    ogs_assert(fd != INVALID_SOCKET);

    pkbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
    ogs_assert(pkbuf);
    ogs_pkbuf_put(pkbuf, OGS_MAX_SDU_LEN);

    size = ogs_recvfrom(fd, pkbuf->data, pkbuf->len, 0, &from);
    if (size <= 0) {
        ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno,
                "ogs_recvfrom() failed");
        ogs_pkbuf_free(pkbuf);
        return;
    }

    ogs_pkbuf_trim(pkbuf, size);

    e = amf_event_new(AMF_EVENT_INTERNEL_MESSAGE);
    ogs_assert(e);

    e->pkbuf = pkbuf;
    //e->client = from;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        ogs_pkbuf_free(e->pkbuf);
        ogs_event_free(e);
    }
}

void icps_client_recv_cb(short when, ogs_socket_t fd, void *data)
{
    int rv;

    ssize_t size;
    amf_event_t *e = NULL;
    ogs_pkbuf_t *pkbuf = NULL;
    ogs_sockaddr_t from;
  

    ogs_assert(fd != INVALID_SOCKET);

    pkbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
    ogs_assert(pkbuf);
    ogs_pkbuf_put(pkbuf, OGS_MAX_SDU_LEN);

    size = ogs_recvfrom(fd, pkbuf->data, pkbuf->len, 0, &from);
    if (size <= 0) {
        ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno,
                "ogs_recvfrom() failed");
        ogs_pkbuf_free(pkbuf);
        return;
    }

    ogs_pkbuf_trim(pkbuf, size);

    e = amf_event_new(AMF_EVENT_INTERNEL_MESSAGE);
    ogs_assert(e);

    e->pkbuf = pkbuf;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        ogs_pkbuf_free(e->pkbuf);
        ogs_event_free(e);
    }
}

//icps给sps发送消息时调用
int udp_ini_sendto(const void *buf, size_t len, int sps_id)
{
	if (sps_id > MAX_SPS_NUM)
	{
		return 0;
	}
	return ogs_sendto(amf_self()->udp_node->sock->fd, buf, len, 0, amf_self()->sps_nodes[sps_id]->addr);
}

//sps给icps发送消息时调用
int udp_ini_sendto_icps(const void *buf, size_t len)
{
	return ogs_sendto(amf_self()->udp_node->sock->fd, buf, len, 0, amf_self()->icps_node->addr);
}

bool add_module_info(uint8_t b_module_no)
{
    if (g_pt_pkt_fwd_tbl->b_sps_num >= MAX_SPS_NUM)
    {   
        return false;
    }
    g_pt_pkt_fwd_tbl->ta_sps_infos[g_pt_pkt_fwd_tbl->b_sps_num].module_no           = b_module_no;
    g_pt_pkt_fwd_tbl->ta_sps_infos[g_pt_pkt_fwd_tbl->b_sps_num].lost_heart_beat_cnt = 0;
    g_pt_pkt_fwd_tbl->b_sps_num++;
    ogs_info("add a new module, id:%d,sps num:%d.", b_module_no, g_pt_pkt_fwd_tbl->b_sps_num);
    return true;
}

bool delete_module_info(uint8_t b_module_no)
{
    return delete_module_from_array(&(g_pt_pkt_fwd_tbl->b_sps_num),
                                               g_pt_pkt_fwd_tbl->ta_sps_infos,
                                               b_module_no);
}

bool find_module_info(uint8_t b_module_no)
{
    uint8_t      bLoop;

    for (bLoop = 0; bLoop < g_pt_pkt_fwd_tbl->b_sps_num; bLoop++)
    {
        if (b_module_no == g_pt_pkt_fwd_tbl->ta_sps_infos[bLoop].module_no)
        {
            g_pt_pkt_fwd_tbl->ta_sps_infos[bLoop].lost_heart_beat_cnt = 0;
            return true;
        }
    }

    return false;
}

module_info_t * find_module_info_ex(uint8_t b_module_no)
{
    uint8_t      bLoop;

    for (bLoop = 0; bLoop < g_pt_pkt_fwd_tbl->b_sps_num; bLoop++)
    {
        if (b_module_no == g_pt_pkt_fwd_tbl->ta_sps_infos[bLoop].module_no)
        {            
            return &g_pt_pkt_fwd_tbl->ta_sps_infos[bLoop];
        }
    }

    return NULL;
}

bool delete_module_from_array(uint8_t *pbNum , module_info_t *pt_module_info, uint8_t b_module_no)
{
    uint8_t      bLoop;
    uint8_t      b_module_no_temp;
    
    if (pbNum == NULL
        || pt_module_info == NULL)
    {
        return false;
    }

    for (bLoop = 0; bLoop < (*pbNum); bLoop++)
    {
        b_module_no_temp = pt_module_info[bLoop].module_no;
        if (b_module_no_temp == b_module_no)
        {
            ogs_info("Del module From Arrary! b_sps_id=%d, module_type=%d\n", b_module_no, pt_module_info[bLoop].module_type); 
                        
            (*pbNum)--;
            pt_module_info[bLoop] = pt_module_info[*pbNum];
            memset(&pt_module_info[*pbNum], 0x00, sizeof(module_info_t));
                     
            return true;
        }
    }

    return false;
}

void udp_ini_hand_shake()
{
    amf_internel_msg_t internel_msg;

    internel_msg.msg_type   = INTERNEL_MSG_HAND_SHAKE_REQ;
    internel_msg.sps_id     = g_sps_id;
    internel_msg.sps_state  = 1;
    //给icps发送握手消息
    udp_ini_sendto_icps(&internel_msg,sizeof(internel_msg));
    ogs_info("sps send internel msg handshake req to icps,msg_type:%d,sps_id:%d,state:%d.",internel_msg.msg_type,internel_msg.sps_id,internel_msg.sps_state);
    if (amf_self()->t_hand_shake_interval)
    {
        ogs_timer_start(amf_self()->t_hand_shake_interval, ogs_time_from_sec(UDP_INI_HEART_BEAT_INTERVAL));
    }
}

void udp_ini_hand_shake_check()
{
    int i;
    for (i = 0; i < g_pt_pkt_fwd_tbl->b_sps_num; i++)
    {   
        g_pt_pkt_fwd_tbl->ta_sps_infos[i].lost_heart_beat_cnt++;
        ogs_info("icps hand shake check, acvite module num:%d, current module %d, lostcnt:%d.", 
            g_pt_pkt_fwd_tbl->b_sps_num,g_pt_pkt_fwd_tbl->ta_sps_infos[i].module_no,g_pt_pkt_fwd_tbl->ta_sps_infos[i].lost_heart_beat_cnt);
    
        if (g_pt_pkt_fwd_tbl->ta_sps_infos[i].lost_heart_beat_cnt >= 3)
        {
            ogs_error("module hand shake check timeout, delete module %d.",g_pt_pkt_fwd_tbl->ta_sps_infos[i].module_no);
            delete_module_info(g_pt_pkt_fwd_tbl->ta_sps_infos[i].module_no);
        }
    }

    if (amf_self()->t_hand_shake_check)
    {
        ogs_timer_start(amf_self()->t_hand_shake_check, ogs_time_from_sec(UDP_INI_HEART_BEAT_INTERVAL));
    }
}

/***************handle类函数***************/

void udp_ini_handle_hand_shake(amf_internel_msg_t *pmsg)
{
    int sent = 0;
    ogs_info("icps recv internel msg handshake req from sps,msg_type:%d,sps_id:%d,state:%d.",pmsg->msg_type,pmsg->sps_id,pmsg->sps_state);

    pmsg->msg_type = INTERNEL_MSG_HAND_SHAKE_RSP;
    sent = udp_ini_sendto(pmsg, sizeof(amf_internel_msg_t), pmsg->sps_id);
    if (sent < 0 || sent != sizeof(amf_internel_msg_t)) {
        ogs_error("ogs_sendto() failed");
    }else{
        ogs_info("icps send internel msg handshake rsp,msg_type:%d,sps_id:%d,state:%d.",pmsg->msg_type,pmsg->sps_id,pmsg->sps_state);
    }

    module_info_t *p_module_info = find_module_info_ex(pmsg->sps_id);
    if (NULL == p_module_info){
        add_module_info(pmsg->sps_id);
    }else{
        p_module_info->lost_heart_beat_cnt = 0;
    }
}