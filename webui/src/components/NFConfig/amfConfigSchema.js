import { loggerSchema, sbiSchema, nrfSchema,nf_sbi_Schema,timeSchema,icpsSchema,metricsSchema,guamiSchema,taiSchema,securitySchema,networkNameSchema,amfNameSchema } from './EditCfgSection';
import { loggerUiSchema, sbiUiSchema, nrfUiSchema,nf_sbi_UiSchema,timeUiSchema,icpsUiSchema,metricsUiSchema,guamiUiSchema,taiUiSchema,securityUiSchema,networkNameUiSchema,amfNameUiSchema } from './EditCfgSection';

export const amfschema = {
  "title": "NRF Configuration",
  "type": "object",
  "properties": {
    logger: loggerSchema, 
    sbi: sbiSchema,
    amf: {
      "type": "object",
      properties:{
       sbi:nf_sbi_Schema,
       metrics:metricsSchema,
      }
    },
    nrf: nrfSchema,
    time: timeSchema, 
  }
};

export const amfuiSchema = {
  logger:loggerUiSchema,   
  sbi:sbiUiSchema,
  nrf:nrfUiSchema,
  "amf": {
    classNames: "col-xs-12",
    sbi:nf_sbi_UiSchema,
    metrics:metricsUiSchema,
  }, 
  time: timeUiSchema, 
};