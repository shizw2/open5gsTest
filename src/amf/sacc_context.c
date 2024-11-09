#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../openapi/external/cJSON.h"
#include "sacc_context.h"
#include "ogs-sbi.h"

#define MAX_LINE_LENGTH 1024
#define MAX_TOKENS 20




// 函数用于读取一行，并将其分割成逗号分隔的值
int read_csv_line(FILE *file, char *buffer, char *tokens[], int max_tokens) {
    char *ptr = buffer;
    int token_count = 0;

    if (fgets(buffer, MAX_LINE_LENGTH, file) == NULL) {
        return -1; // 文件结束或读取错误
    }

    while (*ptr && token_count < max_tokens) {
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        
        if (*ptr == '\0') break; // 行结束

        tokens[token_count++] = ptr; // 保存当前token的起始位置

        while (*ptr && *ptr != ',') ptr++;
        
        if (*ptr == ',') {
            *ptr = '\0'; // 终止当前token
            ptr++;
        } else {
            break; // 行尾，结束解析
        }
    }

    return token_count; // 返回token数量
}

// 函数用于解析JSON字符串
void parse_json_field(char *json_str) {
    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
    } else {
        // 假设我们想要解析PlmnList字段
        cJSON *plmn_list = cJSON_GetObjectItem(json, "PlmnList");
        if (plmn_list != NULL) {
            cJSON *plmn_item = NULL;
            int i = 0;
            cJSON_ArrayForEach(plmn_item, plmn_list) {
                // 解析每个PlmnItem
                cJSON *mcc = cJSON_GetObjectItem(plmn_item, "mcc");
                cJSON *mnc = cJSON_GetObjectItem(plmn_item, "mnc");
                cJSON *tac_list = cJSON_GetObjectItem(plmn_item, "tac_list");
                printf("PlmnList[%d] MCC: %s, MNC: %s, TAC List: %s\n", i++,
                        mcc ? mcc->valuestring : "NULL",
                        mnc ? mnc->valuestring : "NULL",
                        tac_list ? tac_list->valuestring : "NULL");
            }
        }

        // 解析其他字段...
    }
    cJSON_Delete(json);
}

int get_T2_config(void) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *tokens[MAX_TOKENS];
    int num_tokens;

    // 打开CSV文件
    file = fopen("example.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // 逐行读取CSV文件
    while ((num_tokens = read_csv_line(file, line, tokens, MAX_TOKENS)) > 0) {
        // 解析字段
        // 假设JSON字段在第7列
        if (num_tokens > 7 && tokens[7] != NULL) {
            parse_json_field(tokens[7]);
        }
    }

    // 关闭文件
    fclose(file);

    return EXIT_SUCCESS;
}

void ogs_sbi_context_get_nf_addr(ogs_sbi_nf_instance_t *nf_instance, const char *local)

