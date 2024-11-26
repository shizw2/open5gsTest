#include "udp-ini-path.h"
#include "namf-handler.h"
#include "sbi-path.h"

extern int g_sps_id;
int send_heart_cnt;//for sps check icps offline
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

    ogs_list_for_each(&amf_self()->icps_list, node){
        ogs_info("fount a icps addr %s",OGS_ADDR(node->addr, buf));
        break;
    }
    
    ogs_error("udp_ini_open()");

    if(!node){
        ogs_error("no icps addr");
        rv = ogs_getaddrinfo(&icps_addr, AF_INET, "128.128.128.127", amf_self()->icps_port, 0);
        ogs_assert(rv == OGS_OK);
    }else{
        icps_addr = node->addr;
        ogs_error("has icps addr %s",OGS_ADDR(icps_addr, buf));
        //rv = ogs_getaddrinfo(&icps_addr, AF_INET, "128.128.128.127", amf_self()->icps_port, 0);
        //ogs_assert(rv == OGS_OK);
    }


	if (is_amf_icps())
	{
        // rv = ogs_getaddrinfo(&internel_addr, AF_INET, icpsaddr, amf_self()->icps_port, 0);
        // ogs_assert(rv == OGS_OK);

		amf_self()->udp_node = ogs_socknode_new(icps_addr);
		ogs_assert(amf_self()->udp_node);
		
		/*设置sps的信息,用于发送消息*/
		for (i = 1; i <= amf_self()->spsnum && i <= MAX_SPS_NUM; i++)
		{
			char ipstring[20] = {0};
			//ogs_snprintf(ipstring, sizeof(ipstring), "128.128.128.%u", i);
            ogs_snprintf(ipstring, sizeof(ipstring), "%u.%d.%u.%u", icps_addr->sin.sin_addr.s_addr & 0xFF,icps_addr->sin.sin_addr.s_addr>>8& 0xFF,icps_addr->sin.sin_addr.s_addr>>16& 0xFF,i);
            ogs_info("sps string:%s",ipstring);
			rv = ogs_getaddrinfo(&internel_addr, AF_INET, ipstring, amf_self()->icps_port, 0);
			ogs_assert(rv == OGS_OK);

			amf_self()->sps_nodes[i] = ogs_socknode_new(internel_addr);
			ogs_assert(amf_self()->sps_nodes[i]);
		}

		for (i = 1; i <= amf_self()->spsnum && i <= MAX_SPS_NUM; i++)
		{
            ogs_info("icps module, icps addr:%s",OGS_ADDR(icps_addr, buf));
            ogs_info("icps module, ssps addr:%s",OGS_ADDR(amf_self()->sps_nodes[i]->addr, buf));
		}
	}
	else/*sps*/
	{
		char ipstring[20] = {0};
		//ogs_snprintf(ipstring, sizeof(ipstring), "128.128.128.%u", g_sps_id);
        ogs_snprintf(ipstring, sizeof(ipstring), "%u.%d.%u.%u", icps_addr->sin.sin_addr.s_addr & 0xFF,icps_addr->sin.sin_addr.s_addr>>8& 0xFF,icps_addr->sin.sin_addr.s_addr>>16& 0xFF, g_sps_id);
		rv = ogs_getaddrinfo(&internel_addr, AF_INET, ipstring, amf_self()->icps_port, 0);
		ogs_assert(rv == OGS_OK);

		amf_self()->udp_node = ogs_socknode_new(internel_addr);
		ogs_assert(amf_self()->udp_node);
	
	    /*设置icps的信息,用于发送消息*/
		// rv = ogs_getaddrinfo(&icps_addr, AF_INET, icpsaddr, amf_self()->icps_port, 0);
		// ogs_assert(rv == OGS_OK);
        ogs_info("sps module, icps addr:%s",OGS_ADDR(icps_addr, buf));
        ogs_info("sps module, sps addr:%s",OGS_ADDR(internel_addr, buf));
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
	if (sps_id > MAX_SPS_NUM||sps_id >amf_self()->spsnum)
	{
		return 0;
	}
	return ogs_sendto(amf_self()->udp_node->sock->fd, buf, len, 0, amf_self()->sps_nodes[sps_id]->addr);
}

