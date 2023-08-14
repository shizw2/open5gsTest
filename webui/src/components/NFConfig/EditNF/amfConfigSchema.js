import * as schemas from './EditCfgSection';

export const amfschema = {
  title: "AMF Configuration",
  type: "object",
  properties: {
    logger: schemas.loggerSchema,
    sbi: schemas.sbiSchema,
    amf: {
      type: "object",
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
  
};