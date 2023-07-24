export const nrfschema = {
  "title": "NRF Configuration",
  "type": "object",
  "properties": {
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

export const nrfuiSchema = {

  "logger" : {
    classNames: "col-xs-7",
  },
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
