#ifndef SACC_METRICS_H
#define SACC_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum sacc_metric_type_global_s {
    _SACC_METR_GLOB_MAX,
} sacc_metric_type_global_t;
extern ogs_metrics_inst_t *sacc_metrics_inst_global[_SACC_METR_GLOB_MAX];

int sacc_metrics_init_inst_global(void);
int sacc_metrics_free_inst_global(void);

static inline void sacc_metrics_inst_global_set(sacc_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(sacc_metrics_inst_global[t], val); }
static inline void sacc_metrics_inst_global_add(sacc_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(sacc_metrics_inst_global[t], val); }
static inline void sacc_metrics_inst_global_inc(sacc_metric_type_global_t t)
{ ogs_metrics_inst_inc(sacc_metrics_inst_global[t]); }
static inline void sacc_metrics_inst_global_dec(sacc_metric_type_global_t t)
{ ogs_metrics_inst_dec(sacc_metrics_inst_global[t]); }



void sacc_metrics_init(void);
void sacc_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* SACC_METRICS_H */
