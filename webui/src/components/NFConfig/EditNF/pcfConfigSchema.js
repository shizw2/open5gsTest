import * as schemas from './EditCfgSection';

export const pcfschema = {
  title: "PCF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
    pcf: {
      type: "object",
      properties: {
        sbi: schemas.nf_sbi_Schema,        
        freeDiameter:schemas.freeDiameter2Schema,
        metrics: schemas.metricsSchema,
        
      },
    },    
    
  },
};

export const pcfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  pcf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    freeDiameter:schemas.freeDiameter2UiSchema,
    metrics: schemas.metricsUiSchema,    
  },
 
};