{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;
    ogs_sockaddr_t *addr = NULL;
	
    document = ogs_app()->document;
    ogs_assert(document);

    ogs_info("enter ogs_sbi_context_get_nf_addr: local %s",local);
    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
	    ogs_info("enter root, current root_key:%s",root_key);
        if (local && !strcmp(root_key, local)) {
			ogs_info("enter local");
            ogs_yaml_iter_t local_iter;
            ogs_yaml_iter_recurse(&root_iter, &local_iter);
            while (ogs_yaml_iter_next(&local_iter)) {
                const char *local_key = ogs_yaml_iter_key(&local_iter);
                ogs_assert(local_key);
                if (!strcmp(local_key, "sbi")) {
                    ogs_info("enter sbi");
                    ogs_yaml_iter_t sbi_array, sbi_iter;
                    ogs_yaml_iter_recurse(&local_iter, &sbi_array);
                    do {
                        int i, family = AF_UNSPEC;
                        int num = 0;
                        const char *hostname[OGS_MAX_NUM_OF_HOSTNAME];

                        uint16_t port = 7777;//self.sbi_port;

                        if (ogs_yaml_iter_type(&sbi_array) ==
                                YAML_MAPPING_NODE) {
                            memcpy(&sbi_iter, &sbi_array,
                                    sizeof(ogs_yaml_iter_t));
                        } else if (ogs_yaml_iter_type(&sbi_array) ==
                            YAML_SEQUENCE_NODE) {
                            if (!ogs_yaml_iter_next(&sbi_array))
                                break;
                            ogs_yaml_iter_recurse(&sbi_array, &sbi_iter);
                        } else if (ogs_yaml_iter_type(&sbi_array) ==
                            YAML_SCALAR_NODE) {
                            break;
                        } else
                            ogs_assert_if_reached();

                        while (ogs_yaml_iter_next(&sbi_iter)) {
                            const char *sbi_key =
                                ogs_yaml_iter_key(&sbi_iter);
                            ogs_assert(sbi_key);
                            if (!strcmp(sbi_key, "family")) {
                                const char *v = ogs_yaml_iter_value(&sbi_iter);
                                if (v) family = atoi(v);
                                if (family != AF_UNSPEC &&
                                    family != AF_INET && family != AF_INET6) {
                                    ogs_warn("Ignore family(%d) : "
                                        "AF_UNSPEC(%d), "
                                        "AF_INET(%d), AF_INET6(%d) ", 
                                        family, AF_UNSPEC, AF_INET, AF_INET6);
                                    family = AF_UNSPEC;
                                }
                            } else if (!strcmp(sbi_key, "addr") ||
                                    !strcmp(sbi_key, "name")) {
                                ogs_info("enter addr");
                                ogs_yaml_iter_t hostname_iter;
                                ogs_yaml_iter_recurse(&sbi_iter,
                                        &hostname_iter);
                                ogs_assert(ogs_yaml_iter_type(&hostname_iter) !=
                                    YAML_MAPPING_NODE);

                                do {
                                    if (ogs_yaml_iter_type(&hostname_iter) ==
                                            YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(
                                                    &hostname_iter))
                                            break;
                                    }

                                    ogs_assert(num < OGS_MAX_NUM_OF_HOSTNAME);
                                    hostname[num++] = 
                                        ogs_yaml_iter_value(&hostname_iter);
                                } while (
                                    ogs_yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE);
                            }  
							else if (!strcmp(sbi_key, "port")) {
                                const char *v = ogs_yaml_iter_value(&sbi_iter);
                                if (v)
                                    port = atoi(v);
                            }else
                                ogs_warn("unknown key `%s`", sbi_key);
                        }
                        ogs_info("num %d", num);
                        for (i = 0; i < num; i++) {
                            rv = ogs_addaddrinfo(&addr,
                                    family, hostname[i], port, 0);
                            ogs_assert(rv == OGS_OK);
							nf_instance->ipv4[nf_instance->num_of_ipv4] = addr;
                            nf_instance->num_of_ipv4++;
                        }

                    } while (ogs_yaml_iter_type(&sbi_array) ==
                            YAML_SEQUENCE_NODE);

                	}
            }
			break;
    }
    }
	return;
}

