/*
 * Copyright (C) 2019-2023 by Sukchan Lee <acetcom@gmail.com>
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

static pcf_context_t self;

int __pcf_log_domain;

static OGS_POOL(pcf_ue_pool, pcf_ue_t);
static OGS_POOL(pcf_sess_pool, pcf_sess_t);
static OGS_POOL(pcf_app_pool, pcf_app_t);

static int context_initialized = 0;

static void clear_ipv4addr(pcf_sess_t *sess);
static void clear_ipv6prefix(pcf_sess_t *sess);

static ogs_diam_config_t g_diam_conf;

void pcf_context_init(void)
{
    ogs_assert(context_initialized == 0);

    /* Initialize PCF context */
    memset(&self, 0, sizeof(pcf_context_t));

    /* Initial FreeDiameter Config */
    memset(&g_diam_conf, 0, sizeof(ogs_diam_config_t));
    self.diam_config = &g_diam_conf;

    ogs_log_install_domain(&__ogs_diam_domain, "diam", ogs_core()->log.level);
    ogs_log_install_domain(&__ogs_dbi_domain, "dbi", ogs_core()->log.level);
    ogs_log_install_domain(&__pcf_log_domain, "pcf", ogs_core()->log.level);

    ogs_pool_init(&pcf_ue_pool, ogs_global_conf()->max.ue);
    ogs_pool_init(&pcf_sess_pool, ogs_app()->pool.sess);
    ogs_pool_init(&pcf_app_pool, ogs_app()->pool.sess);

    ogs_list_init(&self.pcf_ue_list);

    self.supi_hash = ogs_hash_make();
    ogs_assert(self.supi_hash);
    self.ipv4addr_hash = ogs_hash_make();
    ogs_assert(self.ipv4addr_hash);
    self.ipv6prefix_hash = ogs_hash_make();
    ogs_assert(self.ipv6prefix_hash);

    context_initialized = 1;
}

void pcf_context_final(void)
{
    ogs_assert(context_initialized == 1);

    pcf_ue_remove_all();

    ogs_assert(self.supi_hash);
    ogs_hash_destroy(self.supi_hash);
    ogs_assert(self.ipv4addr_hash);
    ogs_hash_destroy(self.ipv4addr_hash);
    ogs_assert(self.ipv6prefix_hash);
    ogs_hash_destroy(self.ipv6prefix_hash);

    ogs_pool_final(&pcf_app_pool);
    ogs_pool_final(&pcf_sess_pool);
    ogs_pool_final(&pcf_ue_pool);

    context_initialized = 0;
}

pcf_context_t *pcf_self(void)
{
    return &self;
}

static int pcf_context_prepare(void)
{
    return OGS_OK;
}

static int pcf_context_validation(void)
{
    return OGS_OK;
}

static int slice_conf_prepare(void)
{
    return OGS_OK;
}

static int slice_conf_validation(void)
{
    return OGS_OK;
}

static int parse_slice_conf(
        ogs_yaml_iter_t *parent, ogs_app_policy_conf_t *policy_conf)
{
    int rv;
    ogs_yaml_iter_t slice_array, slice_iter;

    ogs_assert(parent);
    ogs_assert(policy_conf);

    rv = slice_conf_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_recurse(parent, &slice_array);
    do {
        ogs_app_slice_conf_t *slice_conf = NULL;
        ogs_s_nssai_t s_nssai;
        bool default_indicator = false;

        s_nssai.sst = 0;
        s_nssai.sd.v = OGS_S_NSSAI_NO_SD_VALUE;

        OGS_YAML_ARRAY_NEXT(&slice_array, &slice_iter);
        while (ogs_yaml_iter_next(&slice_iter)) {
            const char *slice_key = ogs_yaml_iter_key(&slice_iter);
            ogs_assert(slice_key);
            if (!strcmp(slice_key, OGS_SST_STRING)) {
                const char *v = ogs_yaml_iter_value(&slice_iter);
                if (v) {
                    s_nssai.sst = atoi(v);
                    if (s_nssai.sst == 1 || s_nssai.sst == 2 ||
                            s_nssai.sst == 3 || s_nssai.sst == 4) {
                    } else {
                        ogs_error("Unknown SST [%d]", s_nssai.sst);
                        return OGS_ERROR;
                    }
                }
            } else if (!strcmp(slice_key, OGS_SD_STRING)) {
                const char *v = ogs_yaml_iter_value(&slice_iter);
                if (v) s_nssai.sd = ogs_s_nssai_sd_from_string(v);
            } else if (!strcmp(slice_key, OGS_DEFAULT_INDICATOR_STRING)) {
                default_indicator = ogs_yaml_iter_bool(&slice_iter);
            }
        }

        if (s_nssai.sst) {
            slice_conf = ogs_app_slice_conf_add(policy_conf, &s_nssai);
            if (!slice_conf) {
                ogs_error("ogs_app_slice_conf_add() failed [SST:%d,SD:0x%x]",
                        s_nssai.sst, s_nssai.sd.v);
                return OGS_ERROR;
            }
            slice_conf->data.default_indicator = default_indicator;
        } else {
            ogs_error("No SST");
            return OGS_ERROR;
        }

        OGS_YAML_ARRAY_RECURSE(&slice_array, &slice_iter);
        while (ogs_yaml_iter_next(&slice_iter)) {
            const char *slice_key = ogs_yaml_iter_key(&slice_iter);
            ogs_assert(slice_key);
            if (!strcmp(slice_key, OGS_SESSION_STRING)) {
                rv = ogs_app_parse_session_conf(&slice_iter, slice_conf);
                if (rv != OGS_OK) {
                    ogs_error("parse_session_conf() failed");
                    return rv;
                }
            }
        }

    } while (ogs_yaml_iter_type(&slice_array) == YAML_SEQUENCE_NODE);

    rv = slice_conf_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}

