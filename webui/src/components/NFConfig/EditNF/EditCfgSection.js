export const loggerSchema = {
  type: "object",
  title: "Logger",
  properties: {
    level: {
      type: "string",
      title: "Log Level",
      enum: ["fatal", "error", "warn", "info", "debug", "trace"],
      default: "info",
    },
    file: {
      type: "string",
      title: "Log File",
      default: "",
    },
  },
};

export const loggerUiSchema = {
  classNames: "col-xs-12",
  file: {
    classNames: "col-xs-8",
  },
  level: {
    classNames: "col-xs-4",
  },
};

export const sbiSchema = {
  type: "object",
  title: "SBI Security",
  properties: {
    server: {
      type: "object",
      title:"Server",
      properties: {
        no_tls: {
          type: "boolean",
          title: "No TLS",
          default: true,
        },
        no_verify: {
          type: "boolean",
          title: "No Verify",
          default: true,
        },
        /* 此部分在界面上暂不呈现
        cacert: {
          type: "string",
          title: "CA Cert",
          default: "",
        },
        key: {
          type: "string",
          title: "Key",
          default: "",
        },
        cert: {
          type: "string",
          title: "Cert",
          default: "",
        },
        */
      },
    },
    client: {
      type: "object",
      title:"Client",
      properties: {
        no_tls: {
          type: "boolean",
          title: "No TLS",
          default: true,
        },
        no_verify: {
          type: "boolean",
          title: "No Verify",
          default: true,
        },
        /* 此部分在界面上暂不呈现
        cacert: {
          type: "string",
          title: "CA Cert",
          default: "",
        },
        key: {
          type: "string",
          title: "Key",
          default: "",
        },
        cert: {
          type: "string",
          title: "Cert",
          default: "",
        },
        */
      },
    },
  },
};

export const sbiUiSchema = {
  classNames: "col-xs-12",
    server: {
      classNames: "col-xs-6",
      "ui:title": <CustomTitle18 title="Server" />,
      no_tls: {
        classNames: "col-xs-6",
      },
      no_verify: {
        classNames: "col-xs-6",
      },
      /* 此部分在界面上暂不呈现
      cacert: {
        classNames: "col-xs-6",
      },
      key: {
        classNames: "col-xs-6",
      },
      cert: {
        classNames: "col-xs-6",
      },
      */
  },
  client: {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Client" />,
    no_tls: {
      classNames: "col-xs-6",
    },
    no_verify: {
      classNames: "col-xs-6",
    },
    /* 此部分在界面上暂不呈现
    cacert: {
      classNames: "col-xs-6",
    },
    key: {
      classNames: "col-xs-6",
    },
    cert: {
      classNames: "col-xs-6",
    },
    */
  },
};

export const nrfSchema = {
  type: "object",
  title:"NRF",
  properties: {
    sbi: {
      type: "array",
      title: "SBI Interface",
      items: {
        type: "object",
        properties: {
          addr: {
            type: "array",
            title:"IP Address",
            minItems: 1,
            items: {
              type: "string",
              anyOf: [
                { format: "ipv4" },
                { format: "ipv6" },
              ],
              messages: {
                "anyOf": "IPv4 or IPv6 allowed"
              },
              required: true,
            },
            //description: "Please enter a valid IPv4/IPv6 Address",
            required: true,
          },
          port: {
            type: "number",
            title: "Port",
            required: true,
          },
        },
      },
      //required:["addr","port"],
    },
  },
  //required:["addr","port"],
};

export const nrfUiSchema = {
  classNames: "col-xs-12",
  sbi: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="SBI Interface" />,
    items: {
      classNames: "col-xs-12",
      addr: {
        classNames: "col-xs-8",
        "ui:title": <CustomTitle14 title="IP Address" />,
        items:{
          //classNames: "col-xs-7",
          classNames: "col-xs-12",
          //"ui:help": "Enter a valid IPv4/IPv6 Address",
          //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
        }
      },
      port: {
        classNames: "col-xs-4",
        "ui:title": <CustomTitle14Border14 title="Port" />,
      },
    },
  },
};


