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
      properties: {
        sbi: schemas.nf_sbi_Schema,        
        hnet:schemas.hnetSchema,
        
      },
    },    
    
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
 
};