int udp_ini_msg_sendto(int msg_type, ogs_sbi_udp_header_t *header,const void *buf, size_t len, int sps_id)
{
    ogs_pkbuf_t *pkbuf = NULL;
    amf_internel_msg_header_t internel_msg;
    ssize_t sent;

    if (sps_id > MAX_SPS_NUM||sps_id >amf_self()->spsnum)
    {
        ogs_error("sps id %d is out of range.",sps_id);
        return OGS_ERROR;
    }

    #if 0	
    ogs_info("SENDING...[%ld]", len);
    if (len){
        ogs_info("%s", (char*)buf);
    }
    #endif

    internel_msg.msg_type   = msg_type;
    internel_msg.sps_id     = sps_id;
    internel_msg.sps_state  = 1;

    pkbuf = ogs_pkbuf_alloc(NULL, sizeof(amf_internel_msg_header_t) + sizeof(ogs_sbi_udp_header_t) + len);
    ogs_assert(pkbuf);

    ogs_pkbuf_put_data(pkbuf, &internel_msg, sizeof(amf_internel_msg_header_t));
    ogs_pkbuf_put_data(pkbuf, header, sizeof(ogs_sbi_udp_header_t));
    ogs_pkbuf_put_data(pkbuf, buf, len);	

    sent = ogs_sendto(amf_self()->udp_node->sock->fd, pkbuf->data, pkbuf->len, 0, amf_self()->sps_nodes[sps_id]->addr);
    if (sent < 0 || sent != pkbuf->len) {
        ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno,
                "ogs_sendto() failed");
        return OGS_ERROR;
    }

    ogs_info("udp_ini_msg_sendto success, msg_type:%d, msg_len:%d, stream:%lx.",msg_type,pkbuf->len,header->stream_pointer);

    ogs_print_sbi_udp_header(header, true);
    if (pkbuf->len > sizeof(amf_internel_msg_header_t)+sizeof(ogs_sbi_udp_header_t)){
        ogs_info("sbi content:%s", (char*)(pkbuf->data+sizeof(amf_internel_msg_header_t)+sizeof(ogs_sbi_udp_header_t)));
    }

    ogs_pkbuf_free(pkbuf);
    return OGS_OK;
}

//sps给icps发送消息时调用
int udp_ini_sendto_icps(const void *buf, size_t len)
{
	return ogs_sendto(amf_self()->udp_node->sock->fd, buf, len, 0, amf_self()->icps_node->addr);
}

