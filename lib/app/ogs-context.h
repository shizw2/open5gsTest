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

#if !defined(OGS_APP_INSIDE) && !defined(OGS_APP_COMPILATION)
#error "This header cannot be included directly."
#endif

#ifndef OGS_APP_CONTEXT_H
#define OGS_APP_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif


//注意：在处理 YAML 数据时，YAML 库返回的value是由该库自己分配的内存，
//当前代码中，许多字段直接使用了 YAML 的原始内存值（const char*），这样在每次重新读取 YAML 时老的value已经被释放，无法确定 value 是否发生更改。
//如果要准确判断value是否有更改（比如value变化时需要进一步处理，比如通知某进程等），则需要先复制一份副本(ogs_strdup)。
//如果不需要进行特殊处理，则目前const char*的方式也没啥问题

typedef struct ogs_app_context_s {
    const char *version;

    const char *file;
    // 保存文件的最后修改时间
    time_t file_last_modified_time;

    void *document;

    const char *db_uri;

    struct {
        ogs_log_ts_e timestamp;
    } logger_default;

    struct {
        const char *file;
        const char *level;
        const char *domain;
        ogs_log_ts_e timestamp;
    } logger;

    ogs_queue_t *queue;
    ogs_timer_mgr_t *timer_mgr;
    ogs_pollset_t *pollset;

    struct {
        int udp_port;
    } usrsctp;

    struct {
        uint64_t packet;

        uint64_t sess;
        uint64_t bearer;
        uint64_t tunnel;

        uint64_t nf_service;

        uint64_t timer;
        uint64_t message;
        uint64_t event;
        uint64_t socket;
        uint64_t subscription;
        uint64_t xact;
        uint64_t stream;

        uint64_t nf;
        uint64_t gtp_node;

        uint64_t csmap;

        uint64_t impi;
        uint64_t impu;
    } pool;

    struct metrics {
        uint64_t max_specs;
    } metrics;

    int group;
    int node;
} ogs_app_context_t;

int ogs_app_context_init(void);
void ogs_app_context_final(void);

ogs_app_context_t *ogs_app(void);

#ifdef __cplusplus
}
#endif

#endif /* OGS_APP_CONTEXT_H */
