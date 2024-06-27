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
        info: schemas.rtsupiInfoSchema,
        //relative_capacity: schemas.relative_capSchema,
        metrics: schemas.metricsSchema,
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
  //sbi: schemas.sbiUiSchema,
  //nrf: schemas.nrfUiSchema,
  //parameter: schemas.paraUiSchema,
  udm: {
    classNames: "col-xs-12",
    sbi: schemas.nf_sbi_UiSchema,
    hnet: schemas.hnetUiSchema,
    info: schemas.rtsupiInfoUiSchema,
    //relative_capacity: schemas.relative_capUiSchema,
    metrics: schemas.metricsUiSchema,
  },
  global: {
    classNames: "col-xs-12",
    cli: schemas.cliUiSchema,
    parameter: schemas.paraUiSchema,
  }
  //time: {
  //  classNames: "col-xs-12",
  //  instance: schemas.time_instanceUiSchema,
  //},
};