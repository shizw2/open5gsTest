import * as schemas from './EditCfgSection';

export const pcfschema = {
  //title: "PCF Configuration",
  type: "object",
  properties: {
    pcf: {
      type: "object",
      title: "PCF",
      properties: {
        sbi: schemas.nf_sbi_Schema,
        freeDiameter:schemas.freeDiameter2Schema,
        metrics: schemas.metricsSchema,
        info: schemas.supiRangeInfoSchema,
      },
    },
    nrf: schemas.nrfSchema,
    parameter: schemas.paraSchema,
    db_uri: schemas.db_uriSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
  },
};

export const pcfuiSchema = {
  db_uri: schemas.db_uriUiSchema,
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  parameter: schemas.paraUiSchema,
  pcf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    freeDiameter:schemas.freeDiameter2UiSchema,
    metrics: schemas.metricsUiSchema,
    info: schemas.supiRangeInfoUiSchema
  },
};