/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
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
#include "neir-handler.h"



bool eir_neir_handle_imeicheck(
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    int rv;

    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;
    OpenAPI_eir_response_data_t eir_response;
    ImeicheckDocument document;
    char *pei = NULL;
    char *supi = NULL;
    char *supi_type = NULL;
    char *pei_type = NULL;
    memset(&document, 0, sizeof(ImeicheckDocument));


    ogs_assert(stream);
    ogs_assert(recvmsg);

    ogs_debug("recvmsg->param.pei=%s,recvmsg->param.supi=%s",recvmsg->param.pei,recvmsg->param.supi);
    pei_type=ogs_id_get_type(recvmsg->param.pei);
    ogs_debug("pei_type=%s",pei_type);
    if (strcmp(pei_type, "imei") != 0){
        ogs_error("No SUPPORT PEI Type");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No SUPPORT PEI Type", NULL));
        ogs_free(pei_type);
        return false;
    }
   pei = ogs_id_get_value(recvmsg->param.pei);
   ogs_debug("pei=%s",pei);
    if (!pei) {
        ogs_error("No PEI");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No PEI", NULL));
        ogs_free(pei_type);
        return false;
    }
    supi_type=ogs_id_get_type(recvmsg->param.supi);
    ogs_debug("supi_type=%s",supi_type);
    if (strcmp(supi_type, "imsi") != 0){
        ogs_error("No SUPPORT supi_type Type");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No SUPPORT supi_type Type", NULL));
        ogs_free(pei_type);
        ogs_free(supi_type);
        ogs_free(pei);
        return false;
    }
    supi = ogs_id_get_value(recvmsg->param.supi);
    if (!supi) {
        ogs_error("No SUPI");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No SUPI", NULL));
        ogs_free(pei);
        ogs_free(pei_type);
        ogs_free(supi_type);
        return false;
    }

    rv = ogs_dbi_imeicheck_data(pei,&document);
    if (rv != OGS_OK) {
        ogs_warn("[%s] Cannot find IMEI in DB", pei);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_NOT_FOUND,
                recvmsg, "Cannot find PEI", pei));
        ogs_free(supi);
        ogs_free(pei);
        ogs_free(pei_type);
        ogs_free(supi_type);
        return false;
    }
    eir_response.status=document.status;
    bool isFound = false;
    if(document.checkflag){
        for (int i = 0; i < 10; i++) {
            ogs_debug("supi=%s,imsi[%d]=%s",supi, i,document.bindimsi[i].imsi);
            if (strncmp(supi, document.bindimsi[i].imsi,15) == 0) {                
                isFound = true;
                break;
            }
        }
        if(!isFound){
            eir_response.status=2;//如果需要校验，没找到，则设置成黑名单
        }
    }
    if(!eir_response.status) eir_response.status=1;//默认白名单
    memset(&sendmsg, 0, sizeof(sendmsg));
    sendmsg.eir_response = &eir_response;
    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_OK);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));
    ogs_free(supi);
    ogs_free(pei);
    ogs_free(pei_type);
    ogs_free(supi_type);

    return true;
}
