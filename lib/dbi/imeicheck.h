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

#if !defined(OGS_DBI_INSIDE) && !defined(OGS_DBI_COMPILATION)
#error "This header cannot be included directly."
#endif

#ifndef OGS_DBI_IMEICHECK_H
#define OGS_DBI_IMEICHECK_H

#ifdef __cplusplus
extern "C" {
#endif
#define OGS_MAX_BIND_IMSI_NUM              10

typedef struct {
    int schema_version;
    char imei[OGS_MAX_IMEI_LEN];
    int reason;
    int status;
    bool checkflag;
    struct {
        char msisdn[OGS_MAX_MSISDN_BCD_LEN+1];
        char imsi[OGS_MAX_IMSI_BCD_LEN+1];
    } bindimsi[OGS_MAX_BIND_IMSI_NUM];
} ImeicheckDocument;
typedef struct {
    #if 0
    char opttime[1][50];
    char opuser[1][50];
    char optype[1][50];
    char optfm[1][50];
    #endif
    char *optcommand;
} OmmlogSchema;


int ogs_dbi_imeicheck_data(char* imei, ImeicheckDocument* document);
bool insert_document_to_collection(const OmmlogSchema *document);

#ifdef __cplusplus
}
#endif

#endif /* OGS_DBI_SUBSCRIPTION_H */
