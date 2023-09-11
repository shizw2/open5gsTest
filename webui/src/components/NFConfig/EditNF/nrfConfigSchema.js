import * as schemas from './EditCfgSection';

export const nrfschema = {
  title: "NRF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
  
  },
};

export const nrfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
};