static int policy_conf_prepare(void)
{
    return OGS_OK;
}

static int policy_conf_validation(void)
{
    int rv;

    rv = ogs_app_check_policy_conf();
    if (rv != OGS_OK) {
        ogs_error("ogs_app_check_policy_conf() failed");
        return OGS_ERROR;
    }

    return OGS_OK;
}

static int parse_policy_conf(ogs_yaml_iter_t *parent)
{
    int rv;
    ogs_yaml_iter_t policy_array, policy_iter;

    ogs_assert(parent);

    rv = policy_conf_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_recurse(parent, &policy_array);
    do {
        const char *mnc = NULL, *mcc = NULL;
        ogs_app_policy_conf_t *policy_conf = NULL;

        OGS_YAML_ARRAY_NEXT(&policy_array, &policy_iter);
        while (ogs_yaml_iter_next(&policy_iter)) {
            const char *policy_key = ogs_yaml_iter_key(&policy_iter);
            ogs_assert(policy_key);
            if (!strcmp(policy_key, "plmn_id")) {
                ogs_yaml_iter_t plmn_id_iter;

                ogs_yaml_iter_recurse(&policy_iter, &plmn_id_iter);
                while (ogs_yaml_iter_next(&plmn_id_iter)) {
                    const char *id_key = ogs_yaml_iter_key(&plmn_id_iter);
                    ogs_assert(id_key);
                    if (!strcmp(id_key, "mcc")) {
                        mcc = ogs_yaml_iter_value(&plmn_id_iter);
                    } else if (!strcmp(id_key, "mnc")) {
                        mnc = ogs_yaml_iter_value(&plmn_id_iter);
                    }
                }

            }
        }

        if (mcc && mnc) {
            ogs_plmn_id_t plmn_id;
            ogs_plmn_id_build(&plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
            policy_conf = ogs_app_policy_conf_add(&plmn_id);
            if (!policy_conf) {
                ogs_error("ogs_app_policy_conf_add() failed "
                        "[MCC:%s,MNC:%s]", mcc, mnc);
                return OGS_ERROR;
            }
        } else {
            ogs_error("No PLMN-ID [MCC:%s, MNC:%s]", mcc, mnc);
            return OGS_ERROR;
        }

        OGS_YAML_ARRAY_RECURSE(&policy_array, &policy_iter);
        while (ogs_yaml_iter_next(&policy_iter)) {
            const char *policy_key = ogs_yaml_iter_key(&policy_iter);
            ogs_assert(policy_key);
            if (!strcmp(policy_key, OGS_SLICE_STRING)) {
                rv = parse_slice_conf(&policy_iter, policy_conf);
                if (rv != OGS_OK) {
                    ogs_error("parse_slice_conf() failed");
                    return rv;
                }
            }
        }

    } while (ogs_yaml_iter_type(&policy_array) == YAML_SEQUENCE_NODE);

    rv = policy_conf_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}
bool is_nfinfo_changed = false;
int pcf_context_parse_config(bool reloading)
{
    int rv;
    yaml_document_t *document = NULL;
    ogs_yaml_iter_t root_iter;

    document = ogs_app()->document;
    ogs_assert(document);
    
    is_nfinfo_changed = false;

    rv = pcf_context_prepare();
    if (rv != OGS_OK) return rv;

    ogs_yaml_iter_init(&root_iter, document);
    while (ogs_yaml_iter_next(&root_iter)) {
        const char *root_key = ogs_yaml_iter_key(&root_iter);
        ogs_assert(root_key);
        if (!strcmp(root_key, "pcf")) {
            ogs_yaml_iter_t pcf_iter;
            ogs_yaml_iter_recurse(&root_iter, &pcf_iter);
            while (ogs_yaml_iter_next(&pcf_iter)) {
                const char *pcf_key = ogs_yaml_iter_key(&pcf_iter);
                ogs_assert(pcf_key);
                if (!strcmp(pcf_key, "default")) {
                    /* handle config in sbi library */
                } else if (!strcmp(pcf_key, "sbi")) {
                    /* handle config in sbi library */
                } else if (!strcmp(pcf_key, "nrf")) {
                    /* handle config in sbi library */
                } else if (!strcmp(pcf_key, "scp")) {
                    /* handle config in sbi library */
                } else if (!strcmp(pcf_key, "service_name")) {
                    /* handle config in sbi library */
                } else if (!strcmp(pcf_key, "discovery")) {
                    /* handle config in sbi library */
                } else if (!strcmp(pcf_key, "metrics")) {
                    /* handle config in metrics library */
                } else if (!strcmp(pcf_key, OGS_POLICY_STRING)) {
                    rv = parse_policy_conf(&pcf_iter);
                    if (rv != OGS_OK) {
                        ogs_error("parse_policy_conf() failed");
                        return rv;
                    }
                } else if (!strcmp(pcf_key, "freeDiameter")) {
                    if (reloading == true){//重新加载时不读取
                        continue;
                    }
                    yaml_node_t *node =
                        yaml_document_get_node(document, pcf_iter.pair->value);
                    ogs_assert(node);
                    if (node->type == YAML_SCALAR_NODE) {
                        self.diam_conf_path = ogs_yaml_iter_value(&pcf_iter);
                    } else if (node->type == YAML_MAPPING_NODE) {
                        ogs_yaml_iter_t fd_iter;
                        ogs_yaml_iter_recurse(&pcf_iter, &fd_iter);

                        while (ogs_yaml_iter_next(&fd_iter)) {
                            const char *fd_key = ogs_yaml_iter_key(&fd_iter);
                            ogs_assert(fd_key);
                            if (!strcmp(fd_key, "identity")) {
                                self.diam_config->cnf_diamid =
                                    ogs_yaml_iter_value(&fd_iter);
                            } else if (!strcmp(fd_key, "realm")) {
                                self.diam_config->cnf_diamrlm =
                                    ogs_yaml_iter_value(&fd_iter);
                            } else if (!strcmp(fd_key, "port")) {
                                const char *v = ogs_yaml_iter_value(&fd_iter);
                                if (v) self.diam_config->cnf_port = atoi(v);
                            } else if (!strcmp(fd_key, "sec_port")) {
                                const char *v = ogs_yaml_iter_value(&fd_iter);
                                if (v) self.diam_config->cnf_port_tls = atoi(v);
                            } else if (!strcmp(fd_key, "listen_on")) {
                                self.diam_config->cnf_addr =
                                    ogs_yaml_iter_value(&fd_iter);
                            } else if (!strcmp(fd_key, "no_fwd")) {
                                self.diam_config->cnf_flags.no_fwd =
                                    ogs_yaml_iter_bool(&fd_iter);
                            } else if (!strcmp(fd_key, "load_extension")) {
                                ogs_yaml_iter_t ext_array, ext_iter;
                                ogs_yaml_iter_recurse(&fd_iter, &ext_array);
                                do {
                                    const char *module = NULL;
                                    const char *conf = NULL;

                                    if (ogs_yaml_iter_type(&ext_array) ==
                                        YAML_MAPPING_NODE) {
                                        memcpy(&ext_iter, &ext_array,
                                                sizeof(ogs_yaml_iter_t));
                                    } else if (ogs_yaml_iter_type(&ext_array) ==
                                        YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(&ext_array))
                                            break;
                                        ogs_yaml_iter_recurse(
                                                &ext_array, &ext_iter);
                                    } else if (ogs_yaml_iter_type(&ext_array) ==
                                        YAML_SCALAR_NODE) {
                                        break;
                                    } else
                                        ogs_assert_if_reached();

                                    while (ogs_yaml_iter_next(&ext_iter)) {
                                        const char *ext_key =
                                            ogs_yaml_iter_key(&ext_iter);
                                        ogs_assert(ext_key);
                                        if (!strcmp(ext_key, "module"))
                                        {
                                            module = ogs_yaml_iter_value(
                                                    &ext_iter);
                                        } else if (!strcmp(ext_key, "conf")) {
                                            conf = ogs_yaml_iter_value(
                                                    &ext_iter);
                                        } else
                                            ogs_warn("unknown key `%s`",
                                                    ext_key);
                                    }

                                    if (module) {
                                        self.diam_config->
                                            ext[self.diam_config->num_of_ext].
                                                module = module;
                                        self.diam_config->
                                            ext[self.diam_config->num_of_ext].
                                                conf = conf;
                                        self.diam_config->num_of_ext++;
                                    }
                                } while(ogs_yaml_iter_type(&ext_array) ==
                                        YAML_SEQUENCE_NODE);
                            } else if (!strcmp(fd_key, "connect")) {
                                ogs_yaml_iter_t conn_array, conn_iter;
                                ogs_yaml_iter_recurse(&fd_iter, &conn_array);
                                do {
                                    const char *identity = NULL;
                                    const char *addr = NULL;
                                    uint16_t port = 0;

                                    if (ogs_yaml_iter_type(&conn_array) ==
                                        YAML_MAPPING_NODE) {
                                        memcpy(&conn_iter, &conn_array,
                                                sizeof(ogs_yaml_iter_t));
                                    } else if (ogs_yaml_iter_type(&conn_array)
                                            == YAML_SEQUENCE_NODE) {
                                        if (!ogs_yaml_iter_next(&conn_array))
                                            break;
                                        ogs_yaml_iter_recurse(
                                                &conn_array, &conn_iter);
                                    } else if (ogs_yaml_iter_type(&conn_array)
                                            == YAML_SCALAR_NODE) {
                                        break;
                                    } else
                                        ogs_assert_if_reached();

                                    while (ogs_yaml_iter_next(&conn_iter)) {
                                        const char *conn_key =
                                            ogs_yaml_iter_key(&conn_iter);
                                        ogs_assert(conn_key);
                                        if (!strcmp(conn_key, "identity")) {
                                            identity = ogs_yaml_iter_value(
                                                    &conn_iter);
                                        } else if (!strcmp(conn_key, "addr")) {
                                            addr = ogs_yaml_iter_value(
                                                    &conn_iter);
                                        } else if (!strcmp(conn_key, "port")) {
                                            const char *v =
                                                ogs_yaml_iter_value(&conn_iter);
                                            if (v) port = atoi(v);
                                        } else
                                            ogs_warn("unknown key `%s`",
                                                    conn_key);
                                    }

                                    if (identity && addr) {
                                        self.diam_config->
                                            conn[self.diam_config->num_of_conn].
                                                identity = identity;
                                        self.diam_config->
                                            conn[self.diam_config->num_of_conn].
                                                addr = addr;
                                        self.diam_config->
                                            conn[self.diam_config->num_of_conn].
                                                port = port;
                                        self.diam_config->num_of_conn++;
                                    }
                                } while (ogs_yaml_iter_type(&conn_array) ==
                                        YAML_SEQUENCE_NODE);
                            } else
                                ogs_warn("unknown key `%s`", fd_key);
                        }
                    }
                } else if (!strcmp(pcf_key, "info")) { //smf,amf的info都是单独解析的，不太合适放到某个library中
                    ogs_sbi_nf_instance_t *nf_instance = NULL;

                    ogs_yaml_iter_t info_array, info_iter;
                    ogs_yaml_iter_recurse(&pcf_iter, &info_array);

                    nf_instance = ogs_sbi_self()->nf_instance;
                    ogs_assert(nf_instance);

                    do {
                        ogs_sbi_nf_info_t *nf_info = NULL;
                        ogs_sbi_pcf_info_t *pcf_info = NULL;

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
                                        OpenAPI_nf_type_PCF);
                        
                        if (nf_info == NULL){
                            nf_info = ogs_sbi_nf_info_add(
                                        &nf_instance->nf_info_list,
                                            OpenAPI_nf_type_PCF);
                        }
                        ogs_assert(nf_info);

                        pcf_info = &nf_info->pcf;
                        ogs_assert(pcf_info);

                        while (ogs_yaml_iter_next(&info_iter)) {
                            const char *info_key =
                                ogs_yaml_iter_key(&info_iter);
                            ogs_assert(info_key);
                            if (!strcmp(info_key, "supi")) {
                                is_nfinfo_changed = ogs_sbi_context_parse_supi_ranges(&info_iter, &pcf_info->supiRanges); 
                            } else
                                ogs_warn("unknown key `%s`", info_key);
                        }

                    } while (ogs_yaml_iter_type(&info_array) ==
                            YAML_SEQUENCE_NODE);

                } 
                else
                    ogs_warn("unknown key `%s`", pcf_key);
            }
        }
    }

    rv = pcf_context_validation();
    if (rv != OGS_OK) return rv;

    return OGS_OK;
}

