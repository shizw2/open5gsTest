import * as schemas from './EditCfgSection';

export const smfschema = {
  //title: "SMF Configuration",
  type: "object",
  properties: {
    smf: {
      type: "object",
      title: "SMF",
      properties: {
        sbi: schemas.nf_sbi_Schema,        
        pfcp:schemas.pfcpSchema,
        subnet:schemas.subnetSchema,
        emptyLine1:schemas.emptyLineSchema,//为了在增加pfcp多于subnet的时候，gtpc不跑上来
        gtpc:schemas.gtpcSchema,
        gtpu:schemas.gtpuSchema,
        emptyLine2:schemas.emptyLineSchema,
        metrics: schemas.metricsShortSchema,
        dns:schemas.dnsSchema,
        emptyLine3:schemas.emptyLineSchema,
        mtu:schemas.mtuSchema,
        ctf:schemas.ctfSchema,
        freeDiameter:schemas.freeDiameterSchema,
      },
    },
    nrf: schemas.nrfSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
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
    subnet:schemas.subnetUiSchema,
    emptyLine1:schemas.emptyLineUiSchema,
    gtpc:schemas.gtpcUiSchema,
    gtpu:schemas.gtpuUiSchema,
    emptyLine2:schemas.emptyLineUiSchema,
    metrics: schemas.metricsShortUiSchema,
    dns:schemas.dnsUiSchema,
    emptyLine3:schemas.emptyLineUiSchema,
    mtu:schemas.mtuUiSchema,
    ctf:schemas.ctfUiSchema,
    freeDiameter:schemas.freeDiameterUiSchema,
  },
};