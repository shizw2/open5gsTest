import { object } from "prop-types";

let promptRestart = "修改本配置项需重启系统";

const HiddenField = () => {
  return null;
};

export const loggerOldSchema = {
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

export const loggerOldUiSchema = {
  classNames: "col-xs-12",
  file: {
    classNames: "col-xs-8",
  },
  level: {
    classNames: "col-xs-4",
  },
};

export const loggerSchema = {
  type: "object",
  title: "Logger",
  properties: {
    file: {
      type: "object",
      title: "Log File",
      properties: {
        path:{
          type: "string",
          title: "path",
          default: "",
        }
      }
    },
    level: {
      type: "string",
      title: "Log Level",
      enum: ["fatal", "error", "warn", "info", "debug", "trace"],
      //default: "info",
    },
  },
};

export const loggerUiSchema = {
  classNames: "col-xs-12",
  file: {
    classNames: "col-xs-8",
    "ui:title": <CustomTitle14 title="Log File" />,
    path: {
      classNames: "col-xs-12",
      "ui:options": {
        "label": false
      }
    }
  },
  level: {
    classNames: "col-xs-4",
    "ui:title": <CustomTitle14Border14 title="Log Level" />,
  },
};

export const cliSchema = {
  type: "object",
  title: "CLI Configuration",
  properties: {
    server:{
      type: "array",
      title: "Server",
      "maxItems": 4,
      "messages": {
          "maxItems": "4 Interfaces are supported"
      },
      items: {
        type: "object",
        properties: {
          address: {
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
            minimum: 1,
            maximum: 65535,
            required: true,
            //default: 7777
          }
        }
      }
    }
  }
}

export const cliUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="CLI Configuration" />,
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
    items: {
      address: {
        classNames: "col-xs-8",
        //"ui:help": "Enter a valid IPv4/IPv6 Address",
        //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
      },
      port: {
        classNames: "col-xs-4"
      }
    }
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

export const upfSchema = {
  type: "object",
  title:"UPF",
  properties: {
    pfcp: {
      type: "array",
      title:"PFCP",
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
            required: true,
          },
          dnn: {
            type: "array",
            title: "DNN",
            items: {
              type: "string",
              required: true,
            }
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
        },
      },
    },
  },
};

export const upfUiSchema = {
  classNames: "col-xs-12",
  pfcp: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="PFCP" />,
    items: {
      addr: {
        classNames: "col-xs-12",
        //"ui:title": <CustomTitle14Border14 title="IP Address" />,
      },
      dnn: {
        classNames: "col-xs-6",
        "ui:title": <CustomTitle14 title="DNN" />,
        items:{
          //classNames: "col-xs-12",
        }
      },
      tac: {
        classNames: "col-xs-6",
        "ui:title": <CustomTitle14 title="TAC" />,
        //"ui:title": <CustomTitle18Margin45 title="TAC" />,
        items:{
          "ui:placeholder": "Format like 1 or 3-5",
        }
      }
    },
  },
};

export const smfSchema = {
  type: "object",
  title:"SMF",
  properties: {
    pfcp: {
      type: "object",
      title:"PFCP",
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
        },
      },
    },
  },
};

export const SmfUiComponent = ({ formData }) => {
  if (!formData || !formData.pfcp ) {
    // 如果 formData 或者 formData.pfcp 为空，则返回 null 或者其他适当的内容
    return null;
  }
  
  return (
    <div className="col-xs-12">
      {formData.pfcp ? (
        <div>
          <CustomTitle18 title="PFCP Addr" />
          <div className="col-xs-12">
            <div className="col-xs-12">
              <CustomTitle14Border14 title="IP Address" />
            </div>
            {/* 其他PFCP addr相关内容 */}
          </div>
        </div>
      ) : (
        <div>No PFCP data available</div>
      )}
    </div>
  );
};

export const smfUiSchema = {
  classNames: "col-xs-12",
  pfcp: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="PFCP Addr" />,
    addr: {
        classNames: "col-xs-12",
        "ui:options": {
          "label": false
        }
        //"ui:title": <CustomTitle14Border14 title="IP Address" />,
    },
  },
};

export const nrfNFSchema = {
  type: "object",
  title:"NRF",
  properties: {
    sbi: {
      type: "object",
      title:"SBI",
      properties: {
        server:{
          type: "array",
          title:"Server",
          "minItems": 1,
          "maxItems": 4,
          "messages": {
            "minItems": "At least 1 Interface is required",
            "maxItems": "4 Interfaces are supported"
          },
          items: {
            type: "object",
            properties: {
              address: {
                type: "string",
                title: "Address",
/*
                anyOf: [
                  { format: "ipv4" },
                  { format: "ipv6" },
                ],
                messages: {
                  "anyOf": "IPv4 or IPv6 allowed"
                },
*/
                //format:"ipv4",
                //description: "Please enter a valid IPv4/IPv6 Address",
                required: true,
                //default: "127.0.0.5"
              },
              port: {
                type: "number",
                title: "Port",
                minimum: 1,
                maximum: 65535,
                //required: true,
                //default: 7777
              }
            }
          }
        },
      }
    },
    serving: {
      type: "array",
      title: "Serving For Roaming",
      //minItems: 1,
      maxItems: 8,
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
        },
      },
    },
    metrics: {
      type: "object",
      title:"Metrics",
      properties: {
        server:{
          type: "array",
          title: "Server",
          //"minItems": 1,
          "maxItems": 4,
          "messages": {
            "minItems": "At least 1 Interface is required",
            "maxItems": "4 Interfaces are supported"
          },
          items: {
            type: "object",
            properties: {
              address: {
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
                minimum: 1,
                maximum: 65535,
                default: 9090
              }
            }
          }
        }
      }
    },
