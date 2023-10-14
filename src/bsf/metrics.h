#ifndef BSF_METRICS_H
#define BSF_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bsf_metric_type_global_s {
    _BSF_METR_GLOB_MAX,
} bsf_metric_type_global_t;
extern ogs_metrics_inst_t *bsf_metrics_inst_global[_BSF_METR_GLOB_MAX];

int bsf_metrics_init_inst_global(void);
int bsf_metrics_free_inst_global(void);

static inline void bsf_metrics_inst_global_set(bsf_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(bsf_metrics_inst_global[t], val); }
static inline void bsf_metrics_inst_global_add(bsf_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(bsf_metrics_inst_global[t], val); }
static inline void bsf_metrics_inst_global_inc(bsf_metric_type_global_t t)
{ ogs_metrics_inst_inc(bsf_metrics_inst_global[t]); }
static inline void bsf_metrics_inst_global_dec(bsf_metric_type_global_t t)
{ ogs_metrics_inst_dec(bsf_metrics_inst_global[t]); }



void bsf_metrics_init(void);
void bsf_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* BSF_METRICS_H */