int udp_ini_msg_sendto_icps(int msg_type, ogs_sbi_udp_header_t *header,const void *buf, size_t len)
{
	ogs_pkbuf_t *pkbuf = NULL;
	amf_internel_msg_header_t internel_msg;
	ssize_t sent;
	
	pkbuf = ogs_pkbuf_alloc(NULL, sizeof(amf_internel_msg_header_t) + sizeof(ogs_sbi_udp_header_t) + len);
    ogs_assert(pkbuf);
    //ogs_pkbuf_reserve(pkbuf, sizeof(amf_internel_msg_header_t));
	internel_msg.msg_type   = msg_type;   
	
    ogs_pkbuf_put_data(pkbuf, &internel_msg, sizeof(amf_internel_msg_header_t));
    ogs_pkbuf_put_data(pkbuf, header, sizeof(ogs_sbi_udp_header_t));
    ogs_pkbuf_put_data(pkbuf, buf, len);
	
	sent = ogs_sendto(amf_self()->udp_node->sock->fd, pkbuf->data, pkbuf->len, 0, amf_self()->icps_node->addr);
	if (sent < 0 || sent != pkbuf->len) {
		ogs_log_message(OGS_LOG_ERROR, ogs_socket_errno,
				"ogs_sendto() failed");
		return OGS_ERROR;
	}
	
	ogs_info("udp_ini_msg_sendto_icps success, msg_type:%d.",msg_type);
	
	ogs_pkbuf_free(pkbuf);
	return OGS_OK;
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

void udp_ini_send_hand_shake()
{
    amf_internel_msg_header_t internel_msg;

    internel_msg.msg_type   = INTERNEL_MSG_HAND_SHAKE_REQ;
    internel_msg.sps_id     = g_sps_id;
    internel_msg.sps_state  = 1;
    //给icps发送握手消息
    udp_ini_sendto_icps(&internel_msg,sizeof(internel_msg));
    //ogs_info("sps send internel msg handshake req to icps,msg_type:%d,sps_id:%d,state:%d.",internel_msg.msg_type,internel_msg.sps_id,internel_msg.sps_state);
    if (amf_self()->t_hand_shake_interval)
    {
        ogs_timer_start(amf_self()->t_hand_shake_interval, ogs_time_from_sec(UDP_INI_HEART_BEAT_INTERVAL));
    }
    send_heart_cnt=send_heart_cnt+1;
}

void udp_ini_hand_shake_check()
{
    int i;
    for (i = 0; i < g_pt_pkt_fwd_tbl->b_sps_num; i++)
    {   
        g_pt_pkt_fwd_tbl->ta_sps_infos[i].lost_heart_beat_cnt++;
        if (g_pt_pkt_fwd_tbl->ta_sps_infos[i].lost_heart_beat_cnt > 2)
        {
            ogs_warn("icps hand shake check, acvite module num:%d, current module %d, lostcnt:%d.", 
            g_pt_pkt_fwd_tbl->b_sps_num,g_pt_pkt_fwd_tbl->ta_sps_infos[i].module_no,g_pt_pkt_fwd_tbl->ta_sps_infos[i].lost_heart_beat_cnt);
        }

        if (g_pt_pkt_fwd_tbl->ta_sps_infos[i].lost_heart_beat_cnt > 3)
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

void udp_ini_send_supi_notify(amf_ue_t *amf_ue)
{
    amf_internel_msgbuf_t internel_msg;
    ran_ue_t *ran_ue;

    if (NULL == amf_ue)
    {
        return;
    }

    memset(&internel_msg, 0, sizeof(amf_internel_msgbuf_t));

    internel_msg.msg_head.msg_type   = INTERNEL_MSG_SUPI_NOTIFY;
    internel_msg.msg_head.sps_id     = g_sps_id;
    internel_msg.msg_head.sps_state  = 1;

    ran_ue = ran_ue_find_by_id(amf_ue->ran_ue_id);

    if (NULL == ran_ue){
        ogs_error("udp_ini_send_supi_notify,can't find ran_ue by ran_ue_id:%d.", amf_ue->ran_ue_id);
        return;
    }

    internel_msg.msg_head.amf_ue_ngap_id = ran_ue->amf_ue_ngap_id;
    
    memcpy(internel_msg.supi,amf_ue->supi,strlen(amf_ue->supi));

    ogs_info("sps send supi notify to icps, supi:%s",internel_msg.supi);
    udp_ini_sendto_icps(&internel_msg,sizeof(internel_msg));
}

void udp_ini_send_supi_ran_hash_remove_notify(amf_ue_t *amf_ue)
{
    amf_internel_msgbuf_t internel_msg;

    if (NULL == amf_ue)
    {
        return;
    }

    memset(&internel_msg, 0, sizeof(amf_internel_msgbuf_t));

    internel_msg.msg_head.msg_type   = INTERNEL_MSG_SUPI_HASH_REMOVE_NOTIFY;
    internel_msg.msg_head.sps_id     = g_sps_id;
    internel_msg.msg_head.sps_state  = 1;    
    
    memcpy(internel_msg.supi,amf_ue->supi,strlen(amf_ue->supi));

    ogs_info("sps send supi remove notify to icps, supi:%s",internel_msg.supi);
    udp_ini_sendto_icps(&internel_msg,sizeof(internel_msg));
}
/***************handle类函数***************/

void udp_ini_icps_handle_hand_shake(amf_internel_msg_header_t *pmsg)
{
    int sent = 0;
    //ogs_info("icps recv internel msg handshake req from sps,msg_type:%d,sps_id:%d,state:%d.",pmsg->msg_type,pmsg->sps_id,pmsg->sps_state);

    pmsg->msg_type = INTERNEL_MSG_HAND_SHAKE_RSP;
    sent = udp_ini_sendto(pmsg, sizeof(amf_internel_msg_header_t), pmsg->sps_id);
    if (sent < 0 || sent != sizeof(amf_internel_msg_header_t)) {
        ogs_error("ogs_sendto() failed");
    }else{
        //ogs_info("icps send internel msg handshake rsp,msg_type:%d,sps_id:%d,state:%d.",pmsg->msg_type,pmsg->sps_id,pmsg->sps_state);
    }

    module_info_t *p_module_info = find_module_info_ex(pmsg->sps_id);
    if (NULL == p_module_info){
        add_module_info(pmsg->sps_id);
    }else{
        p_module_info->lost_heart_beat_cnt = 0;
    }
}

void udp_ini_handle_sbi_msg(ogs_pkbuf_t *pkbuf)
{	
    ogs_sbi_message_t sbi_message;
    ogs_sbi_request_t request;
    ogs_sbi_udp_header_t *p_udp_header;
    int rv;
    const char *api_version = NULL;
    ogs_sbi_stream_t *stream = NULL;
    int i;

    memset(&request, 0, sizeof(ogs_sbi_request_t));
	ogs_info("sps recv sbi msg, msg_len:%d,msg_head_len:%ld,udp_head_len:%ld.", pkbuf->len,sizeof(amf_internel_msg_header_t),sizeof(ogs_sbi_udp_header_t));					
	if (pkbuf->len > sizeof(amf_internel_msg_header_t)+sizeof(ogs_sbi_udp_header_t)){
		//根据header解析content
        request.http.content = (char*)(pkbuf->data+sizeof(amf_internel_msg_header_t)+sizeof(ogs_sbi_udp_header_t));
        request.http.content_length = pkbuf->len - sizeof(amf_internel_msg_header_t)-sizeof(ogs_sbi_udp_header_t);
      
        p_udp_header = (ogs_sbi_udp_header_t*)(char*)(pkbuf->data+sizeof(amf_internel_msg_header_t));

        memset(&sbi_message, 0, sizeof(sbi_message));
        sbi_message.http.content_type = p_udp_header->request.content_type;
        
        ogs_print_sbi_udp_header(p_udp_header,true);
        ogs_info("http.content_type:%s", sbi_message.http.content_type);    
        ogs_info("sbi content_length:%ld,content:%s", request.http.content_length,request.http.content);
            

        rv = ogs_sbi_parse_udp_request(&sbi_message,&request,p_udp_header);

        if (rv != OGS_OK) {
            /* 'sbi_message' buffer is released in ogs_sbi_parse_request() */
            ogs_error("ogs_sbi_parse_udp_request, parse HTTP sbi_message");
            return;
        }

        stream = p_udp_header->stream_pointer;

        ogs_info("udp_ini_handle_sbi_msg, stream:%p.",stream);

        SWITCH(sbi_message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NUDM_SDM)
            api_version = OGS_SBI_API_V2;
            break;
        DEFAULT
            api_version = OGS_SBI_API_V1;
        END

        ogs_assert(api_version);
        if (strcmp(sbi_message.h.api.version, api_version) != 0) {
            ogs_error("Not supported version [%s]", sbi_message.h.api.version);
            ogs_assert(true ==
                ogs_sbi_server_send_error(
                    stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    &sbi_message, "Not supported version", NULL, NULL));
            ogs_sbi_message_free(&sbi_message);
            return;
        }

        SWITCH(sbi_message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NNRF_NFM)

            SWITCH(sbi_message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_STATUS_NOTIFY)
                SWITCH(sbi_message.h.method)
                CASE(OGS_SBI_HTTP_METHOD_POST)
                    ogs_nnrf_nfm_handle_nf_status_notify(stream, &sbi_message);
                    break;

                DEFAULT
                    ogs_error("Invalid HTTP method [%s]", sbi_message.h.method);
                    ogs_assert(true ==
                        ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_FORBIDDEN, &sbi_message,
                            "Invalid HTTP method", sbi_message.h.method, NULL));
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[0]);
                ogs_assert(true ==
                    ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                        "Invalid resource name",
                        sbi_message.h.resource.component[0], NULL));
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAMF_COMM)        
            SWITCH(sbi_message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_UE_CONTEXTS)
                SWITCH(sbi_message.h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_N1_N2_MESSAGES)
                    SWITCH(sbi_message.h.method)
                    CASE(OGS_SBI_HTTP_METHOD_POST)					
                        rv = amf_namf_comm_handle_n1_n2_message_transfer(
                                stream, &sbi_message);
                        if (rv != OGS_OK) {
                            ogs_assert(true ==
                                ogs_sbi_server_send_error(stream,
                                    OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                                    &sbi_message,
                                    "No N1N2MessageTransferReqData", NULL, NULL));
                        }
                        break;

                    DEFAULT
                        ogs_error("Invalid HTTP method [%s]",
                                sbi_message.h.method);
                        ogs_assert(true ==
                            ogs_sbi_server_send_error(stream,
                                OGS_SBI_HTTP_STATUS_FORBIDDEN, &sbi_message,
                                "Invalid HTTP method", sbi_message.h.method, NULL));
                    END
                    break;

                DEFAULT
                    ogs_error("Invalid resource name [%s]",
                            sbi_message.h.resource.component[2]);
                    ogs_assert(true ==
                        ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                            "Invalid resource name",
                            sbi_message.h.resource.component[2], NULL));
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[0]);
                ogs_assert(true ==
                    ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                        "Invalid resource name",
                        sbi_message.h.resource.component[0], NULL));
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAMF_CALLBACK)
            SWITCH(sbi_message.h.resource.component[1])
            CASE(OGS_SBI_RESOURCE_NAME_SM_CONTEXT_STATUS)
                amf_namf_callback_handle_sm_context_status(
                        stream, &sbi_message);
                break;

            CASE(OGS_SBI_RESOURCE_NAME_DEREG_NOTIFY)
                amf_namf_callback_handle_dereg_notify(stream, &sbi_message);
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SDMSUBSCRIPTION_NOTIFY)
                amf_namf_callback_handle_sdm_data_change_notify(
                        stream, &sbi_message);
                break;

            CASE(OGS_SBI_RESOURCE_NAME_AM_POLICY_NOTIFY)
                ogs_assert(true == ogs_sbi_send_http_status_no_content(stream));
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[1]);
                ogs_assert(true ==
                    ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                        "Invalid resource name",
                        sbi_message.h.resource.component[1], NULL));
            END
            break;

        DEFAULT
            ogs_error("Invalid API name [%s]", sbi_message.h.service.name);
            ogs_assert(true ==
                ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                    "Invalid API name", sbi_message.h.resource.component[0], NULL));
        END

        ogs_sbi_message_free(&sbi_message);

        //由于request没用使用ogs_sbi_request_new创建,因此不能使用ogs_sbi_request_free。
        //但对于内部的一些数据,需要手动释放
        //ogs_sbi_request_free(&request); 
        for (i = 0; i < request.http.num_of_part; i++) {
            if (request.http.part[i].pkbuf)
                ogs_pkbuf_free(request.http.part[i].pkbuf);
            if (request.http.part[i].content_id)
                ogs_free(request.http.part[i].content_id);
            if (request.http.part[i].content_type)
                ogs_free(request.http.part[i].content_type);
        }
       
    }	
}

