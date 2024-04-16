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
        gtpu:schemas.gtpuSchema,
        //emptyLine1:schemas.emptyLineSchema,
        //gtpc:schemas.gtpcSchema,  //纯5GC环境中SMF无需配置GTPC地址
        subnet:schemas.subnetSchema,
        //emptyLine2:schemas.emptyLineSchema,
        metrics: schemas.metricsSchema,
        dns:schemas.dnsSchema,
        "p-cscf": schemas.pcscfSchema,
        info: schemas.smfinfoSchema,
        emptyLine3:schemas.emptyLineSchema,//为了在增加metrics配置项多于dns的时候，下面的配置不跑上来
        mtu:schemas.mtuSchema,
        ctf:schemas.ctfSchema,
        freeDiameter:schemas.freeDiameterSchema,
      },
    },
    upf: schemas.upfSchema,
    nrf: schemas.nrfSchema,
    parameter: schemas.paraSchema,
    sbi: schemas.sbiSchema,
    logger: schemas.loggerSchema,
    cli: schemas.cliSchema,
  },
};

export const smfuiSchema = {
  logger: schemas.loggerUiSchema,
  sbi: schemas.sbiUiSchema,
  upf: schemas.upfUiSchema,
  nrf: schemas.nrfUiSchema,
  parameter: schemas.paraUiSchema,
  smf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    pfcp:schemas.pfcpUiSchema,
    subnet:schemas.subnetUiSchema,
    //emptyLine1:schemas.emptyLineUiSchema,
    gtpc:schemas.gtpcUiSchema,
    gtpu:schemas.gtpuUiSchema,
    //emptyLine2:schemas.emptyLineUiSchema,
    metrics: schemas.metricsUiSchema,
    dns:schemas.dnsUiSchema,
    "p-cscf":schemas.pcscfUiSchema,
    info: schemas.smfinfoUiSchema,
    emptyLine3:schemas.emptyLineUiSchema,
    mtu:schemas.mtuUiSchema,
    ctf:schemas.ctfUiSchema,
    freeDiameter:schemas.freeDiameterUiSchema,
  },
  cli: schemas.cliUiSchema,
};