/*
    relative_capacity:
    {
      type: "number",
      title: "Relative Capacity",
      minimum: 0,
      maximum: 100,
      required: true,
    }
*/
  },
  //required:["addr","port"],
};

export const nrfNFUiSchema = {
  classNames: "col-xs-12",
  sbi: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="SBI Interface" />,
    "ui:description": <Customhelp14 title={promptRestart} />,
    server: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle18 title="Server" />,
      items: {
        address: {
          classNames: "col-xs-8",
          "ui:placeholder": "IPv4/IPv6 or FQDN",
          //"ui:help": "Enter a valid IPv4/IPv6 Address",
        },
        port: {
          classNames: "col-xs-4"
        }
      }
    },
  },
  serving: {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Serving For Roaming" />,
    items: {
      plmn_id: {
        classNames: "col-xs-12",
        "ui:title": <CustomTitle18 title="PLMN_ID" />,
        mcc: {
          classNames: "col-xs-6"
        },
        mnc: {
          classNames: "col-xs-6"
        }
      },
    }
  },
  metrics: {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Metrics" />,
    "ui:description": <Customhelp14 title={promptRestart} />,
    server:{
      classNames: "col-xs-12",
      "ui:title": <CustomTitle18 title="Server" />,
        items: {
          address: {
            classNames: "col-xs-8",
          },
          port: {
            classNames: "col-xs-4",
          },
      },
    }
  },
/*
  relative_capacity: 
  {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Relative Capacity" />,
  }
*/
};

export const nrfNFOldSchema = {
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
            minimum: 1,
            maximum: 65535,
            required: true,
          },
        },
      },
      //required:["addr","port"],
    },
    metrics: {
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
            minimum: 1,
            maximum: 65535,
            default: 9090
          }
        }
      }
    }
  },
  //required:["addr","port"],
};

export const nrfNFOldUiSchema = {
  classNames: "col-xs-12",
  metrics: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Metrics" />,
    "ui:description": <Customhelp14 title={promptRestart} />,
    items: {
      addr: {
        classNames: "col-xs-8",
      },
      port: {
        classNames: "col-xs-4",
      },
    },
  },
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

export const nrfSchema = {
  type: "object",
  title:"NRF",
  properties: {
    sbi: {
      type: "array",
      title: "SBI Interface",
      "minItems": 1,
      "messages": {
        "minItems": "At least 1 Item is required",
      },
      items: {
        type: "object",
        properties: {
          addr: {
            type: "array",
            title:"IP Address",
            "minItems": 1,
            "messages": {
              "minItems": "At least 1 Item is required",
            },
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
            minimum: 1,
            maximum: 65535,
            required: true,
          },
        },
      },
      //required:["addr","port"],
    },
  },
};

export const nrfUiSchema = {
  classNames: "col-xs-12",
  "ui:description": <Customhelp14 title={promptRestart} />,
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

export const relative_capSchema = {
  type: "number",
  title: "Relative Capacity",
  minimum: 0,
  maximum: 100,
  required: true,
}

export const relative_capUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Relative Capacity" />,
}

export const relative_capShortUiSchema = {
  classNames: "col-xs-4",
  "ui:title": <CustomTitle18Border15 title="Relative Capacity" />,
}

export const paraSchema = {
  type: "object",
  title:"Parameter",
  properties: {
    capacity: {
      type: "number",
      title: "Capacity",
      minimum: 0,
      maximum: 100,
      //required: true, 
    },
  },
}

export const paraUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Parameter" />,
  capacity: {
    classNames: "col-xs-12"
  }
}

export const nf_sbi_OldSchema = {
  type: "array",
  title: "SBI Interface",
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
        minimum: 1,
        maximum: 65535,
        required: true,
        //default: 7777
      }
    }
  }
};

export const nf_sbi_OldUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="SBI Interface" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  //"ui:help": <Customhelp14 title={promptRestart} />,
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

export const nf_sbi_Schema = {
  type: "object",
  title:"SBI",
  properties: {
    server:{
      type: "array",
      title:"Server",
      "minItems": 1,
      "maxItems": 4,
      "messages": {
        "minItems": "At least 1 Interface is required",
        "maxItems": "4 Interfaces are supported"
      },
      items: {
        type: "object",
        properties: {
          address: {
            type: "string",
            title: "Address",
/*
            anyOf: [
              { format: "ipv4" },
              { format: "ipv6" },
            ],
            messages: {
              "anyOf": "IPv4 or IPv6 allowed"
            },
*/
            //format:"ipv4",
            //description: "IPv4/IPv6 Address or FQDN",
            required: true,
            //default: "127.0.0.5"
          },
          port: {
            type: "number",
            title: "Port",
            minimum: 1,
            maximum: 65535,
            //required: true,
            //default: 7777
          }
        }
      }
    },
    client: {
      type: "object",
      title: "Client",
      properties: {
        nrf: {
          type: "array",
          title:"NRF",
          "maxItems": 16,
          items: {
            type: "object",
            properties: {
              uri: {
                type: "string",
                title: "URI",
                //required: true,
              }
            }
          }
        },
/*
        scp: {
          type: "array",
          title:"SCP",
          "maxItems": 4,
          items: {
            type: "object",
            properties: {
              uri: {
                type: "string",
                title: "URI",
                //required: true,
              }
            }
          }
        }
*/
      }
    }
  }
};