void udp_ini_icps_handle_sbi_msg(ogs_pkbuf_t *pkbuf)
{	
    ogs_sbi_message_t sbi_message;
    ogs_sbi_request_t request;//暂时不用
    ogs_sbi_response_t *response;
    amf_internel_msg_header_t *msg_header = NULL;
    ogs_sbi_udp_header_t *p_udp_header = NULL;
    int rv;
    const char *api_version = NULL;
    ogs_sbi_stream_t *stream = NULL;
    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue_icps = NULL;

    memset(&request, 0, sizeof(ogs_sbi_request_t));
   

	ogs_info("icps recv sbi msg, msg_len:%d,msg_head_len:%ld,udp_head_len:%ld.", pkbuf->len,sizeof(amf_internel_msg_header_t),sizeof(ogs_sbi_udp_header_t));					
	if (pkbuf->len >= sizeof(amf_internel_msg_header_t)+sizeof(ogs_sbi_udp_header_t)){		
        p_udp_header = (ogs_sbi_udp_header_t*)(char*)(pkbuf->data+ sizeof(amf_internel_msg_header_t)); 
        msg_header = (amf_internel_msg_header_t*)pkbuf->data;   

        response = ogs_sbi_response_new();
        response->http.content_length = pkbuf->len - sizeof(amf_internel_msg_header_t)-sizeof(ogs_sbi_udp_header_t);
        //这里必须要拷贝一份，底层有释放操作
        response->http.content = ogs_strndup((char*)(pkbuf->data+sizeof(amf_internel_msg_header_t)+sizeof(ogs_sbi_udp_header_t)), response->http.content_length);
        ogs_sbi_update_response(p_udp_header,response);        
        
        stream = (ogs_sbi_stream_t *)p_udp_header->stream_pointer;
        ogs_print_sbi_udp_header(p_udp_header,false);
        ogs_info("sbi content_length:%ld,content:%s.", response->http.content_length, response->http.content);
        
        ogs_info("steam:%p.", stream);
        ogs_assert(true == ogs_sbi_server_send_response(stream, response));

        #if 0
        if (p_udp_header->service_type == OGS_SBI_SERVICE_TYPE_NAUSF_AUTH)
        {
            ogs_info("udp_ini_icps_handle_sbi_msg, aush auth.");

            ran_ue_icps = ran_ue_find_by_amf_ue_ngap_id(p_udp_header->ran_ue_ngap_id);
            if (ran_ue_icps && !ran_ue_icps->amf_ue){
                ogs_info("icps add new amf_ue.");
                amf_ue = amf_ue_add(ran_ue_icps);
                amf_ue->suci = "suci-0-999-70-0-0-0-0000021309";
            }

            ogs_assert(true ==
                        amf_ue_sbi_discover_and_send(
                            OGS_SBI_SERVICE_TYPE_NAUSF_AUTH, NULL,
                            amf_nausf_auth_build_authenticate, amf_ue, NULL));
        }
        #endif
    }	
}