pcf_ue_t *pcf_ue_add(char *supi)
{
    pcf_event_t e;
    pcf_ue_t *pcf_ue = NULL;

    ogs_assert(supi);

    ogs_pool_id_calloc(&pcf_ue_pool, &pcf_ue);
    ogs_assert(pcf_ue);

    /* SBI Type */
    pcf_ue->sbi.type = OGS_SBI_OBJ_UE_TYPE;

    /* SBI Features */
    OGS_SBI_FEATURES_SET(pcf_ue->am_policy_control_features,
            OGS_SBI_NPCF_AM_POLICY_CONTROL_UE_AMBR_AUTHORIZATION);

    pcf_ue->association_id = ogs_msprintf("%d",
            (int)ogs_pool_index(&pcf_ue_pool, pcf_ue));
    ogs_assert(pcf_ue->association_id);

    pcf_ue->supi = ogs_strdup(supi);
    ogs_assert(pcf_ue->supi);
    ogs_hash_set(self.supi_hash, pcf_ue->supi, strlen(pcf_ue->supi), pcf_ue);

    memset(&e, 0, sizeof(e));
    e.pcf_ue_id = pcf_ue->id;
    ogs_fsm_init(&pcf_ue->sm, pcf_am_state_initial, pcf_am_state_final, &e);

    ogs_list_add(&self.pcf_ue_list, pcf_ue);

    return pcf_ue;
}

