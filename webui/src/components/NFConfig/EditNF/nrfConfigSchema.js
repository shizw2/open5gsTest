import * as schemas from './EditCfgSection';

export const nrfschema = {
  title: "NRF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
    
    time: schemas.timeSchema,
  },
};

export const nrfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  
  time: schemas.timeUiSchema,
};