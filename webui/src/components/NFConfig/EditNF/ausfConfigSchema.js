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
      },
    },
    nrf: schemas.nrfSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
  },
};

export const ausfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  ausf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
  },
};