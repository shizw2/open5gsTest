import * as schemas from './EditCfgSection';

export const ausfschema = {
  title: "AUSF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
    ausf: {
      type: "object",
      properties: {
        sbi: schemas.nf_sbi_Schema, 
      },
    },    
    
  },
};

export const ausfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  ausf: {
    sbi: schemas.nf_sbi_UiSchema,
  },
 
};