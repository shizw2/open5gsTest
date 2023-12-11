import * as schemas from './EditCfgSection';

export const bsfschema = {
  //title: "BSF Configuration",
  type: "object",
  properties: {
    bsf: {
      type: "object",
      title: "BSF",
      properties: {
        sbi: schemas.nf_sbi_Schema, 
      },
    },
    nrf: schemas.nrfSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
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
