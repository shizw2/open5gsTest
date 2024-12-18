/*
 * Copyright (C) 2019-2022 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "sbi-path.h"
#include "ogs-app-timer.h"

static sacc_context_t self;

int __sacc_log_domain;

static int context_initialized = 0;

sacc_node_t g_sacc_nodes[MAX_PEER_NUM+1];

void sacc_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize SACC context */
    memset(&self, 0, sizeof(sacc_context_t));

    ogs_log_install_domain(&__ogs_dbi_domain, "dbi", ogs_core()->log.level);
    ogs_log_install_domain(&__sacc_log_domain, "sacc", ogs_core()->log.level);

    context_initialized = 1;
}

void sacc_context_final(void)
{
    int n,i;
    
    ogs_assert(context_initialized == 1);

    context_initialized = 0;

    sacc_node_t *node = NULL;

    for (n = 1; n <= sacc_self()->nodeNum && n <= MAX_PEER_NUM; n++){
        node = &g_sacc_nodes[n];
        ogs_info("release node:%d",n);
        for (i = 0; i < node->num_of_ipv4; i++) {
            if (node->ipv4[i])
                ogs_freeaddrinfo(node->ipv4[i]);
        }

        // for (i = 0; i < node->supiRanges.num_of_supi_range; i++) {     
        //     ogs_info("sacc_context_final supiRange_free, start:%p,end:%p",node->supiRanges.supi_ranges[i].start,node->supiRanges.supi_ranges[i].end);    
        //     ogs_free(node->supiRanges.supi_ranges[i].start);
        //     ogs_free(node->supiRanges.supi_ranges[i].end);
        // }
        // node->supiRanges.num_of_supi_range = 0;
        supiRange_free(&node->supiRanges);
        ipRange_free(&node->staticIPRanges);
        ogs_free(node->uri);
    }
}

sacc_context_t *sacc_self(void)
{
    return &self;
}

static int sacc_context_prepare(void)
{
    return OGS_OK;
}

static int sacc_context_validation(void)
{
    return OGS_OK;
}

bool is_nfinfo_changed = false;
int sacc_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);
    
    is_nfinfo_changed = false;

    rv = sacc_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "sacc")) {
            ogs_yaml_iter_t sacc_iter;
            ogs_yaml_iter_recurse(&root_iter, &sacc_iter);
            while (ogs_yaml_iter_next(&sacc_iter)) {
                const char *sacc_key = ogs_yaml_iter_key(&sacc_iter);
                ogs_assert(sacc_key);
                if (!strcmp(sacc_key, "sbi")) {
                    /* handle config in sbi library */
                } else if (!strcmp(sacc_key, "service_name")) {
                    /* handle config in sbi library */
                } else if (!strcmp(sacc_key, "discovery")) {
                    /* handle config in sbi library */
                } else if (!strcmp(sacc_key, "metrics")) {
                    /* handle config in metrics library */
                } else if (!strcmp(sacc_key, "role")) {
                    self.role = ogs_yaml_iter_value(&sacc_iter);
                } else if (!strcmp(sacc_key, "enable")) {
                    self.enable = ogs_yaml_iter_bool(&sacc_iter);
                } else if (!strcmp(sacc_key, "inherite_enable")) {
                    self.inheriteEnable = ogs_yaml_iter_bool(&sacc_iter);
                } else if (!strcmp(sacc_key, "node_num")) {
                    const char *v = ogs_yaml_iter_value(&sacc_iter);
                    if (v) self.nodeNum = atoi(v);
                } else if (!strcmp(sacc_key, "scan_interval")) {
                    const char *v = ogs_yaml_iter_value(&sacc_iter);
                    if (v) self.scanInterval = atoi(v);
                } else if (!strcmp(sacc_key, "heartbeat_interval")) {
                    const char *v = ogs_yaml_iter_value(&sacc_iter);
                    if (v) self.heartbeatInterval = atoi(v);
                } else if (!strcmp(sacc_key, "heartbeat_lost")) {
                    const char *v = ogs_yaml_iter_value(&sacc_iter);
                    if (v) self.heartbeatLost = atoi(v);
                } else if (!strcmp(sacc_key, "device_type")) {
                    self.deviceType = ogs_yaml_iter_value(&sacc_iter);
                } else if (!strcmp(sacc_key, "device_name")) {
                    self.deviceName = ogs_yaml_iter_value(&sacc_iter);
                } else if (!strcmp(sacc_key, "device_seq")) {
                    self.deviceSeq = ogs_yaml_iter_value(&sacc_iter);
                } else if (!strcmp(sacc_key, "version")) {
                    self.version = ogs_yaml_iter_value(&sacc_iter);
                } else
                    ogs_warn("unknown key `%s`", sacc_key);
            }
        }
    }

    rv = sacc_context_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}

