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

#include "pcf-fd-path.h"

int pcf_fd_init(void)
{
    int rv;

    if (pcf_self()->diam_conf_path == NULL &&
        (pcf_self()->diam_config->cnf_diamid == NULL ||
        pcf_self()->diam_config->cnf_diamrlm == NULL ||
        pcf_self()->diam_config->cnf_addr == NULL)) {
        ogs_warn("No diameter configuration");
        return OGS_OK;
    }

    rv = ogs_diam_init(FD_MODE_CLIENT|FD_MODE_SERVER,
                pcf_self()->diam_conf_path, pcf_self()->diam_config);
    ogs_assert(rv == 0);

    // rv = pcrf_gx_init();
    // ogs_assert(rv == OGS_OK);
    rv = pcf_rx_init();
    ogs_assert(rv == OGS_OK);

    rv = ogs_diam_start();
    ogs_assert(rv == 0);

    return OGS_OK;
}

void pcf_fd_final(void)
{
    if (pcf_self()->diam_conf_path == NULL &&
        (pcf_self()->diam_config->cnf_diamid == NULL ||
        pcf_self()->diam_config->cnf_diamrlm == NULL ||
        pcf_self()->diam_config->cnf_addr == NULL)) {
        return;
    }

    pcf_rx_final();
   
    ogs_diam_final();
}