void pcf_ue_remove(pcf_ue_t *pcf_ue)
{
    pcf_event_t e;

    ogs_assert(pcf_ue);

    ogs_list_remove(&self.pcf_ue_list, pcf_ue);

    memset(&e, 0, sizeof(e));
    e.pcf_ue_id = pcf_ue->id;
    ogs_fsm_fini(&pcf_ue->sm, &e);

    /* Free SBI object memory */
    if (ogs_list_count(&pcf_ue->sbi.xact_list))
        ogs_error("UE transaction [%d]",
                ogs_list_count(&pcf_ue->sbi.xact_list));
    ogs_sbi_object_free(&pcf_ue->sbi);

    pcf_sess_remove_all(pcf_ue);

    OpenAPI_policy_association_request_free(pcf_ue->policy_association_request);
    if (pcf_ue->subscribed_ue_ambr)
        OpenAPI_ambr_free(pcf_ue->subscribed_ue_ambr);

    ogs_assert(pcf_ue->association_id);
    ogs_free(pcf_ue->association_id);

    ogs_assert(pcf_ue->supi);
    ogs_hash_set(self.supi_hash, pcf_ue->supi, strlen(pcf_ue->supi), NULL);
    ogs_free(pcf_ue->supi);

    if (pcf_ue->notification_uri)
        ogs_free(pcf_ue->notification_uri);
    if (pcf_ue->namf.client)
        ogs_sbi_client_remove(pcf_ue->namf.client);

    if (pcf_ue->gpsi)
        ogs_free(pcf_ue->gpsi);
    if (pcf_ue->pei)
        ogs_free(pcf_ue->pei);

    ogs_pool_id_free(&pcf_ue_pool, pcf_ue);
}