int yaml_check_proc(void)
{
    int rv;    
    //ogs_info("check yaml config.");

    if (!osg_app_is_config_modified()) {
        return 0;//如果文件未修改,则返回
    }

    ogs_info("yaml config file changed,reloading......");

    //0、读配置
    rv = ogs_app_config_read();
    if (rv != OGS_OK) return rv;

    //1、app级配置    
    rv = ogs_app_context_parse_config();
    if (rv != OGS_OK) return rv;

    //1.1重新设置日志级别
    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) return rv;

    rv = sacc_context_parse_config();
    if (rv != OGS_OK) return rv;
    
    bool needReRegister = false;
    if (ogs_global_conf()->parameter.capacity != ogs_sbi_self()->nf_instance->capacity){
        ogs_info("capacity changed from %d to %d.",ogs_sbi_self()->nf_instance->capacity,ogs_global_conf()->parameter.capacity);
        ogs_sbi_nf_instance_set_capacity(ogs_sbi_self()->nf_instance,ogs_global_conf()->parameter.capacity);
        needReRegister = true;
    }
    
    if (is_nfinfo_changed){
        needReRegister = true;
    }
    
    if (needReRegister){
        ogs_nnrf_nfm_send_nf_register(ogs_sbi_self()->nrf_instance);
    }
    
    return 0;
}

int sacc_sbi_context_get_nf_info(
        OpenAPI_nf_type_e nf_type, sacc_node_t *sacc_nodes)
{
    int rv;
    ogs_sbi_nf_info_t *nf_info = NULL;                        
    ogs_supi_range_t *supiRanges = NULL;
    ogs_ip_range_t *staticIPRanges = NULL;
    int i;

    ogs_sbi_nf_instance_t *nf_instance = NULL;

    switch (nf_type){
        // case OpenAPI_nf_type_UDM:
        //     nf_instance = sacc_nodes->udm_nf_instance;
        //     break;
        // case OpenAPI_nf_type_AUSF:
        //     nf_instance = sacc_nodes->ausf_nf_instance;
        //     break;    
        case OpenAPI_nf_type_SMF:
            nf_instance = sacc_nodes->smf_nf_instance;
            break;
        case OpenAPI_nf_type_AMF:
            nf_instance = sacc_nodes->amf_nf_instance;
            break; 
        default:
            ogs_error("unknown local node type:%d", nf_type);
            return OGS_ERROR;;
    }

    ogs_assert(nf_instance);

    for (i = 0; i < sacc_nodes->num_of_ipv4; i++){
        ogs_copyaddrinfo(&nf_instance->ipv4[i], sacc_nodes->ipv4[i]);
        if (nf_instance->nf_type == OpenAPI_nf_type_SMF){
            nf_instance->ipv4[i]->ogs_sin_port = sacc_nodes->smf_sbi_port;
        }else if (nf_instance->nf_type == OpenAPI_nf_type_AMF){
            nf_instance->ipv4[i]->ogs_sin_port = sacc_nodes->amf_sbi_port;  
        }
    }
    nf_instance->num_of_ipv4 = sacc_nodes->num_of_ipv4;

    if (nf_instance->nf_type == OpenAPI_nf_type_SMF){
        nf_info = ogs_sbi_nf_info_find(
                    &nf_instance->nf_info_list,
                        nf_instance->nf_type);
        
        if (nf_info == NULL){
            nf_info = ogs_sbi_nf_info_add(
                    &nf_instance->nf_info_list,
                        nf_instance->nf_type);
        }
        ogs_assert(nf_info);
        memset(&nf_info->smf.staticIPRanges, 0, sizeof(ogs_ip_range_t));
        add_ipRange(&sacc_nodes->staticIPRanges, &nf_info->smf.staticIPRanges);
    }
    
    // switch (nf_instance->nf_type){
    //     // case OpenAPI_nf_type_UDM:
    //     //     supiRange_copy(&sacc_nodes->supiRanges, &nf_info->udm.supiRanges);
    //     //     break;
    //     // case OpenAPI_nf_type_AUSF:
    //     //     supiRange_copy(&sacc_nodes->supiRanges, &nf_info->ausf.supiRanges);
    //     //     break;
    //     case OpenAPI_nf_type_SMF:
    //         // supiRange_copy(&sacc_nodes->supiRanges, &nf_info->smf.supiRanges);
    //         add_ipRange(&sacc_nodes->staticIPRanges, &nf_info->smf.staticIPRanges);
    //         break;
    //     default:
    //         break;
    // }

    return OGS_OK;
}

