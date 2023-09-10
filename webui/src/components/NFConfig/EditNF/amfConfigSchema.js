import * as schemas from './EditCfgSection';

export const amfschema = {
  title: "AMF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    amf: {
      type: "object",
      title: "AMF",
      properties: {
        sbi: schemas.nf_sbi_Schema,
        ngap: schemas.ngapSchema,
        metrics: schemas.metricsSchema,
        guami: schemas.guamiSchema,
        tai: schemas.taiSchema,
        //plmn
        security:schemas.securitySchema,
        network_name:schemas.network_nameSchema,
        amf_name:schemas.amf_nameSchema,
      },
    },
    nrf: schemas.nrfSchema,
    time: {
      type: "object",
      title: "Time",
      properties: {
        nf_instance: schemas.time_nf_instanceSchema,
        t3512: schemas.time_t3512Schema,
        t3502: schemas.time_t3502Schema,
      },
    },
  },
};

export const amfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  amf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    ngap: schemas.ngapUiSchema,
    metrics: schemas.metricsUiSchema,
    guami: schemas.guamiUiSchema,
    tai: schemas.taiUiSchema,
    security:schemas.securityUiSchema,
    network_name:schemas.network_nameUiSchema,
    amf_name:schemas.amf_nameUiSchema,
  },
  time: {
    classNames: "col-xs-12",
    nf_instance: schemas.time_nf_instanceUiSchema,
    t3512: schemas.time_t3512UiSchema,
    t3502: schemas.time_t3502UiSchema,
  },
};