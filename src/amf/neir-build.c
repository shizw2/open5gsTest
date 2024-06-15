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

#include "neir-build.h"

ogs_sbi_request_t *amf_neir_imeicheck_build(
        amf_ue_t *amf_ue, void *data)
{
    ogs_sbi_message_t message;   
    ogs_sbi_request_t *request = NULL;      

    ogs_assert(amf_ue);
    ogs_assert(amf_ue->supi);

    ogs_info("neir_imeicheck_build.");

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_GET;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_N5G_EIR_EIC;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_EIR_EQUIPMENTSTATUS;
    message.param.pei=ogs_msprintf("imei-%s", amf_ue->imei_bcd);
    message.param.supi=amf_ue->supi;
    request = ogs_sbi_build_request(&message);
    ogs_free(message.param.pei);
    ogs_expect(request);
    return request;
}


