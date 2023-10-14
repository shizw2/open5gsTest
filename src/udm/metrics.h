#ifndef UDM_METRICS_H
#define UDM_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum udm_metric_type_global_s {
    _UDM_METR_GLOB_MAX,
} udm_metric_type_global_t;
extern ogs_metrics_inst_t *udm_metrics_inst_global[_UDM_METR_GLOB_MAX];

int udm_metrics_init_inst_global(void);
int udm_metrics_free_inst_global(void);

static inline void udm_metrics_inst_global_set(udm_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(udm_metrics_inst_global[t], val); }
static inline void udm_metrics_inst_global_add(udm_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(udm_metrics_inst_global[t], val); }
static inline void udm_metrics_inst_global_inc(udm_metric_type_global_t t)
{ ogs_metrics_inst_inc(udm_metrics_inst_global[t]); }
static inline void udm_metrics_inst_global_dec(udm_metric_type_global_t t)
{ ogs_metrics_inst_dec(udm_metrics_inst_global[t]); }



void udm_metrics_init(void);
void udm_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* UDM_METRICS_H */