int sacc_sbi_context_update_nf_info(
        OpenAPI_nf_type_e nf_type, sacc_node_t *sacc_nodes)
{
    int rv;
    ogs_sbi_nf_info_t *nf_info = NULL;                        
    ogs_supi_range_t *supiRanges = NULL;
    ogs_ip_range_t *staticIPRanges = NULL;
    int i;

    ogs_sbi_nf_instance_t *nf_instance = NULL;

    switch (nf_type){
        case OpenAPI_nf_type_SMF:
            nf_instance = sacc_nodes->smf_nf_instance;
            break;
        default:
            ogs_error("unknown local node type:%d", nf_type);
            return OGS_ERROR;
    }

    ogs_assert(nf_instance);

    if (nf_instance->nf_type == OpenAPI_nf_type_SMF){
        nf_info = ogs_sbi_nf_info_find(
                    &nf_instance->nf_info_list,
                        nf_instance->nf_type);
        
        if (nf_info == NULL){
            nf_info = ogs_sbi_nf_info_add(
                    &nf_instance->nf_info_list,
                        nf_instance->nf_type);
        }
        ogs_assert(nf_info);

        memset(&nf_info->smf.staticIPRanges, 0, sizeof(ogs_ip_range_t));
        add_ipRange(&g_sacc_nodes[self.node].staticIPRanges, &nf_info->smf.staticIPRanges);//加自己的ip
        for (i = 0; i < self.temporaryServiceNum; i++){//加继承节点的ip
            ogs_info("add node %d's staticip to node %d.",self.temporaryServices[i].node, self.node);
            add_ipRange(&g_sacc_nodes[self.temporaryServices[i].node].staticIPRanges, &nf_info->smf.staticIPRanges);
        }
    }
    
    return OGS_OK;
}

