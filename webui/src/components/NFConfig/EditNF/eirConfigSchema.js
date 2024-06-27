import * as schemas from './EditCfgSection';

export const eirschema = {
  //title: "EIR Configuration",
  type: "object",
  properties: {
    eir: {
      type: "object",
      title: "EIR",
      properties: {
        sbi: schemas.nf_sbi_Schema, 
        metrics: schemas.metricsSchema,
      },
    },
    //nrf: schemas.nrfSchema,
    //sbi: schemas.sbiSchema,
    db_uri: schemas.db_uriSchema,
    logger: schemas.loggerSchema,
    global: {
      type: "object",
      title: "Global",
      properties: {
        cli: schemas.cliSchema,
        parameter: schemas.paraSchema,
      }
    }
    //cli: schemas.cliSchema,
  },
};

export const eiruiSchema = {
  logger: schemas.loggerUiSchema,
  db_uri: schemas.db_uriUiSchema,
  //sbi: schemas.sbiUiSchema,
  //nrf: schemas.nrfUiSchema,
  //parameter: schemas.paraUiSchema,
  eir: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    metrics: schemas.metricsUiSchema,
  },
  global: {
    classNames: "col-xs-12",
    cli: schemas.cliUiSchema,
    parameter: schemas.paraUiSchema,
  }
  //cli: schemas.cliUiSchema,
};