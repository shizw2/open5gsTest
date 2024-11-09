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

#ifndef SACC_CONTEXT_H
#define SACC_CONTEXT_H

#include "ogs-app.h"
#include "ogs-sbi.h"
#include "ogs-sctp.h"


#ifdef __cplusplus
extern "C" {
#endif

int read_csv_line(FILE *file, char *buffer, char *tokens[], int max_tokens);
void parse_json_field(char *json_str);
int get_T2_config(void);
void ogs_sbi_context_get_nf_addr(ogs_sbi_nf_instance_t *nf_instance, const char *local);
int sacc_sbi_context_get_nf_info(const char *local, const char *nrf, const char *scp,ogs_sbi_nf_instance_t *nf_instance);
#ifdef __cplusplus
}
#endif

#endif /* SACC_CONTEXT_H */
