import * as schemas from './EditCfgSection';

export const nssfschema = {
  //title: "NSSF Configuration",
  type: "object",
  properties: {
    nssf: {
      type: "object",
      title:"NSSF",
      properties: {
        sbi: schemas.nf_sbi_Schema,
        nsi:schemas.nsiSchema,
        metrics: schemas.metricsSchema,
      },
    },
    nrf: schemas.nrfSchema,
    parameter: schemas.paraSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
    cli: schemas.cliSchema,
  },
};

export const nssfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  parameter: schemas.paraUiSchema,
  nssf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    nsi:schemas.nsiUiSchema,
    metrics: schemas.metricsUiSchema,
  },
  cli: schemas.cliUiSchema,
};