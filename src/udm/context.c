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

static udm_context_t self;

int __udm_log_domain;

static OGS_POOL(udm_ue_pool, udm_ue_t);

static int context_initialized = 0;

void udm_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize UDM context */
    memset(&self, 0, sizeof(udm_context_t));

    ogs_log_install_domain(&__udm_log_domain, "udm", ogs_core()->log.level);

    ogs_pool_init(&udm_ue_pool, ogs_app()->max.ue);

    ogs_list_init(&self.udm_ue_list);
    self.suci_hash = ogs_hash_make();
    ogs_assert(self.suci_hash);
    self.supi_hash = ogs_hash_make();
    ogs_assert(self.supi_hash);

    context_initialized = 1;
}

void udm_context_final(void)
{
    ogs_assert(context_initialized == 1);

    udm_ue_remove_all();

    ogs_assert(self.suci_hash);
    ogs_hash_destroy(self.suci_hash);
    ogs_assert(self.supi_hash);
    ogs_hash_destroy(self.supi_hash);

    ogs_pool_final(&udm_ue_pool);

    context_initialized = 0;
}

udm_context_t *udm_self(void)
{
    return &self;
}

static int udm_context_prepare(void)
{
    return OGS_OK;
}

static int udm_context_validation(void)
{
    return OGS_OK;
}

int udm_context_parse_config(void)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);

    rv = udm_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "udm")) {
            ogs_yaml_iter_t udm_iter;
            ogs_yaml_iter_recurse(&root_iter, &udm_iter);
            while (ogs_yaml_iter_next(&udm_iter)) {
                const char *udm_key = ogs_yaml_iter_key(&udm_iter);
                ogs_assert(udm_key);
                if (!strcmp(udm_key, "sbi")) {
                    /* handle config in sbi library */
                } else if (!strcmp(udm_key, "service_name")) {
                    /* handle config in sbi library */
                } else if (!strcmp(udm_key, "discovery")) {
                    /* handle config in sbi library */
                } else if (!strcmp(udm_key, "metrics")) {
                    /* handle config in metrics library */
                } else if (!strcmp(udm_key, "hnet")) {
                    rv = ogs_sbi_context_parse_hnet_config(&udm_iter);
                    if (rv != OGS_OK) return rv;
                } else if (!strcmp(udm_key, "info")) {
                    ogs_warn("has udm info");
                    ogs_sbi_nf_instance_t *nf_instance = NULL;

                    ogs_yaml_iter_t info_array, info_iter;
                    ogs_yaml_iter_recurse(&udm_iter, &info_array);

                    nf_instance = ogs_sbi_self()->nf_instance;
                    ogs_assert(nf_instance);

                    do {
                        ogs_sbi_nf_info_t *nf_info = NULL;
                        ogs_sbi_udm_info_t *udm_info = NULL;

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
                                        OpenAPI_nf_type_UDM);
                        ogs_assert(nf_info);

                        udm_info = &nf_info->udm;
                        ogs_assert(udm_info);
                        
                        const char *low[OGS_MAX_NUM_OF_SUPI];
                        const char *high[OGS_MAX_NUM_OF_SUPI];
                        int i, num_of_range = 0;

                        while (ogs_yaml_iter_next(&info_iter)) {
                            const char *info_key =
                                ogs_yaml_iter_key(&info_iter);
                            ogs_assert(info_key);
                            if (!strcmp(info_key, "supi")) {
                                ogs_warn("has udm supi");
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
                                            ogs_warn("has udm range %d.",num_of_range);              
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
                                            udm_info->supiRanges.supi_ranges[i].start = ogs_strdup(low[i]);
                                            udm_info->supiRanges.supi_ranges[i].end = ogs_strdup(high[i]);
                                            ogs_warn("start %s, end %s, num %d.",low[i],high[i],num_of_range);
                                        }
                                        udm_info->supiRanges.num_of_supi_range =
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
                    ogs_warn("unknown key `%s`", udm_key);
            }
        }
    }

    rv = udm_context_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}