int sacc_context_get_nodes_info(
        const char *local, sacc_node_t *sacc_nodes)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (local && !strcmp(root_key, local)) {
            //ogs_yaml_iter_t local_iter;
            //ogs_yaml_iter_recurse(&root_iter, &local_iter);
            int i;
            ogs_yaml_iter_t node_array, node_iter;
            ogs_yaml_iter_recurse(&root_iter, &node_array);

            do {             

                if (ogs_yaml_iter_type(&node_array) ==
                        YAML_MAPPING_NODE) {
                    memcpy(&node_iter, &node_array,
                            sizeof(ogs_yaml_iter_t));
                } else if (ogs_yaml_iter_type(&node_array) ==
                            YAML_SEQUENCE_NODE) {
                    if (!ogs_yaml_iter_next(&node_array))
                        break;
                    ogs_yaml_iter_recurse(&node_array, &node_iter);
                } else if (ogs_yaml_iter_type(&node_array) ==
                            YAML_SCALAR_NODE) {
                    break;
                } else
                    ogs_assert_if_reached();

                int cur_group = 0;
                int cur_node = 0;
                while (ogs_yaml_iter_next(&node_iter)) {
                    const char *node = NULL;
                    const char *node_key = ogs_yaml_iter_key(&node_iter);
                    ogs_assert(node_key);

                    if (!strcmp(node_key, "group")) {
                        const char *v = ogs_yaml_iter_value(&node_iter);                        
                        if (v) cur_group = atoi(v);
                        
                        if (cur_group  != self.group){
                            break;
                        }                                               
                    } else if (!strcmp(node_key, "node")) {
                        const char *v = ogs_yaml_iter_value(&node_iter);                        
                        if (v) cur_node = atoi(v);
                        if (cur_node > self.nodeNum){
                            ogs_error("node is too big, cur_node %d, self.nodeNum %d.",cur_node , self.nodeNum);
                            break;
                        }
                        sacc_nodes[cur_node].node = cur_node;
                        sacc_nodes[cur_node].group = self.group;
                    } else if (!strcmp(node_key, "priority")) {
                        const char *v = ogs_yaml_iter_value(&node_iter);                        
                        if (v) sacc_nodes[cur_node].priority = atoi(v);                      
                    } else if (!strcmp(node_key, "amf_sbi_port")) {
                        const char *v = ogs_yaml_iter_value(&node_iter);                        
                        if (v) sacc_nodes[cur_node].amf_sbi_port = atoi(v);                      
                    } else if (!strcmp(node_key, "smb_sbi_port")) {
                        const char *v = ogs_yaml_iter_value(&node_iter);                        
                        if (v) sacc_nodes[cur_node].smf_sbi_port = atoi(v);                      
                    } else if (!strcmp(node_key, "sbi")) {
                        ogs_yaml_iter_t sbi_iter;
                        ogs_yaml_iter_recurse(&node_iter, &sbi_iter);
                        while (ogs_yaml_iter_next(&sbi_iter)) {
                            const char *sbi_key = ogs_yaml_iter_key(&sbi_iter);
                            ogs_assert(sbi_key);
                            if (!strcmp(sbi_key, "server")) {
                                //rv = ogs_sbi_context_parse_server_config(
                                //        &sbi_iter, NULL);
                                int rv;
                                ogs_sbi_server_t *server = NULL;
                                OpenAPI_uri_scheme_e scheme = OpenAPI_uri_scheme_NULL;                            

                                ogs_yaml_iter_t server_iter, server_array;
                                ogs_yaml_iter_recurse(&sbi_iter, &server_array);
                                do {
                                    int i, family = AF_UNSPEC;
                                    int num = 0;
                                    const char *hostname[OGS_MAX_NUM_OF_HOSTNAME];
                                    int num_of_advertise = 0;
                                    const char *advertise[OGS_MAX_NUM_OF_HOSTNAME];

                                    uint16_t port = 0;
                                    const char *dev = NULL;
                                    ogs_sockaddr_t *addr = NULL;

                                    const char *private_key = NULL, *cert = NULL;

                                    bool verify_client = false;
                                    const char *verify_client_cacert = NULL;

                                    ogs_sockopt_t option;
                                    bool is_option = false;

                                    if (ogs_yaml_iter_type(&server_array) == YAML_MAPPING_NODE) {
                                        memcpy(&server_iter, &server_array, sizeof(ogs_yaml_iter_t));
                                    } else if (ogs_yaml_iter_type(&server_array) == YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(&server_array))
                                            break;
                                        ogs_yaml_iter_recurse(&server_array, &server_iter);
                                    } else if (ogs_yaml_iter_type(&server_array) == YAML_SCALAR_NODE) {
                                        break;
                                    } else
                                        ogs_assert_if_reached();

                                    while (ogs_yaml_iter_next(&server_iter)) {
                                        const char *server_key = ogs_yaml_iter_key(&server_iter);
                                        ogs_assert(server_key);
                                        if (!strcmp(server_key, OGS_SBI_INTERFACE_NAME_N32F)) {
                                            rv = ogs_sbi_context_parse_server_config(
                                                    &server_iter, server_key);
                                            if (rv != OGS_OK) {
                                                ogs_error("ogs_sbi_context_parse_server_config() "
                                                        "failed[key:%s]", server_key);
                                                return rv;
                                            }
                                        } else if (!strcmp(server_key, "family")) {
                                            const char *v = ogs_yaml_iter_value(&server_iter);
                                            if (v) family = atoi(v);
                                            if (family != AF_UNSPEC && family != AF_INET &&
                                                family != AF_INET6) {
                                                ogs_warn("Ignore family(%d) : AF_UNSPEC(%d), "
                                                    "AF_INET(%d), AF_INET6(%d) ",
                                                    family, AF_UNSPEC, AF_INET, AF_INET6);
                                                family = AF_UNSPEC;
                                            }
                                        } else if (!strcmp(server_key, "address")) {
                                            ogs_yaml_iter_t hostname_iter;
                                            ogs_yaml_iter_recurse(&server_iter, &hostname_iter);
                                            ogs_assert(ogs_yaml_iter_type(
                                                        &hostname_iter) != YAML_MAPPING_NODE);

                                            do {
                                                if (ogs_yaml_iter_type(
                                                            &hostname_iter) == YAML_SEQUENCE_NODE) {
                                                    if (!ogs_yaml_iter_next(&hostname_iter))
                                                        break;
                                                }

                                                ogs_assert(num < OGS_MAX_NUM_OF_HOSTNAME);
                                                hostname[num++] = ogs_yaml_iter_value(&hostname_iter);
                                            } while (ogs_yaml_iter_type(
                                                        &hostname_iter) == YAML_SEQUENCE_NODE);
                                        } else if (!strcmp(server_key, "advertise")) {
                                            ogs_yaml_iter_t advertise_iter;
                                            ogs_yaml_iter_recurse(&server_iter, &advertise_iter);
                                            ogs_assert(ogs_yaml_iter_type(
                                                        &advertise_iter) != YAML_MAPPING_NODE);

                                            do {
                                                if (ogs_yaml_iter_type(
                                                            &advertise_iter) == YAML_SEQUENCE_NODE) {
                                                    if (!ogs_yaml_iter_next(&advertise_iter))
                                                        break;
                                                }

                                                ogs_assert(num_of_advertise < OGS_MAX_NUM_OF_HOSTNAME);
                                                advertise[num_of_advertise++] =
                                                    ogs_yaml_iter_value(&advertise_iter);
                                            } while (ogs_yaml_iter_type(
                                                        &advertise_iter) == YAML_SEQUENCE_NODE);
                                        } else if (!strcmp(server_key, "port")) {
                                            const char *v = ogs_yaml_iter_value(&server_iter);
                                            if (v)
                                                port = atoi(v);
                                        } else if (!strcmp(server_key, "dev")) {
                                            dev = ogs_yaml_iter_value(&server_iter);
                                        } else if (!strcmp(server_key, "scheme")) {
                                            const char *v = ogs_yaml_iter_value(&server_iter);
                                            if (v) {
                                                if (!ogs_strcasecmp(v, "https"))
                                                    scheme = OpenAPI_uri_scheme_https;
                                                else if (!ogs_strcasecmp(v, "http"))
                                                    scheme = OpenAPI_uri_scheme_http;
                                                else
                                                    ogs_warn("unknown scheme `%s`", v);
                                            }
                                        } else if (!strcmp(server_key, "private_key")) {
                                            private_key = ogs_yaml_iter_value(&server_iter);
                                        } else if (!strcmp(server_key, "cert")) {
                                            cert = ogs_yaml_iter_value(&server_iter);
                                        } else if (!strcmp(server_key, "verify_client")) {
                                            verify_client = ogs_yaml_iter_bool(&server_iter);
                                        } else if (!strcmp(server_key, "verify_client_cacert")) {
                                            verify_client_cacert = ogs_yaml_iter_value(&server_iter);
                                        } else if (!strcmp(server_key, "option")) {
                                            rv = ogs_app_parse_sockopt_config(&server_iter, &option);
                                            if (rv != OGS_OK) {
                                                ogs_error("ogs_app_parse_sockopt_config() failed");
                                                return rv;
                                            }
                                            is_option = true;
                                        }
                                    }

                                    if (scheme == OpenAPI_uri_scheme_NULL)
                                        scheme = ogs_sbi_self()->tls.server.scheme;

                                    if (!port) {
                                        if (scheme == OpenAPI_uri_scheme_https)
                                            port = OGS_SBI_HTTPS_PORT;
                                        else if (scheme == OpenAPI_uri_scheme_http)
                                            port = OGS_SBI_HTTP_PORT;
                                        else
                                            ogs_assert_if_reached();
                                    }

                                    addr = NULL;
                                    for (i = 0; i < num; i++) {
                                        rv = ogs_addaddrinfo(&addr, family, hostname[i], port, 0);
                                        ogs_assert(rv == OGS_OK);
                                        sacc_nodes[cur_node].ipv4[sacc_nodes[cur_node].num_of_ipv4] = addr;
                                        sacc_nodes[cur_node].num_of_ipv4++;
                                    }

                                } while (ogs_yaml_iter_type(&server_array) == YAML_SEQUENCE_NODE);

                                if (rv != OGS_OK) {
                                    ogs_error("ogs_sbi_context_parse_server_"
                                            "config() failed");
                                    return rv;
                                }
                            } else
                                ogs_warn("unknown key `%s`", sbi_key);
                        }
                    }  else if (!strcmp(node_key, "info")) { //smf,amf的info都是单独解析的，不太合适放到某个library中
                   
                        ogs_yaml_iter_t info_array, info_iter;
                        ogs_yaml_iter_recurse(&node_iter, &info_array);

                        do {
                            ogs_sbi_nf_info_t *nf_info = NULL;                        
                            ogs_supi_range_t *supiRanges = NULL;
                            ogs_ip_range_t *staticIPRanges = NULL;
                            if (ogs_yaml_iter_type(&info_array) ==
                                    YAML_MAPPING_NODE) {
                                memcpy(&info_iter, &info_array,
                                        sizeof(ogs_yaml_iter_t));
                            } else if (ogs_yaml_iter_type(&info_array) ==
                                        YAML_SEQUENCE_NODE) {
                                if (!ogs_yaml_iter_next(&info_array))
                                    break;
                                ogs_yaml_iter_recurse(&info_array, &info_iter);
                            } else if (ogs_yaml_iter_type(&info_array) ==
                                        YAML_SCALAR_NODE) {
                                break;
                            } else
                                ogs_assert_if_reached();
 
                            while (ogs_yaml_iter_next(&info_iter)) {
                                const char *info_key =
                                    ogs_yaml_iter_key(&info_iter);
                                ogs_assert(info_key);
                                if (!strcmp(info_key, "supi")) {
                                    is_nfinfo_changed = ogs_sbi_context_parse_supi_ranges(&info_iter, &sacc_nodes[cur_node].supiRanges);
                                } else if (!strcmp(info_key, "static_ip")) {
                                    is_nfinfo_changed = ogs_sbi_context_parse_ip_ranges(&info_iter, &sacc_nodes[cur_node].staticIPRanges); 
                                }else
                                    ogs_warn("unknown key `%s`", info_key);
                            }

                        } while (ogs_yaml_iter_type(&info_array) ==
                                YAML_SEQUENCE_NODE);

                    } 
                }
            } while (ogs_yaml_iter_type(&node_array) ==
                    YAML_SEQUENCE_NODE);
            
        }
    }

    // rv = ogs_sbi_context_validation(local, nrf, scp);
    // if (rv != OGS_OK) return rv;

    return OGS_OK;
}

