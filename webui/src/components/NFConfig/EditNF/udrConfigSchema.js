import * as schemas from './EditCfgSection';

export const udrschema = {
  //title: "UDR Configuration",
  type: "object",
  properties: {
    udr: {
      type: "object",
      title: "UDR",
      properties: {
        sbi: schemas.nf_sbi_Schema,
        info: schemas.supiRangeInfoSchema,
        metrics: schemas.metricsSchema,
      },
    },    
    nrf: schemas.nrfSchema,
    parameter: schemas.paraSchema,
    db_uri: schemas.db_uriSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
  },
};

export const udruiSchema = {
  db_uri: schemas.db_uriUiSchema,
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  parameter: schemas.paraUiSchema,
  udr: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    info: schemas.supiRangeInfoUiSchema,
    metrics: schemas.metricsUiSchema,
  },
 
};