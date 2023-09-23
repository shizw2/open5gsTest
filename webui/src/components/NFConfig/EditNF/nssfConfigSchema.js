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
      },
    },
    nrf: schemas.nrfSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
  },
};

export const nssfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  nssf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    nsi:schemas.nsiUiSchema,
  },
};