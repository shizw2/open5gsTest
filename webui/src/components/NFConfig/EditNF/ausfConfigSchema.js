import { loggerSchema, sbiSchema, nrfSchema,nf_sbi_Schema,timeSchema } from './EditCfgSection';
import { loggerUiSchema, sbiUiSchema, nrfUiSchema,nf_sbi_UiSchema,timeUiSchema } from './EditCfgSection';

export const ausfschema = {
  "title": "NRF Configuration",
  "type": "object",
  "properties": {
    logger: {
      ...loggerSchema,
    },
    sbi: {
      ...sbiSchema,
    },
    "ausf": {
      "type": "object",
      ...nf_sbi_Schema,
    },
    nrf: {
      ...nrfSchema,
    },

    time: {
      ...timeSchema, 
    }
  }
};

export const ausfuiSchema = {
  logger: {
    ...loggerUiSchema,
  },
  sbi: {
    ...sbiUiSchema,
  },
  nrf: {
    ...nrfUiSchema,
  },
  "ausf": {
    classNames: "col-xs-12",
    ...nf_sbi_Schema,
  }, 
  time: {
    ...timeUiSchema, 
  }
};