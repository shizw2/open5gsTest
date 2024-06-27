import * as schemas from './EditCfgSection';

export const nssfschema = {
  //title: "NSSF Configuration",
  type: "object",
  properties: {
    nssf: {
      type: "object",
      title:"NSSF",
      properties: {
        sbi: schemas.nssf_sbi_Schema,
        //nsi:schemas.nsiSchema,
        //relative_capacity: schemas.relative_capSchema,
        metrics: schemas.metricsSchema,
      },
    },
    //nrf: schemas.nrfSchema,
    //parameter: schemas.paraSchema,
    //sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
    global: {
      type: "object",
      title: "Global",
      properties: {
        cli: schemas.cliSchema,
        parameter: schemas.paraSchema,
      }
    }
  },
};

export const nssfuiSchema = {
  logger: schemas.loggerUiSchema,
  //sbi: schemas.sbiUiSchema,
  //nrf: schemas.nrfUiSchema,
  //parameter: schemas.paraUiSchema,
  nssf: {
    classNames: "col-xs-12",
    sbi: schemas.nssf_sbi_UiSchema,
    //nsi:schemas.nsiUiSchema,
    //relative_capacity: schemas.relative_capUiSchema,
    metrics: schemas.metricsUiSchema,
  },
  global: {
    classNames: "col-xs-12",
    cli: schemas.cliUiSchema,
    parameter: schemas.paraUiSchema,
  }
};