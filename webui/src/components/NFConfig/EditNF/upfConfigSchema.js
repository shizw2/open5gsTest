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
        metrics: schemas.metricsShortSchema,
      },
    },
    logger: schemas.loggerSchema,
  },
};

export const upfuiSchema = {
  logger: schemas.loggerUiSchema,
  upf: {
    classNames: "col-xs-12",
    pfcp:schemas.pfcpUiSchema,
    gtpu:schemas.gtpuUiSchema,
    emptyLine1:schemas.emptyLineUiSchema,
    metrics: schemas.metricsShortUiSchema,
    subnet:schemas.subnetUiSchema,
  },
};