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

#include "ogs-dbi.h"


int ogs_dbi_imeicheck_data(char* imei, ImeicheckDocument* document)
{
    int rv = OGS_ERROR;
    mongoc_collection_t *collection;
    bson_t *query;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_iter_t iter; 
    const char *utf8 = NULL;
    uint32_t length = 0;

    ogs_assert(document);
    ogs_assert(imei);

    query = BCON_NEW("imei", BCON_UTF8(imei));
#if MONGOC_MAJOR_VERSION >= 1 && MONGOC_MINOR_VERSION >= 5
    cursor = mongoc_collection_find_with_opts(
            ogs_mongoc()->collection.imeicheck, query, NULL, NULL);
#else
    cursor = mongoc_collection_find(ogs_mongoc()->collection.imeicheck,
            MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
#endif  

    while (mongoc_cursor_next(cursor, &doc)) {
        if (bson_iter_init(&iter, doc) && bson_iter_find(&iter, "schema_version") && BSON_ITER_HOLDS_INT32(&iter)) {
            document->schema_version = bson_iter_int32(&iter);

            bson_iter_find(&iter, "imei");
            strncpy(document->imei, bson_iter_utf8(&iter, NULL), OGS_MAX_IMEI_LEN);

            bson_iter_find(&iter, "reason");
            document->reason = bson_iter_int32(&iter);

            bson_iter_find(&iter, "status");
            document->status = bson_iter_int32(&iter);
            ogs_debug("test stautus:document->status=%d",document->status);
            bson_iter_find(&iter, "checkflag");
            document->checkflag = bson_iter_as_bool(&iter);

            bson_iter_find(&iter, "bindimsi");
            if (BSON_ITER_HOLDS_ARRAY(&iter)) {
                bson_iter_t sub_iter;
                bson_iter_recurse(&iter, &sub_iter);            
                int i = 0;
                while (bson_iter_next(&sub_iter) && i < OGS_MAX_BIND_IMSI_NUM) {
                    if (BSON_ITER_HOLDS_DOCUMENT(&sub_iter)) {
                        bson_iter_t doc_iter;
                        bson_iter_recurse(&sub_iter, &doc_iter);
                        while (bson_iter_next(&doc_iter)){
                            const char *key = bson_iter_key(&doc_iter);
                            if(!strcmp(key, "msisdn")){
                                if (BSON_ITER_HOLDS_UTF8(&doc_iter)) {                            
                                    utf8 = bson_iter_utf8(&doc_iter, &length);
                                    ogs_cpystrn(document->bindimsi[i].msisdn,
                                            utf8, ogs_min(length, OGS_MAX_MSISDN_BCD_LEN)+1);                            
                                }
                                ogs_debug("msisdn[%d]=%s",i,document->bindimsi[i].msisdn);
                            }else if(!strcmp(key, "imsi")){
                                if (BSON_ITER_HOLDS_UTF8(&doc_iter)) {
                                    utf8 = bson_iter_utf8(&doc_iter, &length);
                                    ogs_cpystrn(document->bindimsi[i].imsi,
                                            utf8, ogs_min(length, OGS_MAX_IMSI_BCD_LEN)+1);                                    
                                }
                                ogs_debug("imsi[%d]=%s",i,document->bindimsi[i].imsi);
                            }
                        }
                        i++;
                    }
                }
            }
            rv = OGS_OK; // Found the document
            break;
        }
    }

    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
    return rv;
}
bool insert_document_to_collection(const OmmlogSchema *document) {
    bson_error_t error;
    bson_t *doc;
    time_t currentTime;
    struct tm *localTime;
    char dateTimeString[20];   
    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    strftime(dateTimeString, sizeof(dateTimeString), "%Y/%m/%d %H:%M:%S", localTime);

    doc = BCON_NEW(
        "opttime", "[",
            BCON_UTF8(dateTimeString),
        "]",
        "opuser","[",
            BCON_UTF8("5gc"),
        "]",
        "optype", "[",
            BCON_UTF8("警示"),
        "]",
        "optfm", "[",
            BCON_UTF8("黑白名单"),
        "]",
        "optcommand", "[",
            BCON_UTF8(document->optcommand),
         "]"
    );

    if (!mongoc_collection_insert_one(ogs_mongoc()->collection.ommlog, doc, NULL, NULL, &error)) {
        ogs_error("Failed to insert document: %s\n", error.message);
        bson_destroy(doc);
        return false;
    }

    bson_destroy(doc);
    return true;

}

