/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
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

#include "context.h"
#include "ogs-app-timer.h"

static nrf_context_t self;

int __nrf_log_domain;

static int context_initialized = 0;

void nrf_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize NRF context */
    memset(&self, 0, sizeof(nrf_context_t));

    ogs_log_install_domain(&__nrf_log_domain, "nrf", ogs_core()->log.level);

    context_initialized = 1;
}

void nrf_context_final(void)
{
    ogs_sbi_nf_instance_t *nf_instance = NULL, *next_nf_instance = NULL;

    ogs_assert(context_initialized == 1);

    ogs_list_for_each_safe(
            &ogs_sbi_self()->nf_instance_list, next_nf_instance, nf_instance) {
        if (NF_INSTANCE_TYPE_IS_NRF(nf_instance))
            continue;
        if (OGS_FSM_STATE(&nf_instance->sm))
            nrf_nf_fsm_fini(nf_instance);
    }

    context_initialized = 0;
}

nrf_context_t *nrf_self(void)
{
    return &self;
}

static int nrf_context_prepare(void)
{
    /* NF Instance Heartbeat
     * Default value is 10 seconds if it is not configured in nrf.yaml */
    if (!ogs_app()->time.nf_instance.heartbeat_interval)
        ogs_app()->time.nf_instance.heartbeat_interval = 10;

    return OGS_OK;
}

static int nrf_context_validation(void)
{
    return OGS_OK;
}

int nrf_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    rv = nrf_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "time")) {
            ogs_yaml_iter_t time_iter;
            ogs_yaml_iter_recurse(&root_iter, &time_iter);
            while (ogs_yaml_iter_next(&time_iter)) {
                const char *time_key = ogs_yaml_iter_key(&time_iter);
                ogs_assert(time_key);
                if (!strcmp(time_key, "nf_instance")) {
                    ogs_yaml_iter_t sbi_iter;
                    ogs_yaml_iter_recurse(&time_iter, &sbi_iter);

                    while (ogs_yaml_iter_next(&sbi_iter)) {
                        const char *sbi_key =
                            ogs_yaml_iter_key(&sbi_iter);
                        ogs_assert(sbi_key);

                        if (!strcmp(sbi_key, "heartbeat")) {
                            const char *v = ogs_yaml_iter_value(&sbi_iter);
                            if (v) ogs_app()->time.nf_instance.
                                    heartbeat_interval = atoi(v);
                        }
                    }
                }
            }
        }
    }

    rv = nrf_context_validation();
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
    
    return 0;
}