char* sacc_msg_ToString(int msg_type)
{
    const char *msg_typeArray[] =  { "handshake", "heartbeat"};
    size_t sizeofArray = sizeof(msg_typeArray) / sizeof(msg_typeArray[0]);
    if (msg_type < sizeofArray)
        return (char *)msg_typeArray[msg_type];
    else
        return (char *)"Unknown";
}

char * sacc_node_state_ToString(int state){
    const char *stateArray[] =  {"stop", "start"};
    size_t sizeofArray = sizeof(stateArray) / sizeof(stateArray[0]);
    if (state < sizeofArray)
        return (char *)stateArray[state];
    else
        return (char *)"Unknown";
}

int sacc_initialize_nodes(void) {
    int rv,n;
    char buf[OGS_ADDRSTRLEN];
    ogs_sbi_server_t *server = NULL;

    self.group = ogs_global_conf()->parameter.group;
    self.node = ogs_global_conf()->parameter.node;

    ogs_info("SACC initialized with configuration: enable=%d, port=%d, scanInterval=%d, heartbeatInterval=%d, heartbeatLost=%d, role=%s, group=%d, node=%d, nodeNum=%d",
                 self.enable, self.port, self.scanInterval, self.heartbeatInterval, self.heartbeatLost,
                 self.role, self.group, self.node, self.nodeNum);

    char ip[16];

    memset(g_sacc_nodes, 0, sizeof(sacc_node_t)*MAX_PEER_NUM);
    
    sacc_context_get_nodes_info("nodes", g_sacc_nodes);

    for (n = 1; n <= self.nodeNum && n <= MAX_PEER_NUM; n++){
        char ip[16];
        char uri[256] = {};
        //int base = 100 + self.group;
        //int offset = n * 2 + 1; // Calculate the last octet as N*2-1
        //snprintf(ip, sizeof(ip), "192.168.%d.%d", base, offset);

        // g_sacc_nodes[n].group = self.group;
        // g_sacc_nodes[n].node = n;
        if (n == self.node){//本节点上电,默认为online
            g_sacc_nodes[n].state = SACC_PEER_STATE_ONLINE;
        }else{
            g_sacc_nodes[n].state = SACC_PEER_STATE_OFFLINE;
        }

        //sacc_sbi_context_init_for_udm(&g_sacc_nodes[n]);
        //sacc_sbi_context_init_for_ausf(&g_sacc_nodes[n]);
        sacc_sbi_context_init_for_smf(&g_sacc_nodes[n]);
        sacc_sbi_context_init_for_amf(&g_sacc_nodes[n]);

        server = ogs_list_first(&ogs_sbi_self()->server_list);
        ogs_assert(server);
        
        ogs_info("sacc node %d num_of_ipv4:%d",n, g_sacc_nodes[n].num_of_ipv4);
        if (g_sacc_nodes[n].num_of_ipv4 > 0){
            ogs_info("sacc node %d addr:%s, port:%d",n, OGS_ADDR(g_sacc_nodes[n].ipv4[0],buf),OGS_PORT(g_sacc_nodes[n].ipv4[0]));
            sacc_associate_peer_client(&g_sacc_nodes[n]);
        }else{
            ogs_error("sacc node %d has no ipv4 addr",n);
            continue;
        }

        snprintf(uri, sizeof(uri), "http://%s:%d/acc/v1/nfinfo", OGS_ADDR(g_sacc_nodes[n].ipv4[0],buf),OGS_PORT(g_sacc_nodes[n].ipv4[0]));
        g_sacc_nodes[n].uri = ogs_strdup(uri);
        //snprintf(uri, sizeof(uri), "http://%s:%d/acc/v1/heartbeat", ip,g_local_node_config.port);
        //g_sacc_nodes[n].heartbeat_uri = ogs_strdup(uri);
        //ogs_info("node handshake uri:%s,heartbeat uri:%s", g_sacc_nodes[n].uri,g_sacc_nodes[n].heartbeat_uri);

        //TODO:模拟
        if (self.node == 1 && g_sacc_nodes[n].node != self.node){        
            self.temporaryServices[self.temporaryServiceNum].group =  g_sacc_nodes[n].group;
            self.temporaryServices[self.temporaryServiceNum].node =  g_sacc_nodes[n].node;
            self.temporaryServiceNum++;
        }
    }

    self.t_hand_shake_interval = ogs_timer_add(ogs_app()->timer_mgr,sacc_timer_node_handshake_timer_expire, NULL);
	ogs_timer_start(self.t_hand_shake_interval, ogs_time_from_sec(self.heartbeatInterval));


    return OGS_OK;
}

