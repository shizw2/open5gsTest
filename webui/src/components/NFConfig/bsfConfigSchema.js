export const bsfschema = {
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

export const bsfuiSchema = {
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
