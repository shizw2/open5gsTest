export const loggerSchema = {
  type: "object",
  title: "Logger",
  properties: {
    level: {
      type: "string",
      title: "Log Level",
      enum: ["debug", "info", "warning", "error"],
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
    classNames: "col-xs-9",
  },
  level: {
    classNames: "col-xs-3",
  },
};

export const sbiSchema = {
  type: "object",
  title: "SBI",
  properties: {
    server: {
      type: "object",
      title:"Server",
      properties: {
        no_tls: {
          type: "boolean",
          title: "No TLS",
          default: false,
        },
        no_verify: {
          type: "boolean",
          title: "No Verify",
          default: true,
        },
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
      },
    },
    client: {
      type: "object",
      title:"Client",
      properties: {
        no_tls: {
          type: "boolean",
          title: "No TLS",
          default: false,
        },
        no_verify: {
          type: "boolean",
          title: "No Verify",
          default: true,
        },
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
      },
    },
  },
};

export const sbiUiSchema = {
  classNames: "col-xs-12",
    server: {
	classNames: "col-xs-12",
    no_tls: {
      classNames: "col-xs-3",
    },
    no_verify: {
      classNames: "col-xs-3",
    },
    cacert: {
      classNames: "col-xs-6",
    },
    key: {
      classNames: "col-xs-6",
    },
    cert: {
      classNames: "col-xs-6",
    },
  },
  client: {
	classNames: "col-xs-12",  
    no_tls: {
      classNames: "col-xs-3",
    },
    no_verify: {
      classNames: "col-xs-3",
    },
    cacert: {
      classNames: "col-xs-6",
    },
    key: {
      classNames: "col-xs-6",
    },
    cert: {
      classNames: "col-xs-6",
    },
  },
};

export const nrfSchema = {
  type: "object",
  title:"NRF",
  properties: {
    sbi: {
      type: "array",
      title: "SBI",
      items: {
        type: "object",
        properties: {
          addr: {
            type: "array",
            title:"IP Address",
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
    items: {
      addr: {
        classNames: "col-xs-7",
        items:{
          //classNames: "col-xs-12",
          //"ui:help": "Enter a valid IPv4/IPv6 Address",
          //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
        }
      },
      port: {
        classNames: "col-xs-5",
      },
    },
  },
};


export const nf_sbi_Schema = {
  type: "array",
  title:"SBI",
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
  items: {
    addr: {
      classNames: "col-xs-6",
      //"ui:help": "Enter a valid IPv4/IPv6 Address",
      //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
    },
    port: {
      classNames: "col-xs-6"
    }
  }
};

//字段可能存在也可能不存在的处理
// Scheme
export const time_instanceSchema = {
  type: "object",
  properties: {
    nf_instance: {
      type: "object",
      properties: {
        heartbeat: {
          type: "number",
          default: 10
        }
      },
      required: ["heartbeat"]
    }
  }
};

// UiSchema
export const time_instanceUiSchema = {
  classNames: "col-xs-12",
  nf_instance: {
    classNames: "col-xs-12",
    heartbeat: {
      classNames: "col-xs-12"
    }
  }
};

export const time_t3512Schema = {
  type: "object",
  properties: {
    t3512: {
      type: "object",
      properties: {
        value: {
          type: "number"
        }
      }
    }
  }
};

export const time_t3512UiSchema = {
  classNames: "col-xs-12",
  t3512: {
    classNames: "col-xs-12",
    value: {
      classNames: "col-xs-12"
    }
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
  title: "Ngap",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "Address",
        default: "127.0.0.5"
      },
    }
  }
};


export const ngapUiSchema = {
  classNames: "col-xs-12",
  items: {
    metrics: {
      classNames: "col-xs-12",
      items: {
		classNames: "col-xs-12",
        addr: {
          classNames: "col-xs-12",
        },
      },
    },
  },
};

export const metricsSchema = {
  type: "array",
  title: "Metrics",
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
  //items: {
    //classNames: "col-xs-12",//层次感  
    //metrics: {
      //classNames: "col-xs-12",
      items: {
        //classNames: "col-xs-12",
        addr: {
          classNames: "col-xs-6",
        },
        port: {
          classNames: "col-xs-6",
        },
      //},
    //},
  },
};


export const guamiSchema = {
  type: "array",
  title: "Guami",
  items: {
	type: "object",
	properties: {
	  plmn_id: {
		type: "object",
		properties: {
		  mcc: {
			type: "integer"
		  },
		  mnc: {
			type: "integer"
		  }
		},
		required: ["mcc", "mnc"]
	  },
	  amf_id: {
		type: "object",
		properties: {
		  region: {
			type: "integer"
		  },
		  set: {
			type: "integer"
		  }
		},
		required: ["region", "set"]
	  }
	},
	required: ["plmn_id", "amf_id"]
  }

};

export const guamiUiSchema = {
  classNames: "col-xs-12",   //增加这个，guami元素才显示
  items: {
    plmn_id: {
		classNames: "col-xs-12",//增加这个，体现层次感
		mcc: {
		  classNames: "col-xs-6"
		},
		mnc: {
		  classNames: "col-xs-6"
		}
    },
    amf_id: {
		classNames: "col-xs-12",
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
  items: {
	type: "object",
	properties: {
	  plmn_id: {
		type: "object",
		properties: {
		  mcc: {
			type: "integer"
		  },
		  mnc: {
			type: "integer"
		  }
		},
		required: ["mcc", "mnc"]
	  },
	  tac: {
		type: "integer",		
	  }
	},
	required: ["plmn_id", "tac"]
  }

};

export const taiUiSchema = {
  classNames: "col-xs-12",  
  items: {
    plmn_id: {
		classNames: "col-xs-6",//增加这个，体现层次感
		mcc: {
		  classNames: "col-xs-6"
		},
		mnc: {
		  classNames: "col-xs-6"
		}
	},
    tac: {
		classNames: "col-xs-6"
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
      items: {
        type: "string",
        enum: ["NIA2", "NIA1", "NIA0"]
      },
      contains: {
        type: "string",
        format: "unique"
      }
    },
    ciphering_order: {
      type: "array",
      title: "Ciphering Order",
      items: {
        type: "string",
        enum: ["NEA0", "NEA1", "NEA2"]
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
  integrity_order: {
    classNames: "col-xs-12"
  },
  ciphering_order: {
    classNames: "col-xs-12"
  }
};

export const network_nameSchema = {
  type: "object",
  title:"network_name",
  properties: {
    full: {
      type: "string",
    }
  },
};

export const network_nameUiSchema = {
  classNames: "col-xs-6",	
  full: {
	classNames: "col-xs-12",	  
    "ui:placeholder": "Enter the full name",
  }
};

export const amf_nameSchema = {
  type: "string"
};

export const amf_nameUiSchema = {
  classNames: "col-xs-6"
};

export const pfcpSchema = {
  type: "array",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "Address",
        default: "127.0.0.4"
      }
    }
  }
};

export const pfcpUiSchema = {
  classNames: "col-xs-12",
  //pfcp: {  //不需要再加一层。加了反而没层次感  
  items: {
    classNames: "col-xs-12",
    addr: {
  	classNames: "col-xs-12"
    }
  }  
};

export const gtpcSchema = {
  type: "array",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "Address",
        default: "127.0.0.4"
      }
    }
  }
};

export const gtpcUiSchema = {
  classNames: "col-xs-12",
  items: {
  classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12"
    }
  } 
};

export const gtpuSchema = {
  type: "array",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "Address",
        default: "127.0.0.4"
      }
    }
  }
};