export const nf_sbi_Schema = {
  type: "array",
  title:"SBI Interface",
  "minItems": 1,
  "maxItems": 4,
  "messages": {
    "minItems": "At least 1 Interface is required",
    "maxItems": "4 Interfaces are supported"
  },
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" },
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        //format:"ipv4",
        //description: "Please enter a valid IPv4/IPv6 Address",
        required: true,
        //default: "127.0.0.5"
      },
      port: {
        type: "number",
        title: "Port",
        required: true,
        //default: 7777
      }
    }
  }
};

export const nf_sbi_UiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="SBI Interface" />,
  items: {
    addr: {
      classNames: "col-xs-8",
      //"ui:help": "Enter a valid IPv4/IPv6 Address",
      //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
    },
    port: {
      classNames: "col-xs-4",
    }
  }
};

export const nf_sbi_shortSchema = {
  type: "array",
  title:"SBI Interface",
  "minItems": 1,
  "maxItems": 4,
  "messages": {
    "minItems": "At least 1 Interface is required",
    "maxItems": "4 Interfaces are supported"
  },
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" },
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        //format:"ipv4",
        //description: "Please enter a valid IPv4/IPv6 Address",
        required: true,
        //default: "127.0.0.5"
      },
      port: {
        type: "number",
        title: "Port",
        required: true,
        //default: 7777
      }
    }
  }
};

export const nf_sbi_shortUiSchema = {
  classNames: "col-xs-7",
  "ui:title": <CustomTitle18 title="SBI Interface" />,
  items: {
    addr: {
      classNames: "col-xs-7",
      //"ui:help": "Enter a valid IPv4/IPv6 Address",
      //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
    },
    port: {
      classNames: "col-xs-5"
    }
  }
};

//对于没有配置time的网元，使用timeSchema/timeUiSchema
export const timeSchema = {  
};

// UiSchema
export const timeUiSchema = {    
};

//对于配置了具体内容的time,使用下面的Schema/UiSchema,如time_nf_instanceSchema,time_t3512Schema
export const time_nf_instanceSchema = {
  type: "object",
  title:"NF_Instance", 
  properties: {
    heartbeat: {
      type: "number",
      title: "HeartBeat",
      default: 20
    }
  },
};

// UiSchema
export const time_nf_instanceUiSchema = {  
  classNames: "col-xs-4",
  "ui:title": <CustomTitle18 title="NF_Instance" />,
  heartbeat: {
    classNames: "col-xs-12"
  }  
};

export const time_t3512Schema = {
  type: "object",
  title: "T3512",
  properties: {
    value: {
      type: "number",
      title: "Value",
      default: 540
    }
  }
};

export const time_t3512UiSchema = {
  classNames: "col-xs-4",
  "ui:title": <CustomTitle18 title="T3512" />,
  value: {
    classNames: "col-xs-12",
  } 
};

export const time_t3502Schema = {
  type: "object",
  title: "T3502",
  properties: {
    value: {
      type: "number",
      title: "Value",
      default: 720
    }
  }
};

export const time_t3502UiSchema = {
  classNames: "col-xs-4",
  "ui:title": <CustomTitle18 title="T3502" />,
  value: {
    classNames: "col-xs-12",
  } 
};

// ICPS Schema
export const icpsSchema = {
  type: "object",
  properties: {
    spsnum: {
      type: "number",
      default: 3
    },
    port: {
      type: "number",
      default: 9777
    }
  },
  required: ["spsnum", "port"]
};

// UI Schema
export const icpsUiSchema = {
  spsnum: {
    classNames: "col-xs-12"
  },
  port: {
    classNames: "col-xs-12"
  }
};

export const ngapSchema = {
  type: "array",
  title: "NGAP",
  "minItems": 1,
  "maxItems": 4,
  "messages": {
    "minItems": "At least 1 IP is required",
    "maxItems": "4 IPs are supported"
  },
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        default: "127.0.0.5",
        required: true,
      },
    }
  }
};

export const ngapUiSchema = {
  classNames: "col-xs-5",
  "ui:title": <CustomTitle18 title="NGAP" />,
  items: {
    //classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12",
    },
  },
};

export const metricsSchema = {
  type: "array",
  title: "Metrics",
  "minItems": 1,
  "maxItems": 4,
  "messages": {
    "minItems": "At least 1 Interface is required",
    "maxItems": "4 Interfaces are supported"
  },
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        default: "127.0.0.5"
      },
      port: {
        type: "number",
        title: "Port",
        default: 9090
      }
    }
  }
};

