export const loggerSchema = {
  type: "object",
  title: "logger",
  properties: {
    file: {
      type: "string",
      title: "Log File",
      default: "",
    },
    level: {
      type: "string",
      title: "Log Level",
      enum: ["debug", "info", "warning", "error"],
      default: "info",
    },
  },
};

export const loggerUiSchema = {
  classNames: "col-xs-12",
  file: {
    classNames: "col-xs-6",
  },
  level: {
    classNames: "col-xs-6",
  },
};

export const sbiSchema = {
  type: "object",
  properties: {
    server: {
      type: "object",
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
      classNames: "col-xs-6",
    },
    no_verify: {
      classNames: "col-xs-6",
    },
    cacert: {
      classNames: "col-xs-12",
    },
    key: {
      classNames: "col-xs-12",
    },
    cert: {
      classNames: "col-xs-12",
    },
  },
  client: {
	classNames: "col-xs-12",  
    no_tls: {
      classNames: "col-xs-6",
    },
    no_verify: {
      classNames: "col-xs-6",
    },
    cacert: {
      classNames: "col-xs-12",
    },
    key: {
      classNames: "col-xs-12",
    },
    cert: {
      classNames: "col-xs-12",
    },
  },
};

export const nrfSchema = {
  type: "object",
  properties: {
    sbi: {
      type: "array",
      title: "sbi",
      items: {
        type: "object",
        properties: {
          addr: {
                type: "array",
                items: {
                  type: "string"
                }
              },
          port: {
            type: "number",
            title: "Port",
            default: 0,
          },
        },
      },
    },
  },
};

export const nrfUiSchema = {
  classNames: "col-xs-12",
  sbi: {
    classNames: "col-xs-12",
    items: {
      addr: {
        classNames: "col-xs-6",
      },
      port: {
        classNames: "col-xs-6",
      },
    },
  },
};


export const nf_sbi_Schema = {
  type: "array",
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "Address",
        default: "127.0.0.5"
      },
      port: {
        type: "number",
        title: "Port",
        default: 7777
      }
    }
  }
};

export const nf_sbi_UiSchema = {
  sbi: {
    items: {
	  classNames: "col-xs-12",
	  addr: {
        classNames: "col-xs-6"
      },
      port: {
        classNames: "col-xs-6"
      }
    }
  }
};

//字段可能存在也可能不存在的处理
export const timeSchema = {/*
  anyOf: [
    {
  type: "object",
  properties: {
    nf_instance: {
      type: "object",
      properties: {
        heartbeat: {
          type: "integer",
          default: 10
        }
      },
      required: ["heartbeat"]
    }
      },
    },
    {
      type: "null"
    },
  ]*/
};

//TODO
export const timeUiSchema = {
  
  
};


// Schema
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
        title: "Address",
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
  items: {
    metrics: {
      classNames: "col-xs-12",
      items: {
		classNames: "col-xs-12",
        addr: {
          classNames: "col-xs-12",
        },
        port: {
          classNames: "col-xs-12",
        },
      },
    },
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
export const networkNameSchema = {
  type: "object",
  properties: {
    network_name: {
      type: "object",
      properties: {
        full: {
          type: "string"
        }
      },
      required: ["full"]
    }
  }
};

export const networkNameUiSchema = {
  network_name: {
    full: {
      classNames: "col-xs-12"
    }
  }
};

export const amfNameSchema = {
  type: "object",
  properties: {
    amf_name: {
      type: "string"
    }
  },
  required: ["amf_name"]
};

export const amfNameUiSchema = {
  amf_name: {
    classNames: "col-xs-12"
  }
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
  pfcp: {
    classNames: "col-xs-12",
    items: {
	  classNames: "col-xs-12",
      addr: {
        classNames: "col-xs-12"
      }
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
  gtpc: {
    classNames: "col-xs-12",
    items: {
	  classNames: "col-xs-12",
      addr: {
        classNames: "col-xs-12"
      }
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
  gtpu: {
    classNames: "col-xs-12",
    items: {
      classNames: "col-xs-12",
      addr: {
        classNames: "col-xs-12"
      }
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
  subnet: {
    classNames: "col-xs-12",
    items: {
      classNames: "col-xs-12",
      addr: {
        classNames: "col-xs-12"
      }
    }
  }
};

export const dnsSchema = {
  type: "array",
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

export const crfSchema = {
  type: "object",
  properties: {
    enabled: {
      type: "string",
      enum: ['auto', 'yes', 'no']
    }
  },
  required: ["enabled"]
};

export const crfUiSchema = {
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
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string"
      },
      port: {
        type: "integer"
      },
      s_nssai: {
        type: "object",
        properties: {
          sst: {
            type: "integer"
          }
        },
        required: ["sst"]
      }
    },
    required: ["addr", "port", "s_nssai"]
  }
};

export const nsiUiSchema = {
  classNames: "col-xs-12",
  items: {
    addr: {
      classNames: "col-xs-4",
      "ui:autofocus": true,
      "ui:placeholder": "Enter the address"
    },
    port: {
      classNames: "col-xs-4",
      "ui:widget": "updown"
    },
    s_nssai: {
      classNames: "col-xs-4",
      sst: {
        "ui:widget": "updown"
      }
    }
  }
};

export const freeDiameter2Schema = {
  type: "object",
  properties: {
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
      title: "Listen On"
    },
    no_fwd: {
      type: "boolean",
      title: "No Forward"
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
            title: "conf",
            pattern: "^0x[0-9A-Fa-f]+$",
            description: "Please enter a valid hexadecimal number starting with '0x'"
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
            title: "Address"
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
  identity: {
    classNames: "col-xs-12"
  },
  realm: {
    classNames: "col-xs-12"
  },
  listen_on: {
    classNames: "col-xs-12"
  },
  no_fwd: {
    classNames: "col-xs-12"
  },
  load_extension: {
    classNames: "col-xs-12",
    items: {
      module: {
        classNames: "col-xs-12"
      },
      conf: {
        classNames: "col-xs-12"
      }
    }
  },
  connect: {
    classNames: "col-xs-12",
    items: {
      identity: {
        classNames: "col-xs-12"
      },
      addr: {
        classNames: "col-xs-12"
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
        title: "ID"
      },
      scheme: {
        type: "integer",
        title: "Scheme"
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
    hnet: {
      classNames: "col-xs-12",
      items: {
        classNames: "col-xs-12",
        id: {
          classNames: "col-xs-12"
        },
        scheme: {
          classNames: "col-xs-12"
        },
        key: {
          classNames: "col-xs-12"
        }
      }
    }
  }
};
