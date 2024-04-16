import * as schemas from './EditCfgSection';

export const upfschema = {
  //title: "UPF Configuration",
  type: "object",
  properties: {
    upf: {
      type: "object",
      title: "UPF",
      properties: {          
        pfcp:schemas.pfcpSchema,
        gtpu:schemas.gtpuSchema,
        emptyLine1:schemas.emptyLineSchema,
        subnet:schemas.subnetSchema,
        dpdk: schemas.dpdkSchema,
        metrics: schemas.metricsSchema,
      },
    },
    //smf: schemas.smfSchema,
    parameter: schemas.paraSchema,
    logger: schemas.loggerSchema,
    cli: schemas.cliSchema,
  },
};

export const upfuiSchema = {
  logger: schemas.loggerUiSchema,
  parameter: schemas.paraUiSchema,
  upf: {
    classNames: "col-xs-12",
    pfcp:schemas.pfcpUiSchema,
    gtpu:schemas.gtpuUiSchema,
    emptyLine1:schemas.emptyLineUiSchema,
    dpdk: schemas.dpdkUiSchema,
    metrics: schemas.metricsUiSchema,
    subnet:schemas.subnetUiSchema,
  },
  cli: schemas.cliUiSchema,
  //smf: schemas.smfUiSchema,
};