export const metricsUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Metrics" />,
  //items: {
    //classNames: "col-xs-12",//层次感  
    //metrics: {
      //classNames: "col-xs-12",
      items: {
        //classNames: "col-xs-12",
        addr: {
          classNames: "col-xs-8",
        },
        port: {
          classNames: "col-xs-4",
        },
      //},
    //},
  },
};

export const metricsShortSchema = {
  type: "array",
  //title: "Metrics",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        default: "127.0.0.5"
      },
      port: {
        type: "number",
        title: "Port",
        default: 9090
      }
    }
  }
};

export const metricsShortUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="Metrics" />,
  //items: {
    //classNames: "col-xs-12",//层次感  
    //metrics: {
      //classNames: "col-xs-12",
      items: {
        //classNames: "col-xs-12",
        addr: {
          classNames: "col-xs-8",
        },
        port: {
          classNames: "col-xs-4",
        },
      //},
    //},
  },
};

export const guamiSchema = {
  type: "array",
  title: "GUAMI",
  "minItems": 1,
  "maxItems": 4,
  "messages": {
    "minItems": "At least 1 Item is required",
    "maxItems": "4 Items are supported"
  },
  items: {
    type: "object",
    properties: {
      plmn_id: {
        type: "object",
        title: "PLMN_ID",
        properties: {
          mcc: {
            type: "string",
            title: "MCC",
            maxLength: 3,
            required: true,
            pattern: "^\\d+$",
            messages: {
              pattern: "Only digits are allowed"
            }
          },
          mnc: {
            type: "string",
            title: "MNC",
            maxLength: 3,
            required: true,
            pattern: "^\\d+$",
            messages: {
              pattern: "Only digits are allowed"
            }
          }
        },
        //required: ["mcc", "mnc"]
      },
      amf_id: {
        type: "object",
        title: "AMF_ID",
          properties: {
          region: {
            type: "integer",
            title: "Region ID",
            minimum: 1,
            maximum: 255,
            messages: {
              minimum: "must >= 1",
              maximum: "must <= 255",
            },
            required: true,
          },
          set: {
            type: "integer",
            title: "Set ID",
            minimum: 1,
            maximum: 1023,
            messages: {
              minimum: "must >= 1",
              maximum: "must <= 1023",
            },
            required: true,
          }
        },
        //required: ["region", "set"]
      }
    },
    //required: ["plmn_id", "amf_id"]
  }
};

export const guamiUiSchema = {
  classNames: "col-xs-12",   //增加这个，guami元素才显示
  "ui:title": <CustomTitle18 title="GUAMI" />,
  items: {
    plmn_id: {
      classNames: "col-xs-6",//增加这个，体现层次感
      "ui:title": <CustomTitle18 title="PLMN_ID" />,
      mcc: {
        classNames: "col-xs-6"
      },
      mnc: {
        classNames: "col-xs-6"
      }
    },
    amf_id: {
      classNames: "col-xs-6",
      "ui:title": <CustomTitle18 title="AMF_ID"/>,
      region: {
        classNames: "col-xs-6"
      },
      set: {
        classNames: "col-xs-6"
      }
    }
  }
};

export const taiSchema = {
  type: "array",
  title: "TAI",
  "minItems": 1,
  "maxItems": 8,
  "messages": {
    "minItems": "At least 1 Item is required",
    "maxItems": "8 Items are supported"
  },
  items: {
    type: "object",
    properties: {
      plmn_id: {
        type: "object",
        title: "PLMN_ID",
        properties: {
          mcc: {
            type: "string",
            title: "MCC",
            maxLength: 3,
            required: true,
            pattern: "^\\d+$",
            messages: {
              pattern: "Only digits are allowed"
            }
          },
          mnc: {
            type: "string",
            title: "MNC",
            maxLength: 3,
            required: true,
            pattern: "^\\d+$",
            messages: {
              pattern: "Only digits are allowed"
            }
          }
        },
        //required: ["mcc", "mnc"]
      },
      tac: {
        type: "array",
        title: "TAC",
        items: {
          type: "string",
          pattern: /^(?:\d+|\d+-\d+)$/,
  	      messages: {
            pattern: "Format like 1 or 3-5",
            type: "Format like 1 or 3-5",
          },
          required: true,
        }
      }
      /*
      tac: {
        type: "string",
        title: "TAC",
        pattern: /^(\d+|\[\d+(?:-\d+)?(?:, \d+(?:-\d+)?)*])$/,
        required: true,
        messages: {
          pattern: "Format like 1 or [1, 3-8, 10, 12-15]"
        }
      }
      */
    },
    //required: ["plmn_id", "tac"]
  }
};

