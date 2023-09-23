import * as schemas from './EditCfgSection';

export const udmschema = {
  //title: "UDM Configuration",
  type: "object",
  properties: {
    udm: {
      type: "object",
      title: "UDM",
      properties: {
        sbi: schemas.nf_sbi_Schema,        
        hnet:schemas.hnetSchema,
        
      },
    }, 
    nrf: schemas.nrfSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
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