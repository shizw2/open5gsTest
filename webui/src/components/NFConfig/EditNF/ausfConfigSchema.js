import * as schemas from './EditCfgSection';

export const ausfschema = {
  //title: "AUSF Configuration",
  type: "object",
  properties: {
    ausf: {
      type: "object",
      title: "AUSF",
      properties: {
        sbi: schemas.nf_sbi_Schema,
        info: schemas.rtsupiInfoSchema,
        metrics: schemas.metricsSchema,
        relative_capacity: schemas.relative_capSchema,
      },
    },
    //nrf: schemas.nrfSchema,
    //parameter: schemas.paraSchema,
    //sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
    cli: schemas.cliSchema,
  },
};

export const ausfuiSchema = {
  logger: schemas.loggerUiSchema,
  //sbi: schemas.sbiUiSchema,
  //nrf: schemas.nrfUiSchema,
  //parameter: schemas.paraUiSchema,
  ausf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    info: schemas.rtsupiInfoUiSchema,
    metrics: schemas.metricsUiSchema,
    relative_capacity: schemas.relative_capUiSchema,
  },
  cli: schemas.cliUiSchema,
};