void pcf_ue_remove_all(void)
{
    pcf_ue_t *pcf_ue = NULL, *next = NULL;;

    ogs_list_for_each_safe(&self.pcf_ue_list, next, pcf_ue)
        pcf_ue_remove(pcf_ue);
}

pcf_ue_t *pcf_ue_find_by_supi(char *supi)
{
    ogs_assert(supi);
    return (pcf_ue_t *)ogs_hash_get(self.supi_hash, supi, strlen(supi));
}

pcf_ue_t *pcf_ue_find_by_association_id(char *association_id)
{
    ogs_assert(association_id);
    return ogs_pool_find(&pcf_ue_pool, atoll(association_id));
}

pcf_sess_t *pcf_sess_add(pcf_ue_t *pcf_ue, uint8_t psi)
{
    pcf_event_t e;
    pcf_sess_t *sess = NULL;

    ogs_assert(pcf_ue);
    ogs_assert(psi != OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED);

    ogs_pool_id_calloc(&pcf_sess_pool, &sess);
    ogs_assert(sess);

    ogs_list_init(&sess->app_list);

    /* SBI Type */
    sess->sbi.type = OGS_SBI_OBJ_SESS_TYPE;

    /* SBI Features */
    OGS_SBI_FEATURES_SET(sess->smpolicycontrol_features,
            OGS_SBI_NPCF_SMPOLICYCONTROL_DN_AUTHORIZATION);

    OGS_SBI_FEATURES_SET(sess->management_features,
            OGS_SBI_NBSF_MANAGEMENT_BINDING_UPDATE);

    OGS_SBI_FEATURES_SET(sess->policyauthorization_features,
            OGS_SBI_NPCF_POLICYAUTHORIZATION_SPONSORED_CONNECTIVITY);
    OGS_SBI_FEATURES_SET(sess->policyauthorization_features,
            OGS_SBI_NPCF_POLICYAUTHORIZATION_IMS_SBI);

    sess->sm_policy_id = ogs_msprintf("%d",
            (int)ogs_pool_index(&pcf_sess_pool, sess));
    ogs_assert(sess->sm_policy_id);

    sess->pcf_ue_id = pcf_ue->id;
    sess->psi = psi;

    sess->s_nssai.sst = 0;
    sess->s_nssai.sd.v = OGS_S_NSSAI_NO_SD_VALUE;

    memset(&e, 0, sizeof(e));
    e.sess_id = sess->id;
    ogs_fsm_init(&sess->sm, pcf_sm_state_initial, pcf_sm_state_final, &e);

    ogs_list_add(&pcf_ue->sess_list, sess);

    return sess;
}

