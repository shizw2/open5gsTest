import { Component } from 'react';
import PropTypes from 'prop-types';

import withWidth, { SMALL } from 'helpers/with-width';
import { Form } from 'components';

const schema = {
  "title": "NRF Configuration",
  "type": "object",
  "properties": {
    /*"title": {
      "type": "string",
      "title": "Title*",
      "required": true,
      "maxLength": 24
    },  */  
    "logger": {
      "type": "object",
      "properties": {
        "file": {
          "type": "string",
          "title": "Log File",
          "default": ""
        },
        "level": {
          "type": "string",
          "title": "Log Level",
          "enum": ["debug", "info", "warning", "error"],
          "default": "info"
        }
      }
    },
    "nrf": {
      "type": "object",
      "title": "NRF Configuration",
      "properties": {
        "sbi": {
          "type": "object",
          "title": "SBI Configuration",
          "properties": {
            "addr": {
              "type": "array",
              "title": "Addresses",
              "items": {
                "type": "string",
                "title": "Address"
              }
            },
            "port": {
              "type": "number",
              "title": "Port"
            }
          }
        }
      }
    }
  }
};

const uiSchema = {
  /*"title" : {
    classNames: "col-xs-12",
  },*/
  "msisdn" : {
    classNames: "col-xs-7",
  },
  "logger" : {
    classNames: "col-xs-7",
  },
  /*"nrf": {
    "sbi": {
      "addr": {
        "classNames": "col-xs-7"
      },
      "port": {
        "classNames": "col-xs-7"
      }
    }
  }*/
  "nrf": {
    classNames: "col-xs-12",
    "sbi": {
      classNames: "col-xs-12",
      "addr": {
        classNames: "col-xs-12"
      },
      "port": {
        classNames: "col-xs-12"
      }
    }
  }
}

class EditBSF extends Component {
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
    } = props;

    let state = {
      schema,
      uiSchema
    };
    
    if (action === 'update') {
      state = {
        ...state,
        uiSchema : {
          ...uiSchema,
          "title": {
            "ui:disabled": true
          }
        }
      }
    } else if (width !== SMALL) {
      state = {
        ...state,
        uiSchema : {
          ...uiSchema,
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

export default withWidth()(EditBSF);