export const nf_sbi_UiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="SBI Interface" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
    items: {
      address: {
        classNames: "col-xs-8",
        //"ui:help": "IPv4/IPv6 or FQDN",
        "ui:placeholder": "IPv4/IPv6 or FQDN",
      },
      port: {
        classNames: "col-xs-4",
        //"ui:disabled": false,
        //"ui:readonly": false,
      }
    }
  },
  client: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Client" />,
    nrf: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle14 title="NRF URI" />,
      items: {
        uri: {
          classNames: "col-xs-12",
          "ui:options": {
            "label": false
          }
        }
      }
    },
/*
    scp: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle14 title="SCP URI" />,
      items: {
        uri: {
          classNames: "col-xs-12",
          "ui:options": {
            "label": false
          }
        }
      }
    }
*/
  }
};

export const n32Schema = {
  type: "object",
  title:"N32",
  properties: {
    server:{
      type: "array",
      title:"Server",
      "minItems": 1,
      "maxItems": 4,
      "messages": {
        "minItems": "At least 1 Interface is required",
        "maxItems": "4 Interfaces are supported"
      },
      items: {
        type: "object",
        properties: {
          sender: {
            type: "string",
            title: "Sender",
            //required: true,
          },
          address: {
            type: "string",
            title: "Address",
            anyOf: [
              { format: "ipv4" },
              { format: "ipv6" },
            ],
            messages: {
              "anyOf": "IPv4 or IPv6 allowed"
            },
            //required: true,
            //default: "127.0.0.5"
          },
          port: {
            type: "number",
            title: "Port",
            minimum: 1,
            maximum: 65535,
            //required: true,
            //default: 7777
          },
          n32f: {
            type: "object",
            title:"N32f",
            properties: {
              address: {
                type: "string",
                title: "Address",
                anyOf: [
                  { format: "ipv4" },
                  { format: "ipv6" },
                ],
                messages: {
                  "anyOf": "IPv4 or IPv6 allowed"
                },
                //required: true,
                //default: "127.0.0.5"
              },
              port: {
                type: "number",
                title: "Port",
                minimum: 1,
                maximum: 65535,
                //required: true,
                //default: 7777
              }
            }
          }
        }
      }
    },
    client: {
      type: "object",
      title: "Client",
      properties: {
        sepp: {
          type: "array",
          title:"sepp",
          "maxItems": 8,
          items: {
            type: "object",
            properties: {
              receiver: {
                type: "string",
                title: "Receiver",
                //required: true,
              },
              uri: {
                type: "string",
                title: "URI",
                //required: true,
              },
              resolve: {
                type: "string",
                title: "Resolve",
                anyOf: [
                  { format: "ipv4" },
                  { format: "ipv6" },
                ],
                messages: {
                  "anyOf": "IPv4 or IPv6 allowed"
                },
              },
              n32f: {
                type: "object",
                title:"N32f",
                properties: {
                  uri: {
                    type: "string",
                    title: "URI",
                    //required: true,
                  },
                  resolve: {
                    type: "string",
                    title: "Resolve",
                    anyOf: [
                      { format: "ipv4" },
                      { format: "ipv6" },
                    ],
                    messages: {
                      "anyOf": "IPv4 or IPv6 allowed"
                    },
                  },
                }
              }
            }
          }
        },
      }
    }
  }
};

export const n32UiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="N32 Configuration" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
    items: {
      sender: {
        classNames: "col-xs-12",
      },
      address: {
        classNames: "col-xs-8",
        //"ui:help": "IPv4/IPv6 or FQDN",
        "ui:placeholder": "IPv4/IPv6",
      },
      port: {
        classNames: "col-xs-4",
        //"ui:disabled": false,
        //"ui:readonly": false,
      },
      n32f:
      {
        classNames: "col-xs-12",
        "ui:title": <CustomTitle14 title="n32f" />,
        address: {
          classNames: "col-xs-8",
          //"ui:help": "IPv4/IPv6 or FQDN",
          "ui:placeholder": "IPv4/IPv6",
        },
        port: {
          classNames: "col-xs-4",
          //"ui:disabled": false,
          //"ui:readonly": false,
        },
      }
    }
  },
  client: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Client" />,
    sepp: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle14 title="SEPP" />,
      items: {
        receiver: {
          classNames: "col-xs-12",
        },
        uri: {
          classNames: "col-xs-7",
        },
        resolve: {
          classNames: "col-xs-5",
        },
        n32f:
        {
          classNames: "col-xs-12",
          "ui:title": <CustomTitle14 title="n32f" />,
          uri: {
            classNames: "col-xs-7",
          },
          resolve: {
            classNames: "col-xs-5",
          },
        }
      }
    },
  }
};