void sacc_associate_peer_client(sacc_node_t *peer)
{
    ogs_sbi_client_t *client = NULL;
    ogs_sockaddr_t *addr = NULL, *addr6 = NULL;
    OpenAPI_uri_scheme_e scheme = OpenAPI_uri_scheme_http;

    ogs_assert(peer);

    addr = peer->ipv4[0];

    ogs_info("sacc_associate_peer_client addr:%p",addr);

    if (!client) {
        client = ogs_sbi_client_find(
                scheme, NULL, 0, addr, addr6);
        if (!client) {
            ogs_debug("%s: ogs_sbi_client_add()", OGS_FUNC);
            client = ogs_sbi_client_add(
                    scheme, NULL, 0, addr, addr6);
            if (!client) {
                ogs_error("%s: ogs_sbi_client_add() failed", OGS_FUNC);
                return;
            }
        }    
    }

    if (client){
        OGS_SBI_SETUP_CLIENT(peer, client);
        ogs_info("add client to peer");
    }
}


void sacc_sbi_context_init_for_udm(sacc_node_t *peer)
{
    ogs_sbi_nf_instance_t *nf_instance = NULL;
	ogs_sbi_nf_service_t *nf_service = NULL;
	int i = 0;
	bool handled;
	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];    
    ogs_uuid_t uuid;

    ogs_uuid_format_custom(nf_instance_id, OpenAPI_nf_type_UDM, peer->group,peer->node, 0);        

    nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    }

    peer->udm_nf_instance = nf_instance; 

	ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_UDM);
    ogs_sbi_nf_fsm_init(nf_instance);  

    snprintf(nf_name, sizeof(nf_name), "udm_%d_%d",peer->group,peer->node); 

    sacc_sbi_context_get_nf_info(OpenAPI_nf_type_UDM, peer);
    
    nf_instance->time.heartbeat_interval = 0;//不搞心跳,否则NRF会超时释放掉. amf对这些网元搞心跳也麻烦

    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AUSF);

    ogs_sbi_nf_service_t *service = NULL;
    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_UEAU)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_UEAU);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AUSF);
    //}

    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_UECM)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_UECM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_SMF);
    //}

    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NUDM_SDM)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NUDM_SDM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V2, OGS_SBI_API_V2_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_SMF);
    //}
	return;	
}