export const gtpuUiSchema = {
  classNames: "col-xs-12",
  items: {
    classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12"
    }
  }
};


export const subnetSchema = {
  type: "array",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "Address"
      }
    }
  }
};

export const subnetUiSchema = {
  classNames: "col-xs-12",
  //删除无用代码,反而有层次感
  items: {
    classNames: "col-xs-12",
    addr: {
      classNames: "col-xs-12"
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
  classNames: "col-xs-12",
  dns: {
    classNames: "col-xs-12"
  }
};

export const mtuSchema = {
  type: "number",
  minimum: 0
};

export const mtuUiSchema = {
  classNames: "col-xs-12"
};

export const ctfSchema = {
  type: "object",
  properties: {
    enabled: {
      type: "string",
      enum: ['auto', 'yes', 'no']
    }
  },
  required: ["enabled"]
};

export const ctfUiSchema = {
  classNames: "col-xs-12",
  enabled: {
    classNames:"col-xs-12"
  }
};

export const freeDiameterSchema = {
  type: "string"
};

export const freeDiameterUiSchema = {
  classNames: "col-xs-12"
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
  items: {
    addr: {
      classNames: "col-xs-6",
      //"ui:help": "Enter a valid IPv4/IPv6 Address",
      //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
    },
    port: {
      classNames: "col-xs-6",
      //"ui:widget": "updown"
    },
    s_nssai: {
      classNames: "col-xs-12",
      sst: {
        classNames: "col-xs-6",
        "ui:widget": "radio",
        "ui:options": { "inline": true },
      },
      sd: {
        classNames: "col-xs-6",
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
    }
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
  required: ["identity", "realm", "listen_on", "no_fwd", "load_extension", "connect"],
  title: "Free Diameter"
};

export const freeDiameter2UiSchema = {
  classNames: "col-xs-12",
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

export const db_uriSchema = {
  type: "string",
  title: "DB_URI",
};

export const db_uriUiSchema = {
  classNames: "col-xs-12"
};