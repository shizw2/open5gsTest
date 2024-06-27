import * as schemas from './EditCfgSection';

export const nrfschema = {
  //title: "NRF Configuration",
  type: "object",
  properties: {
    nrf: schemas.nrfNFSchema,
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

export const nrfuiSchema = {
  logger: schemas.loggerUiSchema,
  //sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfNFUiSchema,
  //parameter: schemas.paraUiSchema,
  global: {
    classNames: "col-xs-12",
    cli: schemas.cliUiSchema,
    parameter: schemas.paraUiSchema,
  }
};