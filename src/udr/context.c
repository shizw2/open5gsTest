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

static udr_context_t self;

int __udr_log_domain;

static int context_initialized = 0;

void udr_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize UDR context */
    memset(&self, 0, sizeof(udr_context_t));

    ogs_log_install_domain(&__ogs_dbi_domain, "dbi", ogs_core()->log.level);
    ogs_log_install_domain(&__udr_log_domain, "udr", ogs_core()->log.level);

    context_initialized = 1;
}

void udr_context_final(void)
{
    ogs_assert(context_initialized == 1);

    context_initialized = 0;
}

udr_context_t *udr_self(void)
{
    return &self;
}

static int udr_context_prepare(void)
{
    return OGS_OK;
}

static int udr_context_validation(void)
{
    return OGS_OK;
}

bool isCfgChanged = false;
int udr_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    rv = udr_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "udr")) {
            ogs_yaml_iter_t udr_iter;
            ogs_yaml_iter_recurse(&root_iter, &udr_iter);
            while (ogs_yaml_iter_next(&udr_iter)) {
                const char *udr_key = ogs_yaml_iter_key(&udr_iter);
                ogs_assert(udr_key);
                if (!strcmp(udr_key, "sbi")) {
                    /* handle config in sbi library */
                } else if (!strcmp(udr_key, "service_name")) {
                    /* handle config in sbi library */
                } else if (!strcmp(udr_key, "discovery")) {
                    /* handle config in sbi library */
                } else if (!strcmp(udr_key, "metrics")) {
                    /* handle config in metrics library */
                } else if (!strcmp(udr_key, "info")) { //smf,amf的info都是单独解析的，不太合适放到某个library中
                    ogs_sbi_nf_instance_t *nf_instance = NULL;

                    ogs_yaml_iter_t info_array, info_iter;
                    ogs_yaml_iter_recurse(&udr_iter, &info_array);

                    nf_instance = ogs_sbi_self()->nf_instance;
                    ogs_assert(nf_instance);

                    do {
                        ogs_sbi_nf_info_t *nf_info = NULL;
                        ogs_sbi_udr_info_t *udr_info = NULL;

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
                                        OpenAPI_nf_type_UDR);
                        
                        if (nf_info == NULL){
                            nf_info = ogs_sbi_nf_info_add(
                                        &nf_instance->nf_info_list,
                                            OpenAPI_nf_type_UDR);
                        }
                        ogs_assert(nf_info);

                        udr_info = &nf_info->udr;
                        ogs_assert(udr_info);

                        while (ogs_yaml_iter_next(&info_iter)) {
                            const char *info_key =
                                ogs_yaml_iter_key(&info_iter);
                            ogs_assert(info_key);
                            if (!strcmp(info_key, "supi")) {
                                isCfgChanged = ogs_sbi_context_parse_supi_ranges(&info_iter, &udr_info->supiRanges);
                             } else
                                ogs_warn("unknown key `%s`", info_key);
                        }

                    } while (ogs_yaml_iter_type(&info_array) ==
                            YAML_SEQUENCE_NODE);

                } else
                    ogs_warn("unknown key `%s`", udr_key);
            }
        }
    }

    rv = udr_context_validation();
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

    rv = udr_context_parse_config();
    if (rv != OGS_OK) return rv;
    
    bool needReRegister = false;
    if (ogs_app()->parameter.capacity != ogs_sbi_self()->nrf_instance->capacity){
        ogs_info("capacity changed from %d to %d.",ogs_sbi_self()->nrf_instance->capacity,ogs_app()->parameter.capacity);
        ogs_sbi_nf_instance_set_capacity(ogs_sbi_self()->nf_instance,ogs_app()->parameter.capacity);
        needReRegister = true;
    }
    
    if (isCfgChanged){
        needReRegister = true;
    }
    
    if (needReRegister){
        ogs_nnrf_nfm_send_nf_register(ogs_sbi_self()->nrf_instance);
    }
    
    return 0;
}