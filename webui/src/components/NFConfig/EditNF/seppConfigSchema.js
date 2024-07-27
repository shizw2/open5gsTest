import * as schemas from './EditCfgSection';

export const seppschema = {
  //title: "sepp Configuration",
  type: "object",
  properties: {
    sepp: {
      type: "object",
      title: "SEPP",
      properties: {
        sbi: schemas.nf_sbi_Schema, 
        n32: schemas.n32Schema,
      },
    },
    logger: schemas.loggerSchema,
    global: {
      type: "object",
      title: "Global",
      properties: {
        cli: schemas.cliSchema,
        parameter: schemas.paraSchema,
      }
    }
    //cli: schemas.cliSchema,
  },
};

export const seppuiSchema = {
  logger: schemas.loggerUiSchema,
  sepp: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    n32: schemas.n32UiSchema,
  },
  global: {
    classNames: "col-xs-12",
    cli: schemas.cliUiSchema,
    parameter: schemas.paraUiSchema,
  }
  //cli: schemas.cliUiSchema,
};