export const taiUiSchema = {
  classNames: "col-xs-12", 
  "ui:title": <CustomTitle18 title="TAI" />,
  items: {
    plmn_id: {
      classNames: "col-xs-7", //增加这个，体现层次感
      "ui:title": <CustomTitle18 title="PLMN_ID" />,
      mcc: {
        classNames: "col-xs-6"
      },
      mnc: {
        classNames: "col-xs-6"
      }
    },
    tac: {
      classNames: "col-xs-5",
      "ui:title": <CustomTitle18 title="TAC" />,
      //"ui:title": <CustomTitle18Margin45 title="TAC" />,
      items:{
        "ui:placeholder": "Format like 1 or 3-5",
      }
    }
  }
};

export const plmn_supportSchema = {
  type: "array",
  title: "PLMN_SUPPORT",
  items: {
    type: "object",
    properties: {
      plmn_id: {
        type: "object",
        title: "PLMN_ID",
        properties: {
          mcc: {
            type: "string",
            title: "MCC",
            maxLength: 3,
            //required: true,
            pattern: "^\\d+$",
            messages: {
              pattern: "Only digits are allowed"
            }
          },
          mnc: {
            type: "string",
            title: "MNC",
            maxLength: 3,
            //required: true,
            pattern: "^\\d+$",
            messages: {
              pattern: "Only digits are allowed"
            }
          }
        },
        required: ["mcc", "mnc"]
      },
      s_nssai: {
        type: "array",
        title: "S-NSSAI",
        minItems: 1,
        maxItems: 8,
        items: {
          type: "object",
          properties: {
            sst: {
              type: "number",
              title: "SST*",
              enum: [ 1, 2, 3, 4 ],
              required:true,
              default: 1,
            },
            sd: {
              type: "string",
              title: "SD",
              pattern: "^[0-9a-fA-F]+$",
              minLength: 6,
              maxLength: 6,
              messages: {
                "pattern": "Only hexadecimal digits are allowed"
              }
            }
          },
          //required: ["sst"]
        },
      },
    },
  },
};

export const plmn_supportUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="PLMN_Support" />,
  items: {
    plmn_id: {
      classNames: "col-xs-4",
      "ui:title": <CustomTitle18 title="PLMN_ID" />,
      mcc: {
        classNames: "col-xs-6"
      },
      mnc: {
        classNames: "col-xs-6"
      }
    },
    s_nssai: {
      classNames: "col-xs-8",
      "ui:title": <CustomTitle18 title="S_NSSAI" />,
      items: {
        sst: {
          classNames: "col-xs-7",
          "ui:widget": "radio",
          "ui:options": { "inline": true },
        },
        sd: {
          classNames: "col-xs-5",
        },
      }
    }
  }
};

export const securitySchema = {
  type: "object",
  title: "Security",
  properties: {
    integrity_order: {
      type: "array",
      title: "Integrity Order",
      maxItems: 3,
      items: {
        type: "string",
        enum: ["NIA0", "NIA1", "NIA2"],
        default: "NIA0",
        //require: true
      },
      contains: {
        type: "string",
        format: "unique"
      }
    },
    ciphering_order: {
      type: "array",
      title: "Ciphering Order",
      maxItems: 3,
      items: {
        type: "string",
        enum: ["NEA0", "NEA1", "NEA2"],
        default: "NEA2",
      },
      contains: {
        type: "string",
        format: "unique"
      }
    }
  }
};

export const securityUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Security" />,
  integrity_order: {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Integrity Order" />,
  },
  ciphering_order: {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Ciphering Order" />,
  }
};

export const network_nameSchema = {
  type: "object",
  title:"Network Name",
  properties: {
    full: {
      type: "string",
      title: "FULL Name",
    },
    short: {
      type: "string",
      title: "Short Name",
    }
  },
};

export const network_nameUiSchema = {
  classNames: "col-xs-8",	
  "ui:title": <CustomTitle18 title="Network Name" />,
  full: {
    classNames: "col-xs-6",	  
    "ui:placeholder": "Enter the full name",
  },
  short: {
    classNames: "col-xs-6",	  
    "ui:placeholder": "Enter the short name",
  }
};

