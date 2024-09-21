import * as schemas from './EditCfgSection';

export const upfschema = {
  //title: "UPF Configuration",
  type: "object",
  properties: {
    upf: {
      type: "object",
      title: "UPF",
      properties: {
        pfcp:schemas.upfpfcpSchema,
        gtpu:schemas.gtpuSchema,
        metrics: schemas.metricsSchema,
        emptyLine1:schemas.emptyLineSchema,
        session: schemas.sessionSchema,
        dpdk: schemas.dpdkSchema,
        vxlan: schemas.vxlanSchema,
        nbrlocalserver: schemas.nbrlocalserverSchema,
        nbrlocalclient: schemas.nbrlocalclientSchema,
        nbrremoteserver: schemas.nbrremoteserverSchema,
        //relative_capacity: schemas.relative_capSchema,
      },
    },
    //smf: schemas.smfSchema,
    //parameter: schemas.paraSchema,
    logger: schemas.loggerSchema,
    global: {
      type: "object",
      title: "Global",
      properties: {
        cli: schemas.cliSchema,
        parameter: schemas.paraSchema,
      }
    }
  },
};

export const upfuiSchema = {
  logger: schemas.loggerUiSchema,
  parameter: schemas.paraUiSchema,
  upf: {
    classNames: "col-xs-12",
    pfcp:schemas.upfpfcpUiSchema,
    gtpu:schemas.gtpuUiSchema,
    emptyLine1:schemas.emptyLineUiSchema,
    dpdk: schemas.dpdkUiSchema,
    vxlan: schemas.vxlanUiSchema,
    //relative_capacity: schemas.relative_capUiSchema,
    metrics: schemas.metricsShortUiSchema,
    nbrlocalserver: schemas.nbrlocalserverUiSchema,
    nbrlocalclient: schemas.nbrlocalclientUiSchema,
    nbrremoteserver: schemas.nbrremoteserverUiSchema,
    session: schemas.sessionUiSchema,
  },
  global: {
    classNames: "col-xs-12",
    cli: schemas.cliUiSchema,
    parameter: schemas.paraUiSchema,
  }
  //smf: schemas.smfUiSchema,
};