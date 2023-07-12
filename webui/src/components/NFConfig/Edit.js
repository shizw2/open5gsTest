import { Component } from 'react';
import PropTypes from 'prop-types';

import withWidth, { SMALL } from 'helpers/with-width';
import { Form } from 'components';

const schema = {
  "title": "NFConfig Configuration",
  "type": "object",
  "properties": {
    "title": {
      "type": "string",
      "title": "Title*",
      "required": true,
      "maxLength": 24
    },
    "msisdn": {
      "type": "array",
      "title": "",
      "maxItems": 2,
      "messages": {
        "maxItems": "2 MSISDN are supported"
      },
      "items": {
        "type": "string",
        "title": "MSISDN",
        "maxLength": 15,
        "required": true,
        "pattern": "^\\d+$",
        "messages": {
          "pattern": "Only digits are allowed"
        }
      }
    }
  }
};

const uiSchema = {
  "title" : {
    classNames: "col-xs-12",
  },
  "msisdn" : {
    classNames: "col-xs-7",
  }
}

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

export default withWidth()(Edit);
