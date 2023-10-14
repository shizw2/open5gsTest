#ifndef NRF_METRICS_H
#define NRF_METRICS_H

#include "ogs-metrics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum nrf_metric_type_global_s {
    _NRF_METR_GLOB_MAX,
} nrf_metric_type_global_t;
extern ogs_metrics_inst_t *nrf_metrics_inst_global[_NRF_METR_GLOB_MAX];

int nrf_metrics_init_inst_global(void);
int nrf_metrics_free_inst_global(void);

static inline void nrf_metrics_inst_global_set(nrf_metric_type_global_t t, int val)
{ ogs_metrics_inst_set(nrf_metrics_inst_global[t], val); }
static inline void nrf_metrics_inst_global_add(nrf_metric_type_global_t t, int val)
{ ogs_metrics_inst_add(nrf_metrics_inst_global[t], val); }
static inline void nrf_metrics_inst_global_inc(nrf_metric_type_global_t t)
{ ogs_metrics_inst_inc(nrf_metrics_inst_global[t]); }
static inline void nrf_metrics_inst_global_dec(nrf_metric_type_global_t t)
{ ogs_metrics_inst_dec(nrf_metrics_inst_global[t]); }



void nrf_metrics_init(void);
void nrf_metrics_final(void);

#ifdef __cplusplus
}
#endif

#endif /* NRF_METRICS_H */