export const nssf_sbi_Schema = {
  type: "object",
  title:"SBI",
  properties: {
    server:{
      type: "array",
      title:"Server",
      "minItems": 1,
      "maxItems": 4,
      "messages": {
        "minItems": "At least 1 Interface is required",
        "maxItems": "4 Interfaces are supported"
      },
      items: {
        type: "object",
        properties: {
          address: {
            type: "string",
            title: "Address",
/*
            anyOf: [
              { format: "ipv4" },
              { format: "ipv6" },
            ],
            messages: {
              "anyOf": "IPv4 or IPv6 allowed"
            },
*/
            //format:"ipv4",
            //description: "Please enter a valid IPv4/IPv6 Address",
            required: true,
            //default: "127.0.0.5"
          },
          port: {
            type: "number",
            title: "Port",
            minimum: 1,
            maximum: 65535,
            //required: true,
            //default: 7777
          }
        }
      }
    },
    client: {
      type: "object",
      title: "Client",
      properties: {
        nrf: {
          type: "array",
          title:"NRF",
          "maxItems": 16,
          items: {
            type: "object",
            properties: {
              uri: {
                type: "string",
                title: "URI",
                //required: true,
              }
            }
          }
        },
/*
        scp: {
          type: "array",
          title:"SCP",
          "maxItems": 4,
          items: {
            type: "object",
            properties: {
              uri: {
                type: "string",
                title: "URI",
                //required: true,
              }
            }
          }
        },
*/
        nsi: {
          type: "array",
          title:"NSI",
          "minItems": 1,
          "maxItems": 8,
          "messages": {
            "minItems": "At least 1 item is required",          
          },
          items: {
            type: "object",
            properties: {
              uri: {
                type: "string",
                title: "URI",
                required: true,
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
        }
      }
    }
  }
};

export const nssf_sbi_UiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="SBI Interface" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
    items: {
      address: {
        classNames: "col-xs-8",
        //"ui:help": "Enter a valid IPv4/IPv6 Address",
        "ui:placeholder": "IPv4/IPv6 or FQDN",
      },
      port: {
        classNames: "col-xs-4"
      }
    }
  },
  client: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Client" />,
    nrf: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle15 title="NRF URI" />,
      items: {
        uri: {
          classNames: "col-xs-12",
          "ui:options": {
            "label": false
          }
        }
      }
    },
/*
    scp: {
      classNames: "col-xs-6",
      "ui:title": <CustomTitle15 title="SCP URI" />,
      items: {
        uri: {
          classNames: "col-xs-12",
          "ui:options": {
            "label": false
          }
        }
      }
    },
*/
    nsi:{
      classNames: "col-xs-12",
      "ui:title": <CustomTitle15 title="NSI" />,
      items: {
        uri: {
          classNames: "col-xs-6",
          "ui:title": <CustomTitle14 title="URI" />,
        },
        s_nssai: {
          classNames: "col-xs-6",
          "ui:title": <CustomTitle14 title="S_NSSAI" />,
          sst: {
            classNames: "col-xs-8",
            "ui:widget": "radio",
            "ui:options": { "inline": true },
          },
          sd: {
            classNames: "col-xs-4",
            //"ui:widget": "updown"
          },
        }
      }
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
      minimum: 1,
      required: true,
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
      minimum: 1,
      required: true,
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
      minimum: 1,
      required: true,
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
      minimum: 1,
      maximum: 65535,
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
  type: "object",
  title:"NGAP",
  properties: {
    server:{
      type: "array",
      title: "Server",
      "minItems": 1,
      "maxItems": 8,
      "messages": {
        "minItems": "At least 1 IP is required",
        "maxItems": "8 IPs are supported"
      },
      items: {
        type: "object",
        properties: {
          address: {
            type: "string",
            title: "IP Address",
            anyOf: [
              { format: "ipv4" },
              { format: "ipv6" }
            ],
            messages: {
              "anyOf": "IPv4 or IPv6 allowed"
            },
            //default: "127.0.0.5",
            required: true,
          },
        }
      }
    }
  }
};

export const ngapUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="NGAP" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
    items: {
      //classNames: "col-xs-12",
      address: {
        classNames: "col-xs-12",
        "ui:options": {
          "label": false
        }
      },
    }
  },
};

export const metricsOldSchema = {
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
        minimum: 1,
        maximum: 65535,
        default: 9090
      }
    }
  }
};

export const metricsOldUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Metrics" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
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

export const metricsSchema = {
  type: "object",
  title:"Metrics",
  properties: {
    server:{
      type: "array",
      title: "Server",
      //"minItems": 1,
      "maxItems": 4,
      "messages": {
        "minItems": "At least 1 Interface is required",
        "maxItems": "4 Interfaces are supported"
      },
      items: {
        type: "object",
        properties: {
          address: {
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
            minimum: 1,
            maximum: 65535,
            default: 9090
          }
        }
      }
    }
  }
};

