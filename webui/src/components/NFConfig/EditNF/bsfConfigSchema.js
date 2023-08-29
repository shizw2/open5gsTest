import * as schemas from './EditCfgSection';

export const bsfschema = {
  title: "BSF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
    bsf: {
      type: "object",
      properties: {
        sbi: schemas.nf_sbi_Schema, 
      },
    },    
    
  },
};

export const bsfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  bsf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
  },
 
};