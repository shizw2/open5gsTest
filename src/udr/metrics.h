#ifndef UDR_METRICS_H
#define UDR_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum udr_metric_type_global_s {
    _UDR_METR_GLOB_MAX,
} udr_metric_type_global_t;
extern ogs_metrics_inst_t *udr_metrics_inst_global[_UDR_METR_GLOB_MAX];

int udr_metrics_init_inst_global(void);
int udr_metrics_free_inst_global(void);

static inline void udr_metrics_inst_global_set(udr_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(udr_metrics_inst_global[t], val); }
static inline void udr_metrics_inst_global_add(udr_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(udr_metrics_inst_global[t], val); }
static inline void udr_metrics_inst_global_inc(udr_metric_type_global_t t)
{ ogs_metrics_inst_inc(udr_metrics_inst_global[t]); }
static inline void udr_metrics_inst_global_dec(udr_metric_type_global_t t)
{ ogs_metrics_inst_dec(udr_metrics_inst_global[t]); }



void udr_metrics_init(void);
void udr_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* UDR_METRICS_H */