udm_ue_t *udm_ue_add(char *suci)
{
    udm_event_t e;
    udm_ue_t *udm_ue = NULL;

    ogs_assert(suci);

    ogs_pool_alloc(&udm_ue_pool, &udm_ue);
    ogs_assert(udm_ue);
    memset(udm_ue, 0, sizeof *udm_ue);

    udm_ue->ctx_id = ogs_msprintf("%d",
            (int)ogs_pool_index(&udm_ue_pool, udm_ue));
    ogs_assert(udm_ue->ctx_id);

    udm_ue->suci = ogs_strdup(suci);
    ogs_assert(udm_ue->suci);
    ogs_hash_set(self.suci_hash, udm_ue->suci, strlen(udm_ue->suci), udm_ue);

    udm_ue->supi = ogs_supi_from_supi_or_suci(udm_ue->suci);
    ogs_assert(udm_ue->supi);
    ogs_hash_set(self.supi_hash, udm_ue->supi, strlen(udm_ue->supi), udm_ue);

    memset(&e, 0, sizeof(e));
    e.udm_ue = udm_ue;
    ogs_fsm_init(&udm_ue->sm, udm_ue_state_initial, udm_ue_state_final, &e);

    ogs_list_add(&self.udm_ue_list, udm_ue);

    return udm_ue;
}

void udm_ue_remove(udm_ue_t *udm_ue)
{
    udm_event_t e;

    ogs_assert(udm_ue);

    ogs_list_remove(&self.udm_ue_list, udm_ue);

    memset(&e, 0, sizeof(e));
    e.udm_ue = udm_ue;
    ogs_fsm_fini(&udm_ue->sm, &e);

    /* Free SBI object memory */
    ogs_sbi_object_free(&udm_ue->sbi);

    OpenAPI_auth_event_free(udm_ue->auth_event);
    OpenAPI_amf3_gpp_access_registration_free(
            udm_ue->amf_3gpp_access_registration);

    ogs_assert(udm_ue->ctx_id);
    ogs_free(udm_ue->ctx_id);

    ogs_assert(udm_ue->suci);
    ogs_hash_set(self.suci_hash, udm_ue->suci, strlen(udm_ue->suci), NULL);
    ogs_free(udm_ue->suci);

    ogs_assert(udm_ue->supi);
    ogs_hash_set(self.supi_hash, udm_ue->supi, strlen(udm_ue->supi), NULL);
    ogs_free(udm_ue->supi);

    if (udm_ue->serving_network_name)
        ogs_free(udm_ue->serving_network_name);
    if (udm_ue->ausf_instance_id)
        ogs_free(udm_ue->ausf_instance_id);
    if (udm_ue->amf_instance_id)
        ogs_free(udm_ue->amf_instance_id);
    if (udm_ue->dereg_callback_uri)
        ogs_free(udm_ue->dereg_callback_uri);
    if (udm_ue->data_change_callback_uri)
        ogs_free(udm_ue->data_change_callback_uri);

    ogs_pool_free(&udm_ue_pool, udm_ue);
}

void udm_ue_remove_all(void)
{
    udm_ue_t *udm_ue = NULL, *next = NULL;;

    ogs_list_for_each_safe(&self.udm_ue_list, next, udm_ue)
        udm_ue_remove(udm_ue);
}

udm_ue_t *udm_ue_find_by_suci(char *suci)
{
    ogs_assert(suci);
    return (udm_ue_t *)ogs_hash_get(self.suci_hash, suci, strlen(suci));
}

udm_ue_t *udm_ue_find_by_supi(char *supi)
{
    ogs_assert(supi);
    return (udm_ue_t *)ogs_hash_get(self.supi_hash, supi, strlen(supi));
}

udm_ue_t *udm_ue_find_by_suci_or_supi(char *suci_or_supi)
{
    ogs_assert(suci_or_supi);
    if (strncmp(suci_or_supi, "suci-", strlen("suci-")) == 0)
        return udm_ue_find_by_suci(suci_or_supi);
    else
        return udm_ue_find_by_supi(suci_or_supi);
}

udm_ue_t *udm_ue_find_by_ctx_id(char *ctx_id)
{
    ogs_assert(ctx_id);
    return ogs_pool_find(&udm_ue_pool, atoll(ctx_id));
}

udm_ue_t *udm_ue_cycle(udm_ue_t *udm_ue)
{
    return ogs_pool_cycle(&udm_ue_pool, udm_ue);
}

int get_ue_load(void)
{
    return (((ogs_pool_size(&udm_ue_pool) -
            ogs_pool_avail(&udm_ue_pool)) * 100) /
            ogs_pool_size(&udm_ue_pool));
}
