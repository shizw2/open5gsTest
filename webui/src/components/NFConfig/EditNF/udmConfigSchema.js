import * as schemas from './EditCfgSection';

export const udmschema = {
  title: "UDM Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
    udm: {
      type: "object",
      title: "UDM",
      properties: {
        sbi: schemas.nf_sbi_Schema,        
        hnet:schemas.hnetSchema,
        
      },
    },    
    //time: {
    //  type: "object",
    //  title: "time",
    //  properties: {
    //      instance: schemas.time_instanceSchema,
    //  },
    //},
  },
};

export const udmuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  udm: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    hnet:schemas.hnetUiSchema,
  },
  //time: {
  //  classNames: "col-xs-12",
  //  instance: schemas.time_instanceUiSchema,
  //},
};