import { Component } from 'react';
import PropTypes from 'prop-types';

import withWidth, { SMALL } from 'helpers/with-width';
import { Form } from 'components';

const schema = {
  "title": "",
  "type": "object",
  "properties": {
    "imei": {
      "type": "string",
      "title": "IMEI*",
      "required": true,
      "pattern": "^\\d+$",
      "maxLength": 15,
      "messages": {
        "pattern": "Only digits are allowed"
      }
    },
    "status": {
      "type": "number",
      "title": "状态*",
      "required": true,
      "enum": [1, 2, 3],      
      "enumNames": ["白名单", "黑名单", "灰名单"],
      default: 0,
    },
    "reason": {
      "type": "number",
      "title": "原因",
      "required": true, 
      "enum": [0, 1, 2, 3, 4, 5],       
      "enumNames": ["  ","丢失", "被偷", "被抢","非法厂商", "未知原因"],
      default: 0,
    },
    "checkflag": {
      "type": "boolean",
      "title": "是否绑定IMSI",
      "required": true,
      default: false,
    },
    "bindimsi": {
      type: "array",
      title:"IMSI",
      items: {
        type: "object",
        properties:{
        "imsi": {
          "type": "string",
          "title": "IMSI", 
          "required": true,     
          "pattern": "^\\d+$",
          "maxLength": 15,
          "messages": {
            "pattern": "Only digits are allowed"
          }
        },
        "msisdn": {
          "type": "string",
          "title": "MSISDN",
          "pattern": "^\\d+$",
          "maxLength": 15,
          "messages": {
            "pattern": "Only digits are allowed"
          }
       
        },
      }
    }
  }
  }
};

function CustomTitle20({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "700",
          fontSize: "20px",
        }}
      >
        {title}
      </label>
    </div>
  );
}

function CustomTitle14({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "700",
          fontSize: "14px",
        }}
      >
        {title}
      </label>
    </div>
  );
}

const myWidget = (props) => {
  const { value, onChange } = props;  
  const enumOptions = props.options.enumOptions || []; 
  const handleChange = (event) => {
    onChange(event.target.value);
  };
  return (
    <select className="form-control" value={value} onChange={handleChange}>
      {enumOptions.map((option) => (
        <option key={option.value} value={option.value}>
          {option.label}
        </option>
      ))}
    </select>
  );
};

const uiSchema = {
  "title" : {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle14 title="Title*" />,
  }, 
  "imei" : {
    classNames: "col-xs-12",    
  },
  "bindimsi" : {
    classNames: "col-xs-12",
    items: {
      classNames: "col-xs-12",
      imsi: {
        classNames: "col-xs-6",        
      },
      msisdn: {
        classNames: "col-xs-6",        
      },
    }    
  }, 
  "status" : {
    classNames: "col-xs-12",
    "ui:widget": myWidget, // 使用下拉选择框
    "ui:options": {
      enumOptions: [
        { value: 1, label: "白名单" },
        { value: 2, label: "黑名单" },
        { value: 3, label: "灰名单" },
      ],      
    },    
  },
  "reason" : {
    classNames: "col-xs-12", 
    "ui:widget": myWidget, // 使用下拉选择框
    "ui:options": {
      enumOptions: [
        { value: 0, label: " " },
        { value: 1, label: "丢失" },
        { value: 2, label: "被偷" },
        { value: 3, label: "被抢" },
        { value: 4, label: "非法厂商" },
        { value: 5, label: "未知原因" },
      ],     
    },     
  },
  "checkflag" : {
    classNames: "col-xs-12",    
  },  
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
    this.state = {
      props: props,
      ...Edit.getStateFromProps(props)
    };
  }

  componentDidUpdate(prevProps) {
    if (prevProps.imeichecks !== this.props.imeichecks) {
      this.checkFirstPage(this.props.imeichecks);
    }
  }

  static getStateFromProps(props) {
    const {    
      action,  
      width,
      formData 
    } = props;
    let state = {
      schema,
      uiSchema,
      formData
    };
    
    if (action === 'update') {
      state.uiSchema = Object.assign(state.uiSchema, {
        "imei": {
          "ui:disabled": true
        },
      });
    } else if (width !== SMALL) {
      state.uiSchema = Object.assign(state.uiSchema, {
        "imei": {
          "ui:autofocus": true
        }
      });
    }

    return state;
  }
  static getDerivedStateFromProps(nextProps, prevState) {  
    if (nextProps !== prevState.props) {
      return {
        props: nextProps,
        ...Edit.getStateFromProps(nextProps),
      };
    }
    return null;
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
        visible={isLoading ? false : visible}
        title={(action === 'update') ? '编辑' : '新增'}
        width="680px"
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
