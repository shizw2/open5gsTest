/*import { Component } from 'react';
import PropTypes from 'prop-types';

import withWidth, { SMALL } from 'helpers/with-width';


import EditNRF from './EditNRF'
import EditBSF from './EditBSF'

const Edit = ({ formData }) => {
  console.log('Edit2_id ' + formData._id);

  if (formData._id === 'nfconfig1') {
    console.log('EditNRF');
    return <EditNRF />;
  } else if (formData._id === 'nfconfig2') {
    console.log('EditBSF');
    return <EditBSF />;
  }

  return null;
};

export default withWidth()(Edit);*/

import { Component } from 'react';
import PropTypes from 'prop-types';

import withWidth, { SMALL } from 'helpers/with-width';
import { Form } from 'components';

import { nrfschema, nrfuiSchema } from './EditNF/nrfConfigSchema';
import { bsfschema, bsfuiSchema } from './EditNF/bsfConfigSchema';
import { ausfschema, ausfuiSchema } from './EditNF/ausfConfigSchema';
import { amfschema, amfuiSchema } from './EditNF/amfConfigSchema';
import { smfschema, smfuiSchema } from './EditNF/smfConfigSchema';


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

    this.state = this.getStateFromProps(props);
  }

  componentWillReceiveProps(nextProps) {
    this.setState(this.getStateFromProps(nextProps));
  }

  getStateFromProps(props) {
    const { 
      action,
      width,
      formData
    } = props;

 
    let selectedSchema;
    let selectedUiSchema;

    if (formData._id === 'nrf') {
      selectedSchema = nrfschema;
      selectedUiSchema = nrfuiSchema;
    } else if (formData._id === 'bsf') {
      selectedSchema = bsfschema;
      selectedUiSchema = bsfuiSchema;
    }else if (formData._id === 'ausf') {
      selectedSchema = ausfschema;
      selectedUiSchema = ausfuiSchema;
    }else if (formData._id === 'amf' || formData._id === 'a') {
      selectedSchema = amfschema;
      selectedUiSchema = amfuiSchema;
    }else if (formData._id === 'smf') {
      selectedSchema = smfschema;
      selectedUiSchema = smfuiSchema;
    }else {
      // 默认情况下，选择一个适当的“fallback”模式和 UI 模式
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
        visible={visible}
        title={(action === 'update') ? 'Edit NFConfig' : 'Create NFConfig'}
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
