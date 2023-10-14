#include "ogs-app.h"
#include "context.h"

#include "metrics.h"

typedef struct ausf_metrics_spec_def_s {
    unsigned int type;
    const char *name;
    const char *description;
    int initial_val;
    unsigned int num_labels;
    const char **labels;
} ausf_metrics_spec_def_t;

/* Helper generic functions: */
static int ausf_metrics_init_inst(ogs_metrics_inst_t **inst,
        ogs_metrics_spec_t **specs, unsigned int len,
        unsigned int num_labels, const char **labels)
{
    unsigned int i;
    for (i = 0; i < len; i++)
        inst[i] = ogs_metrics_inst_new(specs[i], num_labels, labels);
    return OGS_OK;
}

static int ausf_metrics_free_inst(ogs_metrics_inst_t **inst,
        unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i++)
        ogs_metrics_inst_free(inst[i]);
    memset(inst, 0, sizeof(inst[0]) * len);
    return OGS_OK;
}

static int ausf_metrics_init_spec(ogs_metrics_context_t *ctx,
        ogs_metrics_spec_t **dst, ausf_metrics_spec_def_t *src, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i++) {
        dst[i] = ogs_metrics_spec_new(ctx, src[i].type,
                src[i].name, src[i].description,
                src[i].initial_val, src[i].num_labels, src[i].labels,
                NULL);
    }
    return OGS_OK;
}

/* GLOBAL */
ogs_metrics_spec_t *ausf_metrics_spec_global[_AUSF_METR_GLOB_MAX];
ogs_metrics_inst_t *ausf_metrics_inst_global[_AUSF_METR_GLOB_MAX];
ausf_metrics_spec_def_t ausf_metrics_spec_def_global[_AUSF_METR_GLOB_MAX] = {
/* Global Counters: */
/* Global Gauges: */
};
int ausf_metrics_init_inst_global(void)
{
    return ausf_metrics_init_inst(ausf_metrics_inst_global,
            ausf_metrics_spec_global, _AUSF_METR_GLOB_MAX, 0, NULL);
}
int ausf_metrics_free_inst_global(void)
{
    return ausf_metrics_free_inst(ausf_metrics_inst_global, _AUSF_METR_GLOB_MAX);
}

void ausf_metrics_init(void)
{
    ogs_metrics_context_t *ctx = ogs_metrics_self();
    ogs_metrics_context_init();

    ausf_metrics_init_spec(ctx, ausf_metrics_spec_global,
            ausf_metrics_spec_def_global, _AUSF_METR_GLOB_MAX);

    ausf_metrics_init_inst_global();
    
}

void ausf_metrics_final(void)
{
       ogs_metrics_context_final();
}
