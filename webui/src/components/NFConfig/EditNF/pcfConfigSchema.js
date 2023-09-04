import * as schemas from './EditCfgSection';

export const pcfschema = {
  title: "PCF Configuration",
  type: "object",
  properties: {
    db_uri: schemas.db_uriSchema,  
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
    pcf: {
      type: "object",
      title: "PCF",
      properties: {
        sbi: schemas.nf_sbi_Schema,        
        freeDiameter:schemas.freeDiameter2Schema,
        metrics: schemas.metricsSchema,
      },
    },
  },
};

export const pcfuiSchema = {
  db_uri: schemas.db_uriUiSchema,
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