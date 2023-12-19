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

static ausf_context_t self;

int __ausf_log_domain;

static OGS_POOL(ausf_ue_pool, ausf_ue_t);

static int context_initialized = 0;

void ausf_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize AUSF context */
    memset(&self, 0, sizeof(ausf_context_t));

    ogs_log_install_domain(&__ausf_log_domain, "ausf", ogs_core()->log.level);

    ogs_pool_init(&ausf_ue_pool, ogs_app()->max.ue);

    ogs_list_init(&self.ausf_ue_list);
    self.suci_hash = ogs_hash_make();
    ogs_assert(self.suci_hash);
    self.supi_hash = ogs_hash_make();
    ogs_assert(self.supi_hash);

    context_initialized = 1;
}

void ausf_context_final(void)
{
    ogs_assert(context_initialized == 1);

    ausf_ue_remove_all();

    ogs_assert(self.suci_hash);
    ogs_hash_destroy(self.suci_hash);
    ogs_assert(self.supi_hash);
    ogs_hash_destroy(self.supi_hash);

    ogs_pool_final(&ausf_ue_pool);

    context_initialized = 0;
}

ausf_context_t *ausf_self(void)
{
    return &self;
}

static int ausf_context_prepare(void)
{
    return OGS_OK;
}

static int ausf_context_validation(void)
{
    return OGS_OK;
}

bool isCfgChanged = false;
int ausf_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);
    
    isCfgChanged = false;

    rv = ausf_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "ausf")) {
            ogs_yaml_iter_t ausf_iter;
            ogs_yaml_iter_recurse(&root_iter, &ausf_iter);
            while (ogs_yaml_iter_next(&ausf_iter)) {
                const char *ausf_key = ogs_yaml_iter_key(&ausf_iter);
                ogs_assert(ausf_key);
                if (!strcmp(ausf_key, "sbi")) {
                    /* handle config in sbi library */
                } else if (!strcmp(ausf_key, "service_name")) {
                    /* handle config in sbi library */
                } else if (!strcmp(ausf_key, "discovery")) {
                    /* handle config in sbi library */
                } else if (!strcmp(ausf_key, "metrics")) {
                    /* handle config in metrics library */
                } else if (!strcmp(ausf_key, "info")) { //smf,amf的info都是单独解析的，不太合适放到某个library中
                    ogs_sbi_nf_instance_t *nf_instance = NULL;

                    ogs_yaml_iter_t info_array, info_iter;
                    ogs_yaml_iter_recurse(&ausf_iter, &info_array);

                    nf_instance = ogs_sbi_self()->nf_instance;
                    ogs_assert(nf_instance);

                    do {
                        ogs_sbi_nf_info_t *nf_info = NULL;
                        ogs_sbi_ausf_info_t *ausf_info = NULL;

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
                                        OpenAPI_nf_type_AUSF);
                        
                        if (nf_info == NULL){
                            nf_info = ogs_sbi_nf_info_add(
                                        &nf_instance->nf_info_list,
                                            OpenAPI_nf_type_AUSF);
                        }
                        ogs_assert(nf_info);

                        ausf_info = &nf_info->ausf;
                        ogs_assert(ausf_info);

                        while (ogs_yaml_iter_next(&info_iter)) {
                            const char *info_key =
                                ogs_yaml_iter_key(&info_iter);
                            ogs_assert(info_key);
                            if (!strcmp(info_key, "supi")) {
                                isCfgChanged = ogs_sbi_context_parse_supi_ranges(&info_iter, &ausf_info->supiRanges); 
                            } else if (!strcmp(info_key, "routing_indicator")) {
                                ogs_yaml_iter_t routing_indicator_iter;
                                int num_of_routing_indicator = 0;
                                ogs_yaml_iter_recurse(&info_iter,
                                        &routing_indicator_iter);
                                ogs_assert(ogs_yaml_iter_type(
                                            &routing_indicator_iter) !=
                                    YAML_MAPPING_NODE);

                                do {
                                    const char *v = NULL;

                                    if (ogs_yaml_iter_type(&routing_indicator_iter) ==
                                            YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(
                                                    &routing_indicator_iter))
                                            break;
                                    }

                                    v = ogs_yaml_iter_value(&routing_indicator_iter);
                                    if (v && strlen(v) > 0) {
                                        ogs_info("new routing_indicator %s ",v);
                                        ogs_assert(num_of_routing_indicator < OGS_MAX_NUM_OF_ROUTING_INDICATOR);
                                        if (ausf_info->routing_indicators[num_of_routing_indicator] != NULL){
                                            if ( strcmp(ausf_info->routing_indicators[num_of_routing_indicator],v) != 0){
                                                ogs_info("routing_indicator changed from %s to %s.",ausf_info->routing_indicators[num_of_routing_indicator],v);
                                                isCfgChanged = true;
                                            }
                                            ogs_info("routing_indicator %s already exit.",ausf_info->routing_indicators[num_of_routing_indicator]);
                                            ogs_free(ausf_info->routing_indicators[num_of_routing_indicator]);//先释放老的
                                        }
                                        ausf_info->routing_indicators[num_of_routing_indicator] = ogs_strdup(v);
                                        num_of_routing_indicator++;                                        
                                    }
                                } while (
                                    ogs_yaml_iter_type(&routing_indicator_iter) ==
                                        YAML_SEQUENCE_NODE);
                                        
                                ausf_info->num_of_routing_indicator = num_of_routing_indicator;
                            } else
                                ogs_warn("unknown key `%s`", info_key);
                        }

                    } while (ogs_yaml_iter_type(&info_array) ==
                            YAML_SEQUENCE_NODE);

                } else
                    ogs_warn("unknown key `%s`", ausf_key);
            }
        }
    }

    rv = ausf_context_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}

