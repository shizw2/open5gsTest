#ifndef OPEN5GS_CTRL_PATH_H
#define OPEN5GS_CTRL_PATH_H

#include "context.h"
#include "upf-dpdk.h"

#define MALLOC_NAME "upf-dpdk"

#define FORMAT_IPV4_ARGS(_IPV4) (_IPV4 >> 24) & 0xFF, \
(_IPV4 >> 16) & 0xFF, \
(_IPV4 >> 8) & 0xFF, \
_IPV4 & 0xFF

#define FORMAT_IPV4 "%d.%d.%d.%d"

#define dpdk_malloc(_size) dpdk_malloc_debug(_size, 0);

static inline void *dpdk_malloc_debug(uint32_t size, unsigned allign) {
    void *p = NULL;
    p = rte_zmalloc(MALLOC_NAME, size, allign);
    memset(p, 0, size);
    return p;
}

#define dpdk_free(item) \
    do {                \
        rte_free(item); \
    } while(0)

typedef enum {
    UPF_DPDK_SESS_ESTAB,
    UPF_DPDK_SESS_MOD,
    UPF_DPDK_SESS_DEL,
    UPF_DPDK_SESS_REPORT,
    UPF_DPDK_NBR_MSG
} upf_dpdk_event_type_e;

typedef struct upf_dpdk_event_report_s {
    uint8_t pdr_id;
    uint8_t qfi;
    uint32_t sess_index;
    uint8_t type_volume;
    uint64_t total_octets;
    uint64_t ul_octets;
    uint64_t dl_octets;
    void *paylod;
} upf_dpdk_event_report_t;

typedef enum {
    SESS_REPORT_NORMAL,
    SESS_REPORT_ERR_IND,
    SESS_REPORT_VOLUME
} upf_dpdk_event_report_subtype_t;

typedef struct upf_dpdk_event_s {
    upf_dpdk_event_type_e event_type;
    uint16_t subtype;
    void *event_body;
} upf_dpdk_event_t;


// CP Calls
int upf_dpdk_sess_establish(upf_sess_t *sess);

int upf_dpdk_sess_delete(upf_sess_t *sess);

int upf_dpdk_sess_modify(upf_sess_t *sess);

int upf_dpdk_loop_event(void);

// UP Calls
int free_upf_dpdk_sess(upf_sess_t *sess);

int upf_dpdk_sess_report(int fwd_id, upf_dpdk_event_t *event);

int build_report_event(void);

int upf_dpdk_nbr_notify(upf_nbr_message_t *nbrmsg);
#endif
