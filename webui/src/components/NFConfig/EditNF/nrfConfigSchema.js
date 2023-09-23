import * as schemas from './EditCfgSection';

export const nrfschema = {
  //title: "NRF Configuration",
  type: "object",
  properties: {
    nrf: schemas.nrfSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
  },
};

export const nrfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
};