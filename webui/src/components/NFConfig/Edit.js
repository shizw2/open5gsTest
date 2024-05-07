import { Component } from 'react';
import PropTypes from 'prop-types';

import withWidth, { SMALL } from 'helpers/with-width';
import { Form } from 'components';

import { nrfschema, nrfuiSchema } from './EditNF/nrfConfigSchema';
import { bsfschema, bsfuiSchema } from './EditNF/bsfConfigSchema';
import { ausfschema, ausfuiSchema } from './EditNF/ausfConfigSchema';
import { amfschema, amfuiSchema } from './EditNF/amfConfigSchema';
import { smfschema, smfuiSchema } from './EditNF/smfConfigSchema';
import { nssfschema, nssfuiSchema } from './EditNF/nssfConfigSchema';
import { pcfschema, pcfuiSchema } from './EditNF/pcfConfigSchema';
import { udmschema, udmuiSchema } from './EditNF/udmConfigSchema';
import { upfschema, upfuiSchema } from './EditNF/upfConfigSchema';
import { udrschema, udruiSchema } from './EditNF/udrConfigSchema';
import { eirschema, eiruiSchema } from './EditNF/eirConfigSchema';

class Edit extends Component {
  static propTypes = {
    visible: PropTypes.bool,
    action: PropTypes.string,
    formData: PropTypes.object,
    isLoading: PropTypes.bool,
    validate: PropTypes.func,
    onHide: PropTypes.func,
    onSubmit: PropTypes.func,
    onError: PropTypes.func
  }

  constructor(props) {
    super(props);
    this.state = {
      props: props,
      ...Edit.getStateFromProps(props)
    };
   // this.state = this.getStateFromProps(props);
  }
 /*
  componentWillReceiveProps(nextProps) {
    this.setState(this.getStateFromProps(nextProps));
  }*/
  static getDerivedStateFromProps(nextProps, prevState) {
    if (nextProps !== prevState.props) {
      return {
        props: nextProps,
        ...Edit.getStateFromProps(nextProps),
      };
    }
    return null;
  }
  
