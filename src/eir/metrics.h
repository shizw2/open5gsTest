#ifndef EIR_METRICS_H
#define EIR_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum eir_metric_type_global_s {
    _EIR_METR_GLOB_MAX,
} eir_metric_type_global_t;
extern ogs_metrics_inst_t *eir_metrics_inst_global[_EIR_METR_GLOB_MAX];

int eir_metrics_init_inst_global(void);
int eir_metrics_free_inst_global(void);

static inline void eir_metrics_inst_global_set(eir_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(eir_metrics_inst_global[t], val); }
static inline void eir_metrics_inst_global_add(eir_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(eir_metrics_inst_global[t], val); }
static inline void eir_metrics_inst_global_inc(eir_metric_type_global_t t)
{ ogs_metrics_inst_inc(eir_metrics_inst_global[t]); }
static inline void eir_metrics_inst_global_dec(eir_metric_type_global_t t)
{ ogs_metrics_inst_dec(eir_metrics_inst_global[t]); }



void eir_metrics_init(void);
void eir_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* EIR_METRICS_H */