void udp_ini_icps_handle_supi_notify(ogs_pkbuf_t *pkbuf)
{	
    int rv;
    ran_ue_t *ran_ue_icps = NULL;
    amf_internel_msgbuf_t *msg = NULL;
	
    msg = (amf_internel_msgbuf_t*)pkbuf->data;  

    ogs_info("udp_ini_icps_handle_supi_notify,supi:%s.",msg->supi);	


    ran_ue_icps = ran_ue_find_by_amf_ue_ngap_id(msg->msg_head.amf_ue_ngap_id);
    if (!ran_ue_icps){
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)msg->msg_head.amf_ue_ngap_id);
        return;
    }

    ran_ue_set_supi(ran_ue_icps,msg->supi);
}
void udp_ini_icps_handle_supi_hash_remove_notify(ogs_pkbuf_t *pkbuf)
{	
    int rv;
    ran_ue_t *ran_ue_icps = NULL;
    amf_internel_msgbuf_t *msg = NULL;
	icps_ue_spsno_t *icps_ue = NULL;
    msg = (amf_internel_msgbuf_t*)pkbuf->data;  

    ogs_info("udp_ini_icps_handle_supi_hash_remove_notify,supi:%s.",msg->supi);	
    icps_ue=icps_ue_find_by_supi(msg->supi);
    if(icps_ue)icps_ue_remove(icps_ue);
    ran_ue_icps = ran_ue_find_by_supi(msg->supi);
    if (!ran_ue_icps){
        ogs_info("No RAN UE hash : msg->supi:%s",
                msg->supi);
        return;
    }
    ran_ue_remove_supi(ran_ue_icps,msg->supi);    
    
}