export const metricsUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Metrics" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  //items: {
    //classNames: "col-xs-12",//层次感  
    //metrics: {
      //classNames: "col-xs-12",
  server:{
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
      items: {
        //classNames: "col-xs-12",
        address: {
          classNames: "col-xs-8",
        },
        port: {
          classNames: "col-xs-4",
        },
      //},
    //},
    },
  }
};

export const metricsShortUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="Metrics" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  //items: {
    //classNames: "col-xs-12",//层次感  
    //metrics: {
      //classNames: "col-xs-12",
  server:{
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
      items: {
        //classNames: "col-xs-12",
        address: {
          classNames: "col-xs-8",
        },
        port: {
          classNames: "col-xs-4",
        },
      //},
    //},
    },
  }
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
  "maxItems": 8,
  "messages": {
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
        //minItems: 1,
        items: {
          type: "string",
          pattern: /^(?:\d+|\d+-\d+)$/,
  	      messages: {
            pattern: "Format like 1 or 3-5",
            type: "Format like 1 or 3-5",
          },

          required: true,
        },
        messages: {
          minItems: "At least 1 TAC",
        },
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

export const access_controlSchema = {
  type: "array",
  title: "Access Control",
  maxItems: 16,
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
          },
          reject_cause: {
            type: "integer",
            title: "Reject Cause",
            minimum: 0,
            //required: true,
          },
        },
        //required: ["mcc", "mnc"]
      },
    },
  },
};

export const access_controlUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Access Control" />,
  items: {
    plmn_id: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle18 title="PLMN_ID" />,
      reject_cause: {
        classNames: "col-xs-4"
      },
      mcc: {
        classNames: "col-xs-4"
      },
      mnc: {
        classNames: "col-xs-4"
      }
    },
  }
};

export const plmn_supportSchema = {
  type: "array",
  title: "PLMN_SUPPORT",
  //minItems: 1,
  maxItems: 8,
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
      "minItems": 1,
      "messages": {
        "minItems": "At least 1 Item is required",
      },
      maxItems: 4,
      items: {
        type: "string",
        enum: ["NIA0", "NIA1", "NIA2", "NIA3"],
        //default: "NIA0",
        required: true
      },
      contains: {
        type: "string",
        format: "unique"
      }
    },
    ciphering_order: {
      type: "array",
      title: "Ciphering Order",
      "minItems": 1,
      "messages": {
        "minItems": "At least 1 Item is required",
      },
      maxItems: 4,
      items: {
        type: "string",
        enum: ["NEA0", "NEA1", "NEA2", "NEA3"],
        //default: "NEA0",
        required: true
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
  required: true,
};

export const amf_nameUiSchema = {
  classNames: "col-xs-4",
  "ui:title": <CustomTitle18Border39 title="AMF Name" />,
};

export const smfpfcpSchema = {
  type: "object",
  title: "PFCP",
  properties: {
    server: {
      type: "array",
      title: "Server",
      "minItems": 1,
      "maxItems": 4,
      "messages": {
        "minItems": "At least 1 Interface is required",
        "maxItems": "4 Interfaces are supported"
      },
      items: {
        type: "object",
        properties: {
          address: {
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
        }
      }
    },
    client: {
      type: "object",
      title: "Client",
      properties: {
        upf: {
          type: "array",
          title: "UPF",
          "minItems": 1,
          "maxItems": 4,
          "messages": {
            "minItems": "At least 1 Interface is required",
            "maxItems": "4 Interfaces are supported"
          },
          items: {
            type: "object",
            properties: {
              address: {
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
              dnn: {
                type: "array",
                title: "DNN",
                items: {
                  type: "string",
                  required: true,
                }
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
            }
          }
        }
      }
    }
  }
};

export const smfpfcpUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="PFCP" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  //pfcp: {  //不需要再加一层。加了反而没层次感
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Server" />,
    items: {
      address: {
        classNames: "col-xs-12",
        //"ui:help": "Enter a valid IPv4/IPv6 Address",
        //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
      },
    }
  },
  client: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Client" />,
    upf: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle15 title="UPF" />,
      items: {
        address: {
          classNames: "col-xs-12",
        },
        dnn: {
          classNames: "col-xs-6",
          "ui:title": <CustomTitle14 title="DNN" />,
          items:{
            classNames: "col-xs-12",
          }
        },
        tac: {
          classNames: "col-xs-6",
          "ui:title": <CustomTitle14 title="TAC" />,
          //"ui:title": <CustomTitle18Margin45 title="TAC" />,
          items:{
            classNames: "col-xs-12",
            "ui:placeholder": "Format like 1 or 3-5",
          }
        }
      }
    }
  }
};

