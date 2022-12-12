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

#ifndef PCF_FD_PATH_H
#define PCF_FD_PATH_H

#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gtp_xact_s gtp_xact_t;

int pcf_fd_init(void);
void pcf_fd_final(void);

int pcf_rx_init(void);
void pcf_rx_final(void);
int pcf_rx_send_asr(uint8_t *rx_sid, uint32_t abort_cause);

#ifdef __cplusplus
}
#endif

#endif /* PCF_FD_PATH_H */
