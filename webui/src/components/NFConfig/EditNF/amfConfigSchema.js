import * as schemas from './EditCfgSection';

export const amfschema = {
  //title: "AMF Configuration",
  type: "object",
  properties: {
    amf: {
      type: "object",
      title: "AMF",
      properties: {
        sbi: schemas.nf_sbi_Schema,
        ngap: schemas.ngapSchema,
        guami: schemas.guamiSchema,
        tai: schemas.taiSchema,
        plmn_support: schemas.plmn_supportSchema,
        security:schemas.securitySchema,
        network_name:schemas.network_nameSchema,
        amf_name:schemas.amf_nameSchema,
        access_control: schemas.access_controlSchema,
        metrics: schemas.metricsSchema,
        //relative_capacity: schemas.relative_capSchema,
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
    },
    //nrf: schemas.nrfSchema,
    //parameter: schemas.paraSchema,
    //sbi: schemas.sbiSchema,
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

export const amfuiSchema = {
  logger: schemas.loggerUiSchema,
  //sbi: schemas.sbiUiSchema,
  //nrf: schemas.nrfUiSchema,
  amf: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    ngap: schemas.ngapUiSchema,
    metrics: schemas.metricsUiSchema,
    guami: schemas.guamiUiSchema,
    tai: schemas.taiUiSchema,
    access_control: schemas.access_controlUiSchema,
    plmn_support: schemas.plmn_supportUiSchema,
    security:schemas.securityUiSchema,
    network_name:schemas.network_nameUiSchema,
    amf_name:schemas.amf_nameUiSchema,
    //relative_capacity: schemas.relative_capUiSchema,
    time: {
      classNames: "col-xs-12",
      nf_instance: schemas.time_nf_instanceUiSchema,
      t3512: schemas.time_t3512UiSchema,
      t3502: schemas.time_t3502UiSchema,
    },
  },
  //parameter: schemas.paraUiSchema,
  global: {
    classNames: "col-xs-12",
    cli: schemas.cliUiSchema,
    parameter: schemas.paraUiSchema,
  }
};