ausf_ue_t *ausf_ue_add(char *suci)
{
    ausf_event_t e;
    ausf_ue_t *ausf_ue = NULL;

    ogs_assert(suci);

    ogs_pool_alloc(&ausf_ue_pool, &ausf_ue);
    ogs_assert(ausf_ue);
    memset(ausf_ue, 0, sizeof *ausf_ue);

    ausf_ue->ctx_id =
        ogs_msprintf("%d", (int)ogs_pool_index(&ausf_ue_pool, ausf_ue));
    ogs_assert(ausf_ue->ctx_id);

    ausf_ue->suci = ogs_strdup(suci);
    ogs_assert(ausf_ue->suci);
    ogs_hash_set(self.suci_hash, ausf_ue->suci, strlen(ausf_ue->suci), ausf_ue);

    memset(&e, 0, sizeof(e));
    e.ausf_ue = ausf_ue;
    ogs_fsm_init(&ausf_ue->sm, ausf_ue_state_initial, ausf_ue_state_final, &e);

    ogs_list_add(&self.ausf_ue_list, ausf_ue);

    return ausf_ue;
}

void ausf_ue_remove(ausf_ue_t *ausf_ue)
{
    ausf_event_t e;

    ogs_assert(ausf_ue);

    ogs_list_remove(&self.ausf_ue_list, ausf_ue);

    memset(&e, 0, sizeof(e));
    e.ausf_ue = ausf_ue;
    ogs_fsm_fini(&ausf_ue->sm, &e);

    /* Free SBI object memory */
    ogs_sbi_object_free(&ausf_ue->sbi);

    ogs_assert(ausf_ue->ctx_id);
    ogs_free(ausf_ue->ctx_id);

    ogs_assert(ausf_ue->suci);
    ogs_hash_set(self.suci_hash, ausf_ue->suci, strlen(ausf_ue->suci), NULL);
    ogs_free(ausf_ue->suci);

    if (ausf_ue->supi) {
        ogs_hash_set(self.supi_hash,
                ausf_ue->supi, strlen(ausf_ue->supi), NULL);
        ogs_free(ausf_ue->supi);
    }

    if (ausf_ue->auth_events_url)
        ogs_free(ausf_ue->auth_events_url);

    if (ausf_ue->serving_network_name)
        ogs_free(ausf_ue->serving_network_name);
    
    ogs_pool_free(&ausf_ue_pool, ausf_ue);
}

void ausf_ue_remove_all(void)
{
    ausf_ue_t *ausf_ue = NULL, *next = NULL;;

    ogs_list_for_each_safe(&self.ausf_ue_list, next, ausf_ue)
        ausf_ue_remove(ausf_ue);
}

ausf_ue_t *ausf_ue_find_by_suci(char *suci)
{
    ogs_assert(suci);
    return (ausf_ue_t *)ogs_hash_get(self.suci_hash, suci, strlen(suci));
}

ausf_ue_t *ausf_ue_find_by_supi(char *supi)
{
    ogs_assert(supi);
    return (ausf_ue_t *)ogs_hash_get(self.supi_hash, supi, strlen(supi));
}

ausf_ue_t *ausf_ue_find_by_suci_or_supi(char *suci_or_supi)
{
    ogs_assert(suci_or_supi);
    if (strncmp(suci_or_supi, "suci-", strlen("suci-")) == 0)
        return ausf_ue_find_by_suci(suci_or_supi);
    else
        return ausf_ue_find_by_supi(suci_or_supi);
}

ausf_ue_t *ausf_ue_find_by_ctx_id(char *ctx_id)
{
    ogs_assert(ctx_id);
    return ogs_pool_find(&ausf_ue_pool, atoll(ctx_id));
}

ausf_ue_t *ausf_ue_cycle(ausf_ue_t *ausf_ue)
{
    return ogs_pool_cycle(&ausf_ue_pool, ausf_ue);
}

int get_ue_load(void)
{
    return (((ogs_pool_size(&ausf_ue_pool) -
            ogs_pool_avail(&ausf_ue_pool)) * 100) /
            ogs_pool_size(&ausf_ue_pool));
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

    rv = ausf_context_parse_config();
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