export const amf_nameSchema = {
  type: "string",
  title: "AMF Name",
};

export const amf_nameUiSchema = {
  classNames: "col-xs-4",
  "ui:title": <CustomTitle18Border39 title="AMF Name" />,
};

export const pfcpSchema = {
  type: "array",
  title: "PFCP Address",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        default: "127.0.0.5",
      },
    }
  }
};

export const pfcpUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="PFCP Address" />,
  //pfcp: {  //不需要再加一层。加了反而没层次感  
  items: {
    classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12",
      //"ui:widget": "hidden",
      "ui:options": {
        "label": false
      }
    }
  }  
};

export const gtpcSchema = {
  type: "array",
  title: "GTPC Address",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        default: "127.0.0.5"
      },
    }
  }
};

export const gtpcUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="GTPC Address" />,
  items: {
    classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12",
      "ui:options": {
        "label": false
      }
    }
  } 
};

export const gtpuSchema = {
  type: "array",
  title: "GTPU Address",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        default: "127.0.0.5"
      },
    }
  }
};

export const gtpuUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="GTPU Address" />,
  items: {
    classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12",
      "ui:options": {
        "label": false
      }
    }
  }
};

const subnetipv4Regex = /^(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\/\d{1,2})$/;
//const subnetipv6Regex = /^((?:[A-Fa-f0-9]{1,4}:){6}(?:[A-Fa-f0-9]{1,4}:|[A-Fa-f0-9]\.)(?:\d{1,3}\.){3}\d{1,3}\/\d{1,3}|(?:[A-Fa-f0-9]{1,4}:){7}[A-Fa-f0-9]{1,4})$/;
//const subnetipv6Regex = /^([0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4}$/;
//const subnetipv6Regex = /^(?:[A-F0-9]{1,4}:){7}[A-F0-9]{1,4}(?:\/\d{1,3})?$/i;
//const subnetipv6Regex = /^([A-Fa-f0-9]{1,4}(::)?){1,7}[A-Fa-f0-9]{1,4}\/\d{1,3}$/;
//const subnetipv6Regex = /^(?:[A-Fa-f0-9]{1,4}:){7}[A-Fa-f0-9]{1,4}(?:\/\d{1,3})?$/;
const subnetipv6Regex = /^([a-f0-9]{1,4}(:[a-f0-9]{1,4}){7}|[a-f0-9]{1,4}(:[a-f0-9]{1,4}){0,7}::[a-f0-9]{0,4}(:[a-f0-9]{1,4}){0,7})(?:\/\d{1,3})$/

export const subnetSchema = {
  type: "array",
  title: "Subnet Address",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "IP Address",
        anyOf: [
          { pattern: subnetipv4Regex.source },
          { pattern: subnetipv6Regex.source }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        default: "10.45.0.1/16"
      },
    }
  }
};

export const subnetUiSchema = {
  classNames: "col-xs-6",
  //删除无用代码,反而有层次感
  "ui:title": <CustomTitle18 title="Subnet Address" />,
  items: {
    classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12",
      "ui:placeholder": "ipv4/v6 subnet address",
      "ui:options": {
        "label": false
      }
    }
  }  
};

export const dnsSchema = {
  type: "array",
  title:"DNS",
  items: {
    type: "string"
  }
};

export const dnsUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="DNS" />,
  dns: {
    classNames: "col-xs-12"
  }
};

const HiddenField = () => {
  return null;
};

export const emptyLineSchema = {
  type: "number",
};

export const emptyLineUiSchema = {
  classNames: "col-xs-12",
  "ui:field": HiddenField,
 };

export const mtuSchema = {
  type: "number",
  title: "MTU",
  minimum: 0
};

export const mtuUiSchema = {
  classNames: "col-xs-3",
  "ui:title": <CustomTitle18Border15 title="MTU" />,
};

export const ctfSchema = {
  type: "object",
  title: "CTF Gy Enabled",
  properties: {
    enabled: {
      type: "string",
      title: "",
      enum: ["auto", "yes", "no"],
      default: "auto"
    }
  },
  required: ["enabled"]
};