export const upfpfcpSchema = {
  type: "object",
  title: "PFCP",
  properties: {
    server: {
      type: "array",
      title: "Server",
      "minItems": 1,
      "maxItems": 4,
      "messages": {
        "minItems": "At least 1 Interface is required",
        "maxItems": "4 Interfaces are supported"
      },
      items: {
        type: "object",
        properties: {
          address: {
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
        }
      }
    },
    client: {
      type: "object",
      title: "Client",
      properties: {
        smf: {
          type: "array",
          title: "SMF",
          "maxItems": 4,
          "messages": {
            "maxItems": "4 Interfaces are supported"
          },
          items: {
            type: "object",
            properties: {
              address: {
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
            }
          }
        }
      }
    }
  }
};

export const upfpfcpUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="PFCP" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  //pfcp: {  //不需要再加一层。加了反而没层次感
  server: {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Server" />,
    items: {
      address: {
        classNames: "col-xs-12",
        //"ui:help": "Enter a valid IPv4/IPv6 Address",
        //"ui:placeholder": "Enter a valid IPv4/IPv6 Address",
      },
    }
  },
  client: {
    classNames: "col-xs-6",
    "ui:title": <CustomTitle18 title="Client" />,
    smf: {
      classNames: "col-xs-12",
      "ui:title": <CustomTitle14 title="SMF Address" />,
      items: {
        address: {
          classNames: "col-xs-12",
          "ui:options": {
            "label": false
          }
        },
      }
    }
  }
};

export const pfcpOldSchema = {
  type: "array",
  title: "PFCP Address",
  minItems: 1,
  maxItems: 8,
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
        required: true,
        //default: "127.0.0.5",
      },
    }
  }
};

export const pfcpOldUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="PFCP Address" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
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
  type: "object",
  title: "GTPC",
  properties: {
    server:{
      type: "array",
      minItems: 1,
      maxItems: 8,
      items: {
        type: "object",
        properties: {
          address: {
            type: "string",
            title: "IP Address",
            anyOf: [
              { format: "ipv4" },
              { format: "ipv6" }
            ],
            messages: {
              "anyOf": "IPv4 or IPv6 allowed"
            },
            //default: "127.0.0.5"
            required: true,
          },
        }
      }
    }
  }
};

export const gtpcUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="GTPC" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle14 title="Server Address" />,
    "ui:options": {
      "label": false
    },
    items: {
      classNames: "col-xs-12",
      address: {
        classNames: "col-xs-12",
        "ui:options": {
          "label": false
        }
      }
    }
  }
};

export const gtpuSchema = {
  type: "object",
  title: "GTPU",
  properties: {
    server:{
      type: "array",
      minItems: 1,
      maxItems: 8,
      items: {
        type: "object",
        properties: {
          address: {
            type: "string",
            title: "IP Address",
            anyOf: [
              { format: "ipv4" },
              { format: "ipv6" }
            ],
            messages: {
              "anyOf": "IPv4 or IPv6 allowed"
            },
            //default: "127.0.0.5"
            required: true,
          },
        }
      }
    }
  }
};

export const gtpuUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="GTPU" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  server: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle14 title="Server Address" />,
    "ui:options": {
      "label": false
    },
    items: {
      classNames: "col-xs-12",
      address: {
        classNames: "col-xs-12",
        "ui:options": {
          "label": false
        }
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

export const sessionSchema = {
  type: "array",
  title: "UE IP Pool",
  //"minItems": 1,
  "maxItems": 32,
  "messages": {
    "minItems": "At least 1 subnet is required",
    "maxItems": "16 subnets are supported"
  },
  items: {
    type: "object",
    properties: {
      subnet: {
        type: "string",
        title: "Subnet",
        anyOf: [
          { pattern: subnetipv4Regex.source },
          { pattern: subnetipv6Regex.source },
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 subnet allowed"
        },
        default: "10.45.0.1/16",
        required: true,
      },
      gateway: {
        type: "string",
        title: "Gatewy",
        anyOf: [
          { format: "ipv4" },
          { format: "ipv6" },
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 allowed"
        },
      },
      dnn: {
        type: "string",
        title: "DNN",
      }
    }
  }
};

export const sessionUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Session Configuration" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  items: {
    subnet: {
      classNames: "col-xs-5",
      "ui:placeholder": "ipv4/v6 subnet address",
    },
    gateway: {
      classNames: "col-xs-4",
    },
    dnn: {
      classNames: "col-xs-3",
    }
  }
};

export const subnetSchema = {
  type: "array",
  title: "UE IP Pool",
  "minItems": 1,
  "maxItems": 16,
  "messages": {
    "minItems": "At least 1 subnet is required",
    "maxItems": "16 subnets are supported"
  },
  items: {
    type: "object",
    properties: {
      addr: {
        type: "string",
        title: "Subnet",
        anyOf: [
          { pattern: subnetipv4Regex.source },
          { pattern: subnetipv6Regex.source },
        ],
        messages: {
          "anyOf": "IPv4 or IPv6 subnet allowed"
        },
        default: "10.45.0.1/16",
        required: true,
      },
      dnn: {
        type: "string",
        title: "DNN",
      }
    }
  }
};

export const subnetUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="UE IP Pool" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  items: {
    addr: {
      classNames: "col-xs-6",
      "ui:placeholder": "ipv4/v6 subnet address",
    },
    dnn: {
      classNames: "col-xs-6",
    }
  }
};

