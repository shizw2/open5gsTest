import * as schemas from './EditCfgSection';

export const smfschema = {
  title: "SMF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    nrf: schemas.nrfSchema,
    smf: {
      type: "object",
      properties: {
        sbi: schemas.nf_sbi_Schema,        
        pfcp:schemas.pfcpSchema,
        gtpc:schemas.gtpcSchema,
        gtpu:schemas.gtpuSchema,
        metrics: schemas.metricsSchema,
        subnet:schemas.subnetSchema,
        dns:schemas.dnsSchema,
        mtu:schemas.mtuSchema,
        crf:schemas.crfSchema,
        freeDiameter:schemas.freeDiameterSchema,
        info:schemas.infoSchema,
      },
    },    
    time: schemas.timeSchema,
  },
};

export const smfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  nrf: schemas.nrfUiSchema,
  smf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    pfcp:schemas.pfcpUiSchema,
    gtpc:schemas.gtpcUiSchema,
    gtpu:schemas.gtpuUiSchema,
    metrics: schemas.metricsUiSchema,
    subnet:schemas.subnetUiSchema,
    dns:schemas.dnsUiSchema,
    mtu:schemas.mtuUiSchema,
    crf:schemas.crfUiSchema,
    freeDiameter:schemas.freeDiameterUiSchema,
    info:schemas.infoUiSchema,
  },
  time: schemas.timeUiSchema,
};