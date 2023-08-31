import * as schemas from './EditCfgSection';

export const bsfschema = {
  title: "BSF Configuration",
  type: "object",
  properties: {
    db_uri: schemas.db_uriSchema,
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
  db_uri: schemas.db_uriUiSchema,
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  bsf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
  },
 
};