export const ctfUiSchema = {
  classNames: "col-xs-3",
  "ui:title": <CustomTitle18 title="CTF Gy Enabled" />,
  enabled: {
    classNames:"col-xs-12",
    "ui:options": {
      "label": false
    }
  }
};

export const freeDiameterSchema = {
  type: "string"
};

export const freeDiameterUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18Border15 title="Diameter Config" />,
};


export const infoSchema = {
  type: "array",
  items: {
    type: "object",
    properties: {
      s_nssai: {
        type: "array",
        items: {
          type: "object",
          properties: {
            sst: {
              type: "number"
            },
            dnn: {
              type: "array",
              items: {
                type: "string"
              }
            }
          },
          required: ["sst", "dnn"]
        }
      },
      tai: {
        type: "array",
        items: {
          type: "object",
          properties: {
            plmn_id: {
              type: "object",
              properties: {
                mcc: {
                  type: "number"
                },
                mnc: {
                  type: "number"
                }
              },
              required: ["mcc", "mnc"]
            },
            tac: {
              type: "number"
            }
          },
          required: ["plmn_id", "tac"]
        }
      }
    }
  }
};

export const infoUiSchema = {
  classNames: "col-xs-12",
  items: {
    classNames: "col-xs-12",
    s_nssai: {
      classNames: "col-xs-12",
      items: {
        sst: {
          classNames: "col-xs-12"
        },
        dnn: {
          classNames: "col-xs-12"
        }
      }
    },
    tai: {
      classNames: "col-xs-12",
      items: {
        plmn_id: {
          classNames: "col-xs-12",
          mcc: {
            classNames: "col-xs-12"
          },
          mnc: {
            classNames: "col-xs-12"
          }
        },
        tac: {
          classNames: "col-xs-12"
        }
      }
    }
  }
};

export const nsiSchema = {
  type: "array",
  title:"NSI",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title:"IP Address",
        anyOf: [
            { format: "ipv4" },
            { format: "ipv6" }
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
        required: true
      },
      port: {
        type: "number",
        title:"Port",
        required:true,
      },
      s_nssai: {
        type: "object",
        title:"S_NSSAI",
        properties: {
          sst: {
            type: "number",
            title: "SST*",
            enum: [ 1, 2, 3, 4 ],
            required:true,
          },
          sd: {
            type: "string",
            title: "SD",
            pattern: "^[0-9a-fA-F]+$",
            minLength: 6,
            maxLength: 6,
            messages: {
              "pattern": "Only hexadecimal digits are allowed"
            }
          }
        },
        //required: ["sst"]
      }
    },
    //required: ["addr", "port", "s_nssai"]
  }
};

export const nsiUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="NSI" />,
  items: {
    addr: {
      classNames: "col-xs-4",
      "ui:title": <CustomTitle14Border14 title="IP Address" />,
      //"ui:help": "Enter a valid IPv4/IPv6 Address",
      //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
    },
    port: {
      classNames: "col-xs-2",
      "ui:title": <CustomTitle14Border14 title="Port" />,
      //"ui:widget": "updown"
    },
    s_nssai: {
      classNames: "col-xs-6",
      "ui:title": <CustomTitle14 title="S_NSSAI" />,
      sst: {
        classNames: "col-xs-7",
        "ui:widget": "radio",
        "ui:options": { "inline": true },
      },
      sd: {
        classNames: "col-xs-5",
        //"ui:widget": "updown"
      },
    }
  }
};

export const freeDiameter2Schema = {
  type: "object",
  properties: {
    no_fwd: {
      type: "boolean",
      title: "No Forward"
    },
    identity: {
      type: "string",
      title: "Identity"
    },
    realm: {
      type: "string",
      title: "Realm"
    },
    listen_on: {
      type: "string",
      title: "Listen On",
      anyOf: [
        { format: "ipv4" },
        { format: "ipv6" }
      ],
      messages: {
        "anyOf": "IPv4 or IPv6 allowed"
      },
    },
    load_extension: {
      type: "array",
      title: "Load Extension",
      items: {
        type: "object",
        properties: {
          module: {
            type: "string",
            title: "Module"
          },
          conf: {
            type: "string",
            title: "Conf",
            pattern: "^0x[0-9A-Fa-f]+$",
            messages: {
              "pattern": "Hex starting with '0x'"
            },
            //description: "Please enter a valid hexadecimal number starting with '0x'",
          },
        },
        required: ["module"]
      },
      title: "Load Extension"
    },
    connect: {
      type: "array",
      items: {
        type: "object",
        properties: {
          identity: {
            type: "string",
            title: "Identity"
          },
          addr: {
            type: "string",
            title:"IP Address",
            anyOf: [
              { format: "ipv4" },
              { format: "ipv6" }
            ],
            messages: {
              "anyOf": "IPv4 or IPv6 allowed"
            },
          }
        },
        required: ["identity", "addr"]
      },
      title: "Connect"
    }
  },
  //required: ["identity", "realm", "listen_on", "no_fwd", "load_extension", "connect"],
  title: "Free Diameter"
};