void sacc_sbi_context_init_for_ausf(sacc_node_t *peer){
    ogs_sbi_nf_instance_t *nf_instance = NULL;
	ogs_sbi_nf_service_t *nf_service = NULL;
	
	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];
    int i;

    ogs_uuid_format_custom(nf_instance_id, OpenAPI_nf_type_AUSF, peer->group,peer->node, 0);  

	nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    }
    peer->ausf_nf_instance = nf_instance;
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_AUSF);
    ogs_sbi_nf_fsm_init(nf_instance);  

    /* Build NF instance information. It will be transmitted to NRF. */
    //ogs_sbi_nf_instance_build_default(nf_instance);
    ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    snprintf(nf_name, sizeof(nf_name), "ausf_%d_%d",peer->group,peer->node);
   
    sacc_sbi_context_get_nf_info(OpenAPI_nf_type_AUSF, peer);
    ogs_info("nf_name:%s,nf_instance->num_of_ipv4:%d",nf_name,nf_instance->num_of_ipv4);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);

    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NAUSF_AUTH)) {
#if 1
        nf_service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NAUSF_AUTH);
        ogs_assert(nf_service);
        ogs_sbi_nf_service_add_version(
                    nf_service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_AMF);
#endif 
    //}

    /* Setup Subscription-Data */ //TODO:得修改
    ogs_sbi_subscription_spec_add(OpenAPI_nf_type_SEPP, NULL);
    ogs_sbi_subscription_spec_add(
            OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NUDM_UEAU);

    return ;
}

