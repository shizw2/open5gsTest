#include "udp-ini-path.h"

extern int g_sps_id;
#if 0
int sps_udp_ini_open(void)
{
    ogs_socknode_t *node = NULL;
    ogs_sock_t *udp = NULL;
    char buf[OGS_ADDRSTRLEN];

    int rv;
    ogs_sockaddr_t  *internel_addr;
	ogs_sockaddr_t  *icps_addr;

    char ipstring[20] = {0};
    ogs_snprintf(ipstring, sizeof(ipstring), "128.128.128.%u", g_sps_id);

    rv = ogs_getaddrinfo(&internel_addr, AF_INET, ipstring, amf_self()->icps_port, 0);
    ogs_assert(rv == OGS_OK);

    amf_self()->sps_node = ogs_socknode_new(internel_addr);
    ogs_assert(amf_self()->sps_node);

    rv = ogs_getaddrinfo(&icps_addr, AF_INET, "128.128.128.127", amf_self()->icps_port, 0);
    ogs_assert(rv == OGS_OK);

    amf_self()->icps_node = ogs_socknode_new(icps_addr);
    ogs_assert(amf_self()->icps_node); 

#if 0
    ogs_list_for_each(&amf_self()->icps_list, node) {
        udp = ogs_udp_client(node->addr, node->option);
        if (udp) {
            ogs_info("ogs_udp_client() [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));

            node->sock = udp;
        }else{ 
            ogs_error("ogs_udp_client() error [%s]:%d",
                    OGS_ADDR(node->addr, buf), OGS_PORT(node->addr));
            return OGS_ERROR;
        }

        node->poll = ogs_pollset_add(ogs_app()->pollset,
                OGS_POLLIN, udp->fd, icps_client_recv_cb, udp);
        ogs_assert(node->poll);
    }
#endif
    node = amf_self()->sps_node;
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
	
    //set timer
	ogs_timer_t *timer = ogs_timer_add(ogs_app()->timer_mgr,amf_timer_internel_heart_beat_timer_expire,udp);
	
	ogs_timer_start(timer,ogs_time_from_sec(1));

    return OGS_OK;
}
#endif

int icps_udp_ini_open(void)
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
		ogs_timer_t *timer = ogs_timer_add(ogs_app()->timer_mgr,amf_timer_internel_heart_beat_timer_expire,udp);
		
		ogs_timer_start(timer,ogs_time_from_sec(1));
	}
	
    return OGS_OK;
}

void udp_ini_close(void)
{
    if (is_amf_icps())
    {
        ogs_info("free socknode.");
        ogs_socknode_free(amf_self()->icps_node);

        int i;

        for (i = 1; i <= amf_self()->spsnum && i <= MAX_SPS_NUM; i++)
        {
            ogs_socknode_free(amf_self()->sps_nodes[i]);
        }
    }
    else
    {
        ogs_socknode_free(amf_self()->udp_node);
    }
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


int udp_ini_sendto(const void *buf, size_t len, int sps_id)
{
	if (sps_id > MAX_SPS_NUM)
	{
		return 0;
	}
	return ogs_sendto(amf_self()->icps_node->sock->fd, buf, len, 0, amf_self()->sps_nodes[sps_id]->addr);
}