export const dpdkSchema = {
  type: "object",
  title: "DPDK Configuration",
  properties: {
    pfcp_lcore: {
      type: "number",
      title: "PFCP LCore",
      minimum: 0,
      maximum: 255,
      //default: 0,
    },
    dpt_lcore: {
      type: "number",
      title: "DPT LCore",
      minimum: 0,
      maximum: 255,
      //default: 1,
    },
    fwd_lcore: {
      type: "number",
      title: "FWD LCore",
      minimum: 0,
      maximum: 255,
      //default: 2,
    },
    emptyLine1: {
      type: "number",
    },
    n3_default_gw: {
      type: "string",
      title: "N3 IPv4 Default Gateway",
      format: "ipv4",
    },
    n3_mask_bits: {
      type: "number",
      title: "N3 IPv4 Mask Bits",
      minimum: 1,
      maximum: 32,
      //default: 24,
    },
    emptyLine2: {
      type: "number",
    },
    n3_default_gw6: {
      type: "string",
      title: "N3 IPv6 Default Gateway",
      format: "ipv6",
    },
    n3_mask6_bits: {
      type: "number",
      title: "N3 IPv6 Mask Bits",
      minimum: 1,
      maximum: 128,
      //default: 64,
    },
    emptyLine3: {
      type: "number",
    },
    n6_addr: {
      type: "string",
      title: "N6 IPv4 Address",
      pattern: subnetipv4Regex.source,
      messages: {
        "pattern": "Format like 192.168.6.10/24"
      },
      //default: "10.45.0.1/16",
      //required: true,
    },
    n6_default_gw: {
      type: "string",
      title: "N6 IPv4 Default Gateway",
      format: "ipv4",
    },
    emptyLine4: {
      type: "number",
    },
    n6_addr6: {
      type: "string",
      title: "N6 IPv6 Address",
      pattern: subnetipv6Regex.source,
      messages: {
        "patternf": "Format like 2001::10/48"
      },
      //default: "10.45.0.1/16",
      //required: true,
    },
    n6_default_gw6: {
      type: "string",
      title: "N6 IPv6 Default Gateway",
      format: "ipv6",
    },
  },
};

export const dpdkUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="DPDK Configuration" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
  pfcp_lcore: {
    classNames: "col-xs-4",
  },
  dpt_lcore: {
    classNames: "col-xs-4",
  },
  fwd_lcore: {
    classNames: "col-xs-4",
  },
  emptyLine1: {
    classNames: "col-xs-12",
    "ui:field": HiddenField,
  },
  n3_default_gw: {
    classNames: "col-xs-6",
  },
  n3_mask_bits: {
    classNames: "col-xs-6",
  },
  emptyLine2: {
    classNames: "col-xs-12",
    "ui:field": HiddenField,
  },
  n3_default_gw6: {
    classNames: "col-xs-6",
  },
  n3_mask6_bits: {
    classNames: "col-xs-6",
  },
  emptyLine3: {
    classNames: "col-xs-12",
    "ui:field": HiddenField,
  },
  n6_addr: {
    classNames: "col-xs-6",
  },
  n6_default_gw: {
    classNames: "col-xs-6",
  },
  emptyLine4: {
    classNames: "col-xs-12",
    "ui:field": HiddenField,
  },
  n6_addr6: {
    classNames: "col-xs-6",
  },
  n6_default_gw6: {
    classNames: "col-xs-6",
  },
};

export const dnsSchema = {
  type: "array",
  title:"DNS Address",
  //minItems: 1,
  maxItems: 16,
  items: {
    type: "string",
    anyOf: [
      { format: "ipv4" },
      { format: "ipv6" }
    ],
    messages: {
      "anyOf": "IPv4 or IPv6 allowed"
    },
    required: true,
  }
};

export const dnsUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="DNS Address" />,
  dns: {
    classNames: "col-xs-12"
  }
};

export const pcscfSchema = {
  type: "array",
  title: "P-CSCF Address",
  maxItems: 8,
  items: {
    type: "string",
    anyOf: [
      { format: "ipv4" },
      { format: "ipv6" }
    ],
    messages: {
      "anyOf": "IPv4 or IPv6 allowed"
    },
    required: true,
    //default: "127.0.0.1"
  },
};

export const pcscfUiSchema = {
  classNames: "col-xs-6",
  "ui:title": <CustomTitle18 title="P-CSCF Address" />,
  items: {
      classNames: "col-xs-12",
  }
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
  minimum: 1
};

export const mtuUiSchema = {
  classNames: "col-xs-6",
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
  classNames: "col-xs-6",
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
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Diameter Config" />,
  "ui:description": <Customhelp14 title={promptRestart} />,
};

export const smfinfoSchema = {
  type: "array",
  title: "SMF Info",
  items: {
    type: "object",
    properties: {
      s_nssai: {
        title: "S-NSSAI",
        type: "array",
        "minItems": 1,
        "maxItems": 8,
        "messages": {
          "minItems": "At least 1 item is required",          
        },
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
            },
            dnn: {
              type: "array",
              title: "DNN",
              minItems: 1,
              items: {
                type: "string",
                required: true,
              }
            }
          },
          //required: ["sst", "dnn"]
        }
      },
      tai: {
        type: "array",
        title: "TAI",
        maxItems: 8,
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
              minItems: 1,
              items: {
                type: "string",
                pattern: /^(?:\d+|\d+-\d+)$/,
                messages: {
                  pattern: "Format like 1 or 3-5",
                  type: "Format like 1 or 3-5",
                },
                required: true,
              },
              messages: {
                minItems: "At least 1 TAC",
              },
            },
          },
        }
      }
    }
  }
};