export const freeDiameter2UiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Diameter" />,
  no_fwd: {
    classNames: "col-xs-3"
  },
  identity: {
    classNames: "col-xs-3"
  },
  realm: {
    classNames: "col-xs-3"
  },
  listen_on: {
    classNames: "col-xs-3"
  },
  load_extension: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Load Extension" />,
    items: {
      module: {
        classNames: "col-xs-9"
      },
      conf: {
        classNames: "col-xs-3"
      }
    }
  },
  connect: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Connect" />,
    items: {
      identity: {
        classNames: "col-xs-6"
      },
      addr: {
        classNames: "col-xs-6"
      }
    }
  }
};

export const hnetSchema = {
  type: "array",
  title: "HNET",
  items: {
    type: "object",
    properties: {
      id: {
        type: "integer",
        title: "ID",
        minimum: 1,
        maximum:99,
      },
      scheme: {
        type: "integer",
        title: "Scheme",
        minimum: 1,
      },
      key: {
        type: "string",
        title: "Key"
      }
    },
    required: ["id", "scheme", "key"]
  }
};

export const hnetUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Home Network Public Key" />,
  items: {
    id: {
      classNames: "col-xs-2"
    },
    scheme: {
      classNames: "col-xs-2"
    },
    key: {
      classNames: "col-xs-8"
    }
  }
};

function CustomInput1(props) {
  return (
    <input
      //className="col-xs-12"
      style={{
        //fontWeight: "400",
        //fontSize: "21px"
        marginLeft: "10px",
        
        display: "block",
        width: "-webkit-fill-available",
        height: "34px",
        padding: "6px 12px",
        fontSize: "14px",
        lineHeight: "1.42857143",
        color: "#555",
        backgroundColor: "#fff",
        backgroundImage: "none",
        border: "1px solid #ccc",
        borderRadius: "4px",
        boxShadow: "inset 0 1px 1px rgba(0,0,0,.075)",
        writingMode: "horizontal-tb !important",
        transition: "border-color ease-in-out .15s,box-shadow ease-in-out .15s"
        
      }}
      {...props}
    />
  );
}

function CustomTitle18({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "400",
          fontSize: "18px",
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


function CustomTitle21({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "400",
          fontSize: "21px",
          //marginLeft: "5px",
        }}
      >
        {title}
      </label>
    </div>
  );
}

function CustomTitle18Border39({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "400",
          fontSize: "18px",
          borderBottom: "1px solid #e5e5e5",
          width: "1000px",
          marginBottom: "39px",
          lineHeight: "1.9",
        }}
      >
        {title}
      </label>
    </div>
  );
}

function CustomTitle18Border15({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "400",
          fontSize: "18px",
          borderBottom: "1px solid #e5e5e5",
          width: "1000px",
          marginBottom: "15px",
          lineHeight: "1.9",
        }}
      >
        {title}
      </label>
    </div>
  );
}

function CustomTitle18Margin45({ title }) {
  return (
    <legend 
      style={{
              marginBottom: "45px",
            }}
     >
      <div>
        <label
          style={{
            fontWeight: "400",
            fontSize: "18px",
          }}
        >
        {title}
        </label>
      </div>
    </legend>
  );
}

function CustomTitle14Border14({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "700",
          fontSize: "14px",
          borderBottom: "1px solid #e5e5e5",
          width: "1000px",
          marginBottom: "14px",
          lineHeight: "2.28",
        }}
      >
        {title}
      </label>
    </div>
  );
}

export const db_uriSchema = {
  type: "string",
  title: "DB_URI",
};

export const db_uriUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle21 title="DB_URI" />,
  //"ui:widget": CustomInput1
};