void pcf_sess_remove(pcf_sess_t *sess)
{
    pcf_event_t e;
    pcf_ue_t *pcf_ue = NULL;

    ogs_assert(sess);
    pcf_ue = pcf_ue_find_by_id(sess->pcf_ue_id);
    ogs_assert(pcf_ue);

    ogs_list_remove(&pcf_ue->sess_list, sess);

    memset(&e, 0, sizeof(e));
    e.sess_id = sess->id;
    ogs_fsm_fini(&sess->sm, &e);

    /* Free SBI object memory */
    if (ogs_list_count(&sess->sbi.xact_list))
        ogs_error("Session transaction [%d]",
                ogs_list_count(&sess->sbi.xact_list));
    ogs_sbi_object_free(&sess->sbi);

    pcf_app_remove_all(sess);

    ogs_assert(sess->sm_policy_id);
    ogs_free(sess->sm_policy_id);

    PCF_BINDING_CLEAR(sess);
    if (sess->binding.client)
        ogs_sbi_client_remove(sess->binding.client);

    if (sess->dnn)
        ogs_free(sess->dnn);
    if (sess->full_dnn)
        ogs_free(sess->full_dnn);

    if (sess->notification_uri)
        ogs_free(sess->notification_uri);
    if (sess->nsmf.client)
        ogs_sbi_client_remove(sess->nsmf.client);

    clear_ipv4addr(sess);
    clear_ipv6prefix(sess);

    OpenAPI_clear_and_free_string_list(sess->ipv4_frame_route_list);
    OpenAPI_clear_and_free_string_list(sess->ipv6_frame_route_list);

    if (sess->subscribed_sess_ambr)
        OpenAPI_ambr_free(sess->subscribed_sess_ambr);
    if (sess->subscribed_default_qos)
        OpenAPI_subscribed_default_qos_free(sess->subscribed_default_qos);

    ogs_pool_id_free(&pcf_sess_pool, sess);
}

void pcf_sess_remove_all(pcf_ue_t *pcf_ue)
{
    pcf_sess_t *sess = NULL, *next_sess = NULL;

    ogs_assert(pcf_ue);

    ogs_list_for_each_safe(&pcf_ue->sess_list, next_sess, sess)
        pcf_sess_remove(sess);
}

static void clear_ipv4addr(pcf_sess_t *sess)
{
    ogs_assert(sess);

    if (sess->ipv4addr_string) {
        ogs_hash_set(self.ipv4addr_hash,
                &sess->ipv4addr, sizeof(sess->ipv4addr), NULL);
        ogs_free(sess->ipv4addr_string);
    }
}

static void clear_ipv6prefix(pcf_sess_t *sess)
{
    ogs_assert(sess);

    if (sess->ipv6prefix_string) {
        ogs_hash_set(self.ipv6prefix_hash,
                &sess->ipv6prefix, (sess->ipv6prefix.len >> 3) + 1, NULL);
        ogs_free(sess->ipv6prefix_string);
    }
}

bool pcf_sess_set_ipv4addr(pcf_sess_t *sess, char *ipv4addr_string)
{
    int rv;

    ogs_assert(sess);
    ogs_assert(ipv4addr_string);

    clear_ipv4addr(sess);

    rv = ogs_ipv4_from_string(&sess->ipv4addr, ipv4addr_string);
    if (rv != OGS_OK) {
        ogs_error("ogs_ipv4_from_string() failed");
        return false;
    }

    sess->ipv4addr_string = ogs_strdup(ipv4addr_string);
    if (!sess->ipv4addr_string) {
        ogs_error("ogs_strdup() failed");
        return false;
    }

    ogs_hash_set(self.ipv4addr_hash,
            &sess->ipv4addr, sizeof(sess->ipv4addr), sess);

    return true;
}

