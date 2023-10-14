#ifndef NSSF_METRICS_H
#define NSSF_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum nssf_metric_type_global_s {
    _NSSF_METR_GLOB_MAX,
} nssf_metric_type_global_t;
extern ogs_metrics_inst_t *nssf_metrics_inst_global[_NSSF_METR_GLOB_MAX];

int nssf_metrics_init_inst_global(void);
int nssf_metrics_free_inst_global(void);

static inline void nssf_metrics_inst_global_set(nssf_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(nssf_metrics_inst_global[t], val); }
static inline void nssf_metrics_inst_global_add(nssf_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(nssf_metrics_inst_global[t], val); }
static inline void nssf_metrics_inst_global_inc(nssf_metric_type_global_t t)
{ ogs_metrics_inst_inc(nssf_metrics_inst_global[t]); }
static inline void nssf_metrics_inst_global_dec(nssf_metric_type_global_t t)
{ ogs_metrics_inst_dec(nssf_metrics_inst_global[t]); }



void nssf_metrics_init(void);
void nssf_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* NSSF_METRICS_H */