void sacc_sbi_context_init_for_smf(sacc_node_t *peer){
    ogs_sbi_nf_instance_t *nf_instance = NULL;
    ogs_sbi_nf_service_t *service = NULL;

	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];
    ogs_uuid_format_custom(nf_instance_id, OpenAPI_nf_type_SMF, peer->group,peer->node, 0);

	nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    } 
    peer->smf_nf_instance = nf_instance;
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_SMF);
    ogs_sbi_nf_fsm_init(nf_instance);

    /* Build NF instance information. It will be transmitted to NRF. */
    //ogs_sbi_nf_instance_build_default(nf_instance);
    ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    snprintf(nf_name, sizeof(nf_name), "smf_%d_%d",peer->group,peer->node);
   
    sacc_sbi_context_get_nf_info(OpenAPI_nf_type_SMF, peer);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);

    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NSMF_PDUSESSION)) {
        service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NSMF_PDUSESSION);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(
                    service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(service, OpenAPI_nf_type_AMF);
    //}

    /* Setup Subscription-Data */ //TODO:可能需要
    // ogs_sbi_subscription_spec_add(OpenAPI_nf_type_SEPP, NULL);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NAMF_COMM);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NPCF_SMPOLICYCONTROL);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NUDM_SDM);
    // ogs_sbi_subscription_spec_add(
    //         OpenAPI_nf_type_NULL, OGS_SBI_SERVICE_NAME_NUDM_UECM);

    return ;
}

void sacc_sbi_context_init_for_amf(sacc_node_t *peer){
    ogs_sbi_nf_instance_t *nf_instance = NULL;
	ogs_sbi_nf_service_t *nf_service = NULL;
	
	char nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];
    char nf_name[10];
    int i;

    ogs_uuid_format_custom(nf_instance_id, OpenAPI_nf_type_AMF, peer->group,peer->node, 0);

    nf_instance = ogs_sbi_nf_instance_find((char*)nf_instance_id);
    if (!nf_instance) {
        nf_instance = ogs_sbi_nf_instance_add();
        ogs_assert(nf_instance);        

        ogs_sbi_nf_instance_set_id(
                nf_instance, (char*)nf_instance_id);
    }
    peer->amf_nf_instance = nf_instance;
    ogs_sbi_nf_instance_set_type(nf_instance, OpenAPI_nf_type_AMF);
    ogs_sbi_nf_fsm_init(nf_instance);  

    /* Build NF instance information. It will be transmitted to NRF. */
    //ogs_sbi_nf_instance_build_default(nf_instance);
    ogs_sbi_nf_instance_set_status(nf_instance, OpenAPI_nf_status_REGISTERED);
    snprintf(nf_name, sizeof(nf_name), "amf_%d_%d",peer->group,peer->node);
   
    sacc_sbi_context_get_nf_info(OpenAPI_nf_type_AMF, peer);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SCP);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_SMF);
    ogs_sbi_nf_instance_add_allowed_nf_type(nf_instance, OpenAPI_nf_type_AMF);

    /* Build NF service information. It will be transmitted to NRF. */
    //if (ogs_sbi_nf_service_is_available(OGS_SBI_SERVICE_NAME_NAMF_COMM)) {
        nf_service = sacc_sbi_nf_service_build_default(
                    nf_instance, OGS_SBI_SERVICE_NAME_NAMF_COMM);
        ogs_assert(nf_service);
        ogs_sbi_nf_service_add_version(
                    nf_service, OGS_SBI_API_V1, OGS_SBI_API_V1_0_0, NULL);
        ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_SMF);
        ogs_sbi_nf_service_add_allowed_nf_type(nf_service, OpenAPI_nf_type_AMF);

    return ;
}

ogs_sbi_nf_service_t *sacc_sbi_nf_service_build_default(
        ogs_sbi_nf_instance_t *nf_instance, const char *name)
{
    ogs_sbi_nf_service_t *nf_service = NULL;    
    int i;
    ogs_uuid_t uuid;
    char id[OGS_UUID_FORMATTED_LENGTH + 1];

    OpenAPI_uri_scheme_e scheme = OpenAPI_uri_scheme_NULL;

    ogs_assert(nf_instance);
    ogs_assert(name);

    ogs_uuid_get(&uuid);
    ogs_uuid_format(id, &uuid);

    nf_service = ogs_sbi_nf_service_add(nf_instance, id, name, OpenAPI_uri_scheme_http);
    ogs_assert(nf_service);

    char buf[OGS_ADDRSTRLEN];
	for(i = 0; i < nf_instance->num_of_ipv4; i++ )
	{
        ogs_sockaddr_t *addr = NULL;
        ogs_copyaddrinfo(&addr, nf_instance->ipv4[i]);
	    nf_service->addr[nf_service->num_of_addr].ipv4 = addr;
        nf_service->addr[nf_service->num_of_addr].is_port = true;
		nf_service->addr[nf_service->num_of_addr].port = OGS_PORT(addr);
		nf_service->num_of_addr++;
	}   

    ogs_info("NF Service [%s]", nf_service->name);

    return nf_service;
}