export const smfinfoUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="SMF Info" />,
  items: {
    classNames: "col-xs-12",
    s_nssai: {
      classNames: "col-xs-12",
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
        dnn: {
          classNames: "col-xs-12",
          "ui:title": <CustomTitle14 title="DNN" />,
        }
      }
    },
    tai: {
      classNames: "col-xs-12", 
      "ui:title": <CustomTitle18 title="TAI" />,
      items: {
        plmn_id: {
          classNames: "col-xs-12", //增加这个，体现层次感
          "ui:title": <CustomTitle14 title="PLMN_ID" />,
          mcc: {
            classNames: "col-xs-6"
          },
          mnc: {
            classNames: "col-xs-6"
          }
        },
        tac: {
          classNames: "col-xs-12",
          "ui:title": <CustomTitle14 title="TAC" />,
          //"ui:title": <CustomTitle18Margin45 title="TAC" />,
          items:{
            "ui:placeholder": "Format like 1 or 3-5",
          }
        }
      }
    }
  }
};

export const nsiSchema = {
  type: "array",
  title:"NSI",
  "minItems": 1,
  "messages": {
    "minItems": "At least 1 item is required",          
  },
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
        minimum: 1,
        maximum: 65535,
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
  "ui:description": <Customhelp14 title={promptRestart} />,
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
        classNames: "col-xs-7"
      },
      addr: {
        classNames: "col-xs-5"
      }
    }
  }
};

export const hnetSchema = {
  type: "array",
  title: "HNET",
  maxItems: 256,
  items: {
    type: "object",
    properties: {
      id: {
        type: "number",
        title: "key ID",
        minimum: 0,
        maximum: 255,
        required: true,
        messages: {
          required: "required",
          minimum: "最小值0",
          maximum: "最大值255",
        }
      },
      scheme: {
        type: "integer",
        title: "Scheme ID",
        minimum: 0,
        maximum: 15,
        required: true,
        messages: {
          required: "required",
          minimum: "最小值0",
          maximum: "最大值15",
        },
      },
      key: {
        type: "string",
        title: "Key",
        required: true,
        messages: {
          required: "required",
        },
      }
    },
    //required: ["id", "scheme", "key"]
  },
};

export const hnetUiSchema = {
  classNames: "col-xs-12",
  "ui:title": <CustomTitle18 title="Home Network Public Key" />,
  items: {
    id: {
      classNames: "col-xs-2"
    },
    scheme: {
      classNames: "col-xs-2",
      //"ui:placeholder": "1:Profile A; 2: Profile B",
    },
    key: {
      classNames: "col-xs-8"
    }
  }
};

export const supiRangeInfoSchema = {
  type: "object",
  title: "Info",
  properties: {
    supi:{
      type: "array",
      title: "SUPI",
      maxItems:16,
      items: {
        type: "object",
        properties: {
          range: {
            type: "string",
            title: "Range",
            maxLength: 31,
            pattern: /^(\d+-\d+)$/,
            messages: {
              pattern: "Format: x1...xn-y1...yn; xn, yn is digit; n<15; like 46001-46009",
              type: "Format: x1...xn-y1...yn; xn, yn is digit; n<15; like 46001-46009",
            },
            required: true,
          },
        },
      }
    }
  }
};

export const supiRangeInfoUiSchema = {
  classNames: "col-xs-12",
  supi: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="SUPI Range" />,
    items: {
      range: {
        classNames: "col-xs-12",
        "ui:options": {
          "label": false
        }
      },
    }
  }
};

export const rtsupiInfoSchema = {
  type: "object",
  title: "NF Info",
  properties: {
    routing_indicator: {
      type: "array",
      title: "Routing Indicator",
      maxItems: 16,
      items: {
        type: "string",
        maxLength: 4,
        pattern: /^(\d+)$/,
        messages: {
          pattern: "Number please",
        },
        required: true,
      },
    },
    supi:{
      type: "array",
      title: "SUPI",
      maxItems:16,
      items: {
        type: "object",
        properties: {
          range: {
            type: "string",
            title: "Range",
            maxLength: 31,
            pattern: /^(\d+-\d+)$/,
            messages: {
              pattern: "Format: x1...xn-y1...yn; xn, yn is digit; n<15; like 46001-46009",
              type: "Format: x1...xn-y1...yn; xn, yn is digit; n<15; like 46001-46009",
            },
            required: true,
          },
        },
      }
    }
  }
};

export const rtsupiInfoUiSchema = {
  classNames: "col-xs-12",
  routing_indicator: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="Routing Indicator" />,
  },
  supi: {
    classNames: "col-xs-12",
    "ui:title": <CustomTitle18 title="SUPI Range" />,
    items: {
      range: {
        classNames: "col-xs-12",
        "ui:options": {
          "label": false
        }
      },
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

function Customhelp14({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "400",
          fontSize: "14px",
          color: "grey"
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

function CustomTitle15({ title }) {
  return (
    <div>
      <label
        style={{
          fontWeight: "700",
          fontSize: "15px",
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