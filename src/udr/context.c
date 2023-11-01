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

                        nf_info = ogs_sbi_nf_info_add(
                                    &nf_instance->nf_info_list,
                                        OpenAPI_nf_type_UDR);
                        ogs_assert(nf_info);

                        udr_info = &nf_info->udr;
                        ogs_assert(udr_info);
                        
                        const char *low[OGS_MAX_NUM_OF_SUPI];
                        const char *high[OGS_MAX_NUM_OF_SUPI];
                        int i, num_of_range = 0;

                        while (ogs_yaml_iter_next(&info_iter)) {
                            const char *info_key =
                                ogs_yaml_iter_key(&info_iter);
                            ogs_assert(info_key);
                            if (!strcmp(info_key, "supi")) {
                                ogs_yaml_iter_t supi_array, supi_iter;
                                ogs_yaml_iter_recurse(&info_iter, &supi_array);
                                do {                                  
                                    if (ogs_yaml_iter_type(&supi_array) ==
                                            YAML_MAPPING_NODE) {
                                        memcpy(&supi_iter, &supi_array,
                                                sizeof(ogs_yaml_iter_t));
                                    } else if (ogs_yaml_iter_type(&supi_array) ==
                                            YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(&supi_array))
                                            break;
                                        ogs_yaml_iter_recurse(&supi_array,
                                                &supi_iter);
                                    } else if (ogs_yaml_iter_type(&supi_array) ==
                                            YAML_SCALAR_NODE) {
                                        break;
                                    } else
                                        ogs_assert_if_reached();

                                    while (ogs_yaml_iter_next(&supi_iter)) {
                                        const char *tai_key =
                                            ogs_yaml_iter_key(&supi_iter);
                                        ogs_assert(tai_key);
                                        if (!strcmp(tai_key, "range")) {
                                            ogs_yaml_iter_t range_iter;
                                            ogs_yaml_iter_recurse(
                                                    &supi_iter, &range_iter);
                                            ogs_assert(ogs_yaml_iter_type(&range_iter) !=
                                                YAML_MAPPING_NODE);
                                            do {
                                                char *v = NULL;

                                                if (ogs_yaml_iter_type(&range_iter) ==
                                                        YAML_SEQUENCE_NODE) {
                                                    if (!ogs_yaml_iter_next(&range_iter))
                                                        break;
                                                }

                                                v = (char *)
                                                    ogs_yaml_iter_value(&range_iter);
                                                if (v) {
                                                    ogs_assert(num_of_range <
                                                            OGS_MAX_NUM_OF_SUPI);
                                                    low[num_of_range] =
                                                        (const char *)strsep(&v, "-");
                                                    if (low[num_of_range] && strlen(low[num_of_range]) == 0)
                                                        low[num_of_range] = NULL;

                                                    high[num_of_range] = (const char *)v;
                                                    if (high[num_of_range] && strlen(high[num_of_range]) == 0)
                                                        high[num_of_range] = NULL;
                                                }

                                                if (low[num_of_range] || high[num_of_range]) {
                                                    num_of_range++;
                                                    ogs_warn("num_of_range:%d.",num_of_range);                                                    
                                                }
                                            } while (
                                                ogs_yaml_iter_type(&range_iter) ==
                                                YAML_SEQUENCE_NODE);
                                        } else
                                            ogs_warn("unknown key `%s`",
                                                    tai_key);
                                    }                                   
                                    
                                } while (ogs_yaml_iter_type(&supi_array) ==
                                        YAML_SEQUENCE_NODE);

                                if (num_of_range) {
                                        int i;
                              
                                        for (i = 0; i < num_of_range; i++) {                     
                                            udr_info->supiRanges.supi_ranges[i].start = ogs_strdup(low[i]);
                                            udr_info->supiRanges.supi_ranges[i].end = ogs_strdup(high[i]);
                                            ogs_warn("start %s, end %s, num %d.",low[i],high[i],num_of_range);
                                        }
                                        udr_info->supiRanges.num_of_supi_range =
                                                    num_of_range;
                                                    
                                } else {
                                    ogs_warn("No supi range info");
                                }        

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
