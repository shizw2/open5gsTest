#ifndef AUSF_METRICS_H
#define AUSF_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ausf_metric_type_global_s {
    _AUSF_METR_GLOB_MAX,
} ausf_metric_type_global_t;
extern ogs_metrics_inst_t *ausf_metrics_inst_global[_AUSF_METR_GLOB_MAX];

int ausf_metrics_init_inst_global(void);
int ausf_metrics_free_inst_global(void);

static inline void ausf_metrics_inst_global_set(ausf_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(ausf_metrics_inst_global[t], val); }
static inline void ausf_metrics_inst_global_add(ausf_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(ausf_metrics_inst_global[t], val); }
static inline void ausf_metrics_inst_global_inc(ausf_metric_type_global_t t)
{ ogs_metrics_inst_inc(ausf_metrics_inst_global[t]); }
static inline void ausf_metrics_inst_global_dec(ausf_metric_type_global_t t)
{ ogs_metrics_inst_dec(ausf_metrics_inst_global[t]); }



void ausf_metrics_init(void);
void ausf_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* AUSF_METRICS_H */
