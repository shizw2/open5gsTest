import * as schemas from './EditCfgSection';

export const eirschema = {
  //title: "EIR Configuration",
  type: "object",
  properties: {
    eir: {
      type: "object",
      title: "EIR",
      properties: {
        sbi: schemas.nf_sbi_OldSchema, 
        metrics: schemas.metricsOldSchema,
      },
    },
    nrf: schemas.nrfSchema,
    sbi: schemas.sbiSchema,
    db_uri: schemas.db_uriSchema,
    logger: schemas.loggerOldSchema,
    //cli: schemas.cliSchema,
  },
};

export const eiruiSchema = {
  logger: schemas.loggerOldUiSchema,
  db_uri: schemas.db_uriUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  parameter: schemas.paraUiSchema,
  eir: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_OldUiSchema,
    metrics: schemas.metricsOldUiSchema,
  },
  //cli: schemas.cliUiSchema,
};