 static getStateFromProps(props) {
    const { 
      action,
      width,
      formData
    } = props;

 
    let selectedSchema;
    let selectedUiSchema;

    if (formData.parameter && formData.parameter.capacity)
      formData.parameter.capacity = parseInt(formData.parameter.capacity);

    if (formData._id === 'nrf' || formData._nf === 'nrf') {
      selectedSchema = nrfschema;
      selectedUiSchema = nrfuiSchema;
    } else if (formData._id === 'bsf' || formData._nf === 'bsf') {
      selectedSchema = bsfschema;
      selectedUiSchema = bsfuiSchema;
    }else if (formData._id === 'ausf'  || formData._nf === 'ausf') {
      selectedSchema = ausfschema;
      selectedUiSchema = ausfuiSchema;
    }else if (formData._id === 'amf' || formData._nf === 'amf') {        
      //初始配置文件会按数字读取mcc,mnc,将读取到的mcc,mnc的类型改为string
      if (formData.amf&&formData.amf.guami&&Array.isArray(formData.amf.guami)) {
        formData.amf.guami.forEach(guami => {         
          if (typeof guami.plmn_id.mcc === 'number') {
            guami.plmn_id.mcc = String(guami.plmn_id.mcc);
          }
          if (typeof guami.plmn_id.mnc === 'number') {
            guami.plmn_id.mnc = String(guami.plmn_id.mnc);
          }
        });
      }

      if (formData.amf&&formData.amf.tai&&Array.isArray(formData.amf.tai)) {
        formData.amf.tai.forEach(tai => {         
          if (typeof tai.plmn_id.mcc === 'number') {
            tai.plmn_id.mcc = String(tai.plmn_id.mcc);
          }
          if (typeof tai.plmn_id.mnc === 'number') {
            tai.plmn_id.mnc = String(tai.plmn_id.mnc);
          }
          const tacArray = tai.tac;
          const tacStringArray = tacArray.map(item => String(item));
          tai.tac = tacStringArray;
        });
      }

      if (formData.amf&&formData.amf.plmn_support&&Array.isArray(formData.amf.plmn_support)) {
        formData.amf.plmn_support.forEach(plmn_support => {         
          if (typeof plmn_support.plmn_id.mcc === 'number') {
            plmn_support.plmn_id.mcc = String(plmn_support.plmn_id.mcc);
          }
          if (typeof plmn_support.plmn_id.mnc === 'number') {
            plmn_support.plmn_id.mnc = String(plmn_support.plmn_id.mnc);
          }
          if (plmn_support.s_nssai)
          {
            plmn_support.s_nssai.forEach(s_nssai => {  
              if (s_nssai.sd)
                s_nssai.sd = String(s_nssai.sd);
            })
          }
        });
      }

      if (formData.amf && formData.amf.relative_capacity)
        formData.amf.relative_capacity = parseInt(formData.amf.relative_capacity);

      selectedSchema = amfschema;
      selectedUiSchema = amfuiSchema;
    }else if (formData._id === 'smf' || formData._nf === 'smf') {
      //初始配置文件会按数字读取mcc,mnc,将读取到的mcc,mnc的类型改为string
      if (formData.smf&&formData.smf.info&&Array.isArray(formData.smf.info)) {
        formData.smf.info.forEach(info => {
          if (info.s_nssai&&Array.isArray(info.s_nssai)){
            info.s_nssai.forEach(s_nssai => {
              if (typeof s_nssai.sd === 'number') {
                s_nssai.sd = String(s_nssai.sd);
              }
            })
          }

          if (info.tai&&Array.isArray(info.tai)){
            info.tai.forEach(tai => {
              if (typeof tai.plmn_id.mcc === 'number') {
                tai.plmn_id.mcc = String(tai.plmn_id.mcc);
              }
              if (typeof tai.plmn_id.mnc === 'number') {
                tai.plmn_id.mnc = String(tai.plmn_id.mnc);
              }
              const tacArray = tai.tac;
              const tacStringArray = tacArray.map(item => String(item));
              tai.tac = tacStringArray;
            })
          }
        })
      };

      selectedSchema = smfschema;
      selectedUiSchema = smfuiSchema;
    }else if (formData._id === 'nssf' || formData._nf === 'nssf') {
      selectedSchema = nssfschema;
      selectedUiSchema = nssfuiSchema;
    }else if (formData._id === 'pcf' || formData._nf === 'pcf') {   
      if (formData.pcf&&formData.pcf.freeDiameter &&formData.pcf.freeDiameter.load_extension
        &&Array.isArray(formData.pcf.freeDiameter.load_extension)) {          
          formData.pcf.freeDiameter.load_extension.forEach(load_extension => {               
          if (typeof load_extension.conf === 'number') {//初次读取配置文件时,会将十六进制字符按数字读取。这里转回十六进制字符串
            load_extension.conf =  "0x" + load_extension.conf.toString(16);           
          }          
        });
      }
      selectedSchema = pcfschema;
      selectedUiSchema = pcfuiSchema;
    }else if (formData._id === 'udm' || formData._nf === 'udm') {
      selectedSchema = udmschema;
      selectedUiSchema = udmuiSchema;
    }else if (formData._id === 'upf' || formData._nf === 'upf') {
      if (formData.upf && formData.upf.dpdk && formData.upf.dpdk.pfcp_lcore)
        formData.upf.dpdk.pfcp_lcore = parseInt(formData.upf.dpdk.pfcp_lcore);
      
      if (formData.upf && formData.upf.dpdk && formData.upf.dpdk.dpt_lcore)
        formData.upf.dpdk.dpt_lcore = parseInt(formData.upf.dpdk.dpt_lcore);
      
      if (formData.upf && formData.upf.dpdk && formData.upf.dpdk.fwd_lcore)
        formData.upf.dpdk.fwd_lcore = parseInt(formData.upf.dpdk.fwd_lcore);
      
      selectedSchema = upfschema;
      selectedUiSchema = upfuiSchema;
    }else if (formData._id === 'udr' || formData._nf === 'udr') {
      selectedSchema = udrschema;
      selectedUiSchema = udruiSchema;
    }else if (formData._id === 'eir' || formData._nf === 'eir') {
      selectedSchema = eirschema;
      selectedUiSchema = eiruiSchema;
    }else {
      // 默认情况下，选择一个适当的“fallback”模式和 UI 模式
      console.log("Edit Default");     
      selectedSchema = nrfschema;
      selectedUiSchema = nrfuiSchema;
   }

    let state = {
      schema: selectedSchema,
      uiSchema: selectedUiSchema
    };
    
    if (action === 'update') {
      state = {
        ...state,
        uiSchema : {
          ...selectedUiSchema,
          "title": {
            "ui:disabled": true
          }
        }
      }
    } else if (width !== SMALL) {
      state = {
        ...state,
        uiSchema : {
          ...selectedUiSchema,
          "title": {
            "ui:autofocus": true
          }
        }
      }
    }

    return state;
  }

  render() {
    const {
      visible,
      action,
      formData,
      isLoading,
      validate,
      onHide,
      onSubmit,
      onError
    } = this.props;

    return (
      <Form 
        visible={isLoading?false:visible}//解决页面超时后空白的问题
        title={(action === 'update') ? `Edit  ${String(formData._name).toUpperCase()} Configuration` : `Create ${String(formData._name).toUpperCase()} Configuration`}
        schema={this.state.schema}
        uiSchema={this.state.uiSchema}
        formData={formData}
        isLoading={isLoading}
        validate={validate}
        onHide={onHide}
        onSubmit={onSubmit}
        onError={onError}/>
    )
  }
}

export default withWidth()(Edit);