bool pcf_sess_set_ipv6prefix(pcf_sess_t *sess, char *ipv6prefix_string)
{
    int rv;

    ogs_assert(sess);
    ogs_assert(ipv6prefix_string);

    clear_ipv6prefix(sess);

    rv = ogs_ipv6prefix_from_string(
            sess->ipv6prefix.addr6, &sess->ipv6prefix.len, ipv6prefix_string);
    if (rv != OGS_OK) {
        ogs_error("ogs_ipv6prefix_from_string() failed");
        return false;
    }

    ogs_assert(sess->ipv6prefix.len == OGS_IPV6_128_PREFIX_LEN);

    sess->ipv6prefix_string = ogs_strdup(ipv6prefix_string);
    if (!sess->ipv6prefix_string) {
        ogs_error("ogs_strdup() failed");
        return false;
    }

    ogs_hash_set(self.ipv6prefix_hash,
            &sess->ipv6prefix, (sess->ipv6prefix.len >> 3) + 1, sess);

    return true;
}

pcf_sess_t *pcf_sess_find(uint32_t index)
{
    return ogs_pool_find(&pcf_sess_pool, index);
}

pcf_sess_t *pcf_sess_find_by_sm_policy_id(char *sm_policy_id)
{
    ogs_assert(sm_policy_id);
    return pcf_sess_find(atoll(sm_policy_id));
}

pcf_sess_t *pcf_sess_find_by_psi(pcf_ue_t *pcf_ue, uint8_t psi)
{
    pcf_sess_t *sess = NULL;

    ogs_list_for_each(&pcf_ue->sess_list, sess)
        if (psi == sess->psi) return sess;

    return NULL;
}

pcf_sess_t *pcf_sess_find_by_ipv4addr(char *ipv4addr_string)
{
    uint32_t ipv4addr;
    int rv;

    ogs_assert(ipv4addr_string);

    rv = ogs_ipv4_from_string(&ipv4addr, ipv4addr_string);
    if (rv != OGS_OK) {
        ogs_error("ogs_ipv4_from_string() failed");
        return NULL;
    }

    return ogs_hash_get(self.ipv4addr_hash, &ipv4addr, sizeof(ipv4addr));
}

int pcf_sessions_number_by_snssai_and_dnn(
        pcf_ue_t *pcf_ue, ogs_s_nssai_t *s_nssai, char *dnn)
{
    int number_of_sessions = 0;
    pcf_sess_t *sess = NULL;

    ogs_assert(s_nssai);
    ogs_assert(dnn);

    ogs_list_for_each(&pcf_ue->sess_list, sess)
        if (sess->s_nssai.sst == s_nssai->sst &&
            sess->dnn && ogs_strcasecmp(sess->dnn, dnn) == 0)
            number_of_sessions++;

    return number_of_sessions;
}

pcf_sess_t *pcf_sess_find_by_ipv6addr(char *ipv6addr_string)
{
    int rv;
    ogs_sockaddr_t tmp;
    struct {
        uint8_t len;
        uint8_t addr6[OGS_IPV6_LEN];
    } ipv6prefix;

    ogs_assert(ipv6addr_string);

    rv = ogs_inet_pton(AF_INET6, ipv6addr_string, &tmp);
    if (rv != OGS_OK) {
        ogs_error("ogs_inet_pton() failed");
        return NULL;
    }

    memcpy(ipv6prefix.addr6, tmp.sin6.sin6_addr.s6_addr, OGS_IPV6_LEN);
    ipv6prefix.len = OGS_IPV6_128_PREFIX_LEN;

    return ogs_hash_get(self.ipv6prefix_hash,
            &ipv6prefix, (ipv6prefix.len >> 3) + 1);
}

//TODO:可能有bug
pcf_sess_t *pcf_sess_find_by_ipv6(const void *ipv6addr)
{    
    struct {
        uint8_t len;
        uint8_t addr6[OGS_IPV6_LEN];
    } ipv6prefix;
    memcpy(ipv6prefix.addr6, ipv6addr, OGS_IPV6_LEN);
    ipv6prefix.len = OGS_IPV6_128_PREFIX_LEN;
    return ogs_hash_get(self.ipv6prefix_hash,
            &ipv6prefix, (ipv6prefix.len >> 3) + 1);
}