int sacc_sbi_context_get_nf_info(
        const char *local, const char *nrf, const char *scp, ogs_sbi_nf_instance_t *nf_instance)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    //rv = ogs_sbi_context_prepare();
    //if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (local && !strcmp(root_key, local)) {
            ogs_yaml_iter_t local_iter;
            ogs_yaml_iter_recurse(&root_iter, &local_iter);
            while (ogs_yaml_iter_next(&local_iter)) {
                const char *local_key = ogs_yaml_iter_key(&local_iter);
                ogs_assert(local_key);
                if (!strcmp(local_key, "sbi")) {
                    ogs_yaml_iter_t sbi_iter;
                    ogs_yaml_iter_recurse(&local_iter, &sbi_iter);
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
                                    nf_instance->ipv4[nf_instance->num_of_ipv4] = addr;
                                    nf_instance->num_of_ipv4++;
                                }

                            } while (ogs_yaml_iter_type(&server_array) == YAML_SEQUENCE_NODE);

                            if (rv != OGS_OK) {
                                ogs_error("ogs_sbi_context_parse_server_"
                                        "config() failed");
                                return rv;
                            }
                        } else if (!strcmp(sbi_key, "client")) {
                            ogs_yaml_iter_t client_iter;
                            ogs_yaml_iter_recurse(&sbi_iter, &client_iter);
                            while (ogs_yaml_iter_next(&client_iter)) {
                                const char *client_key =
                                    ogs_yaml_iter_key(&client_iter);
                                ogs_assert(client_key);
                                if (ogs_global_conf()->
                                        parameter.no_nrf == false &&
                                    nrf && !strcmp(client_key, nrf)) {
                                    ogs_yaml_iter_t nrf_array, nrf_iter;
                                    ogs_yaml_iter_recurse(&client_iter,
                                            &nrf_array);
                                    do {
                                        ogs_sbi_client_t *client = NULL;

                                        if (ogs_yaml_iter_type(&nrf_array) ==
                                                YAML_MAPPING_NODE) {
                                            memcpy(&nrf_iter, &nrf_array,
                                                    sizeof(ogs_yaml_iter_t));
                                        } else if (ogs_yaml_iter_type(
                                                    &nrf_array) ==
                                                YAML_SEQUENCE_NODE) {
                                            if (!ogs_yaml_iter_next(&nrf_array))
                                                break;
                                            ogs_yaml_iter_recurse(
                                                    &nrf_array, &nrf_iter);
                                        } else if (ogs_yaml_iter_type(
                                                    &nrf_array) ==
                                                YAML_SCALAR_NODE) {
                                            break;
                                        } else
                                            ogs_assert_if_reached();

                                        if (NF_INSTANCE_CLIENT(
                                                    nf_instance)) {
                                            ogs_error("Only one NRF client "
                                                    "can be set");
                                            return OGS_ERROR;
                                        }

                                        client =
                                            ogs_sbi_context_parse_client_config(
                                                &nrf_iter);
                                        if (!client) {
                                            ogs_error("ogs_sbi_context_parse_"
                                                    "client_config() failed");
                                            return OGS_ERROR;
                                        }

                                        OGS_SBI_SETUP_CLIENT(
                                                nf_instance, client);

                                    } while (ogs_yaml_iter_type(&nrf_array) ==
                                            YAML_SEQUENCE_NODE);
                                } else if (ogs_global_conf()->
                                        parameter.no_scp == false &&
                                        scp && !strcmp(client_key, scp)) {
                                    ogs_yaml_iter_t scp_array, scp_iter;
                                    ogs_yaml_iter_recurse(
                                            &client_iter, &scp_array);
                                    do {
                                        ogs_sbi_client_t *client = NULL;

                                        if (ogs_yaml_iter_type(&scp_array) ==
                                                YAML_MAPPING_NODE) {
                                            memcpy(&scp_iter, &scp_array,
                                                    sizeof(ogs_yaml_iter_t));
                                        } else if (ogs_yaml_iter_type(
                                                    &scp_array) ==
                                            YAML_SEQUENCE_NODE) {
                                            if (!ogs_yaml_iter_next(&scp_array))
                                                break;
                                            ogs_yaml_iter_recurse(
                                                    &scp_array, &scp_iter);
                                        } else if (ogs_yaml_iter_type(
                                                    &scp_array) ==
                                                YAML_SCALAR_NODE) {
                                            break;
                                        } else
                                            ogs_assert_if_reached();

                                        if (NF_INSTANCE_CLIENT(
                                                    nf_instance)) {
                                            ogs_error("Only one SCP client "
                                                    "can be set");
                                            return OGS_ERROR;
                                        }

                                        client =
                                            ogs_sbi_context_parse_client_config(
                                                &scp_iter);
                                        if (!client) {
                                            ogs_error("ogs_sbi_context_parse_"
                                                    "client_config() failed");
                                            return OGS_ERROR;
                                        }

                                        OGS_SBI_SETUP_CLIENT(
                                                nf_instance, client);

                                    } while (ogs_yaml_iter_type(&scp_array) ==
                                            YAML_SEQUENCE_NODE);
                                }
                            }
                        } else
                            ogs_warn("unknown key `%s`", sbi_key);
                    }
                }  else if (!strcmp(local_key, "info")) { //smf,amf的info都是单独解析的，不太合适放到某个library中
                    //ogs_sbi_nf_instance_t *nf_instance = NULL;

                    ogs_yaml_iter_t info_array, info_iter;
                    ogs_yaml_iter_recurse(&local_iter, &info_array);

                    //nf_instance = ogs_sbi_self()->nf_instance;
                    //ogs_assert(nf_instance);

                    do {
                        ogs_sbi_nf_info_t *nf_info = NULL;                        
                        ogs_supi_range_t *supiRanges = NULL;
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

                        nf_info = ogs_sbi_nf_info_find(
                                    &nf_instance->nf_info_list,
                                        nf_instance->nf_type/*OpenAPI_nf_type_5G_EIR*/);
                        
                        if (nf_info == NULL){
                            nf_info = ogs_sbi_nf_info_add(
                                    &nf_instance->nf_info_list,
                                        nf_instance->nf_type/*OpenAPI_nf_type_5G_EIR*/);
                        }
                        ogs_assert(nf_info);
                        
                        switch (nf_instance->nf_type){
                            case OpenAPI_nf_type_UDM:
                                supiRanges = &nf_info->udm.supiRanges;
                                break;
                            case OpenAPI_nf_type_AUSF:
                                supiRanges = &nf_info->ausf.supiRanges;
                                break;    
                            case OpenAPI_nf_type_SMF:
                                //supiRanges = &nf_info->smf.supiRanges;
                                break;
                            default:
                                break;
                        }
                        
                        ogs_assert(supiRanges);

                        while (ogs_yaml_iter_next(&info_iter)) {
                            const char *info_key =
                                ogs_yaml_iter_key(&info_iter);
                            ogs_assert(info_key);
                            if (!strcmp(info_key, "supi")) {
                                ogs_sbi_context_parse_supi_ranges(&info_iter, supiRanges);
                            } else
                                ogs_warn("unknown key `%s`", info_key);
                        }

                    } while (ogs_yaml_iter_type(&info_array) ==
                            YAML_SEQUENCE_NODE);

                } else if (!strcmp(local_key, "service_name")) {
                    // ogs_yaml_iter_t service_name_iter;
                    // ogs_yaml_iter_recurse(&local_iter, &service_name_iter);
                    // ogs_assert(ogs_yaml_iter_type(
                    //             &service_name_iter) != YAML_MAPPING_NODE);

                    // do {
                    //     const char *v = NULL;

                    //     if (ogs_yaml_iter_type(&service_name_iter) ==
                    //             YAML_SEQUENCE_NODE) {
                    //         if (!ogs_yaml_iter_next(&service_name_iter))
                    //             break;
                    //     }

                    //     v = ogs_yaml_iter_value(&service_name_iter);
                    //     if (v && strlen(v))
                    //         self.service_name[self.num_of_service_name++] = v;

                    // } while (ogs_yaml_iter_type(
                    //             &service_name_iter) == YAML_SEQUENCE_NODE);

                } else if (!strcmp(local_key, "discovery")) {
                    // ogs_yaml_iter_t discovery_iter;
                    // ogs_yaml_iter_recurse(&local_iter, &discovery_iter);
                    // while (ogs_yaml_iter_next(&discovery_iter)) {
                    //     const char *discovery_key =
                    //         ogs_yaml_iter_key(&discovery_iter);
                    //     ogs_assert(discovery_key);
                    //     if (!strcmp(discovery_key, "delegated")) {
                    //         const char *delegated =
                    //             ogs_yaml_iter_value(&discovery_iter);
                    //         if (!strcmp(delegated, "auto"))
                    //             self.discovery_config.delegated =
                    //                 OGS_SBI_DISCOVERY_DELEGATED_AUTO;
                    //         else if (!strcmp(delegated, "yes"))
                    //             self.discovery_config.delegated =
                    //                 OGS_SBI_DISCOVERY_DELEGATED_YES;
                    //         else if (!strcmp(delegated, "no"))
                    //             self.discovery_config.delegated =
                    //                 OGS_SBI_DISCOVERY_DELEGATED_NO;
                    //         else
                    //             ogs_warn("unknown 'delegated' value `%s`",
                    //                     delegated);
                    //     } else if (!strcmp(discovery_key, "option")) {
                    //         ogs_yaml_iter_t option_iter;
                    //         ogs_yaml_iter_recurse(
                    //                 &discovery_iter, &option_iter);

                    //         while (ogs_yaml_iter_next(&option_iter)) {
                    //             const char *option_key =
                    //                 ogs_yaml_iter_key(&option_iter);
                    //             ogs_assert(option_key);

                    //             if (!strcmp(option_key, "no_service_names")) {
                    //                 self.discovery_config.no_service_names =
                    //                     ogs_yaml_iter_bool(&option_iter);
                    //             } else if (!strcmp(option_key,
                    //                     "prefer_requester_nf_instance_id")) {
                    //                 self.discovery_config.
                    //                     prefer_requester_nf_instance_id =
                    //                         ogs_yaml_iter_bool(&option_iter);
                    //             } else
                    //                 ogs_warn("unknown key `%s`", option_key);
                    //         }
                    //     } else
                    //         ogs_warn("unknown key `%s`", discovery_key);
                    // }
                }
            }
        }
    }

    // rv = ogs_sbi_context_validation(local, nrf, scp);
    // if (rv != OGS_OK) return rv;

    return OGS_OK;
}
