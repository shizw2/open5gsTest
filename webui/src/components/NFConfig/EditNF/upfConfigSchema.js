import * as schemas from './EditCfgSection';

export const upfschema = {
  title: "UPF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    upf: {
      type: "object",
      properties: {          
        pfcp:schemas.pfcpSchema,
        gtpu:schemas.gtpuSchema,
		subnet:schemas.subnetSchema,
        metrics: schemas.metricsSchema,
      },
    },    
    time: schemas.timeSchema,
  },
};

export const upfuiSchema = {
  logger: schemas.loggerUiSchema,
  upf: {
    classNames: "col-xs-12",
    pfcp:schemas.pfcpUiSchema,
    gtpu:schemas.gtpuUiSchema,
    metrics: schemas.metricsUiSchema,
    subnet:schemas.subnetUiSchema,
  },
  time: schemas.timeUiSchema,
};