pcf_sess_t *pcf_sess_find_by_ipv6prefix(char *ipv6prefix_string)
{
    int rv;
    struct {
        uint8_t len;
        uint8_t addr6[OGS_IPV6_LEN];
    } ipv6prefix;

    ogs_assert(ipv6prefix_string);

    rv = ogs_ipv6prefix_from_string(
            ipv6prefix.addr6, &ipv6prefix.len, ipv6prefix_string);
    ogs_assert(rv == OGS_OK);

    ogs_assert(ipv6prefix.len == OGS_IPV6_128_PREFIX_LEN);

    return ogs_hash_get(self.ipv6prefix_hash,
            &ipv6prefix, (ipv6prefix.len >> 3) + 1);
}

pcf_ue_t *pcf_ue_find_by_id(ogs_pool_id_t id)
{
    return ogs_pool_find_by_id(&pcf_ue_pool, id);
}

pcf_sess_t *pcf_sess_find_by_id(ogs_pool_id_t id)
{
    return ogs_pool_find_by_id(&pcf_sess_pool, id);
}

pcf_app_t *pcf_app_add(pcf_sess_t *sess)
{
    pcf_app_t *app = NULL;

    ogs_assert(sess);

    ogs_pool_alloc(&pcf_app_pool, &app);
    ogs_assert(app);
    memset(app, 0, sizeof *app);

    app->app_session_id = ogs_msprintf("%d",
            (int)ogs_pool_index(&pcf_app_pool, app));
    ogs_assert(app->app_session_id);

    app->sess = sess;

    ogs_list_add(&sess->app_list, app);

    return app;
}

int pcf_app_remove(pcf_app_t *app)
{
    int i;

    ogs_assert(app);
    ogs_assert(app->sess);

    ogs_list_remove(&app->sess->app_list, app);

    for (i = 0; i < app->num_of_pcc_rule; i++) {
        OGS_PCC_RULE_FREE(&app->pcc_rule[i]);
    }

    if (app->notif_uri)
        ogs_free(app->notif_uri);
    if (app->naf.client)
        ogs_sbi_client_remove(app->naf.client);

    ogs_assert(app->app_session_id);
    ogs_free(app->app_session_id);

    if (app->rx_sid)
        ogs_free(app->rx_sid);

    ogs_pool_free(&pcf_app_pool, app);

    return OGS_OK;
}

void pcf_app_remove_all(pcf_sess_t *sess)
{
    pcf_app_t *app = NULL, *next_app = NULL;

    ogs_assert(sess);
    ogs_list_for_each_safe(&sess->app_list, next_app, app)
        pcf_app_remove(app);
}

pcf_app_t *pcf_app_find(uint32_t index)
{
    return ogs_pool_find(&pcf_app_pool, index);
}

pcf_app_t *pcf_app_find_by_app_session_id(char *app_session_id)
{
    ogs_assert(app_session_id);
    return pcf_app_find(atoll(app_session_id));
}

int pcf_instance_get_load(void)
{
    return (((ogs_pool_size(&pcf_ue_pool) -
            ogs_pool_avail(&pcf_ue_pool)) * 100) /
            ogs_pool_size(&pcf_ue_pool));
}

int pcf_db_qos_data(char *supi,
        ogs_plmn_id_t *plmn_id, ogs_s_nssai_t *s_nssai, char *dnn,
        ogs_session_data_t *session_data)
{
    int rv;

    ogs_app_policy_conf_t *policy_conf = NULL;

    ogs_assert(supi);
    ogs_assert(s_nssai);
    ogs_assert(dnn);
    ogs_assert(session_data);

    memset(session_data, 0, sizeof(*session_data));

    if (plmn_id)
        policy_conf = ogs_app_policy_conf_find_by_plmn_id(plmn_id);
    else
        ogs_warn("No PLMN_ID");

    if (policy_conf) {
        rv = ogs_app_config_session_data(
                plmn_id, s_nssai, dnn, session_data);
        if (rv != OGS_OK)
            ogs_error("ogs_app_config_session_data() failed - "
                    "MCC[%d] MNC[%d] SST[%d] SD[0x%x] DNN[%s]",
                    ogs_plmn_id_mcc(plmn_id), ogs_plmn_id_mnc(plmn_id),
                    s_nssai->sst, s_nssai->sd.v, dnn);
    } else {
        rv = ogs_dbi_session_data(supi, s_nssai, dnn, session_data);
        if (rv != OGS_OK)
            ogs_error("ogs_dbi_session_data() failed - "
                    "SUPI[%s] SST[%d] SD[0x%x] DNN[%s]",
                    supi, s_nssai->sst, s_nssai->sd.v, dnn);
    }

    return rv;
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
  
    rv = pcf_context_parse_config(true);
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