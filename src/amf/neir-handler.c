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

#include "neir-handler.h"

#include "sbi-path.h"
#include "nas-path.h"

int amf_neir_imeicheck_handle(
        amf_ue_t *amf_ue, int state, ogs_sbi_message_t *recvmsg)
{
    int i, r;

    OpenAPI_eir_response_data_t *eir_response;

    ogs_assert(amf_ue);
    ogs_assert(recvmsg);
    
    eir_response=recvmsg->eir_response;

    if (!eir_response) {
        ogs_error("[%s] No eir_response", amf_ue->suci);
        return OGS_ERROR;
    }

    SWITCH(recvmsg->h.resource.component[0])
    CASE(OGS_SBI_RESOURCE_NAME_EIR_EQUIPMENTSTATUS)
        if(strcmp(OpenAPI_equipment_status_ToString(eir_response->status), "BLACKLISTED") == 0){
             r=OGS_ERROR;
         }else{
             r=OGS_OK;
             ogs_info(" eir_response->status :[%s]", OpenAPI_equipment_status_ToString(eir_response->status));
        }
        ogs_free(eir_response);
        break;

    DEFAULT
        r=OGS_OK;
        ogs_error("Invalid resource name [%s]",
                recvmsg->h.resource.component[0]);
        ogs_assert_if_reached();
    END

   
    return r;
}
