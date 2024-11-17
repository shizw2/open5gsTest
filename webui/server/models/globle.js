 exports.nfStatus ={
    "AMF" : {
       "up" : 1,
       "reason" : "AMF服务离线,或监控IP或端口错误",
       "time": "",
    },
    "SMF" : {
      "up" : 1,
      "reason" : "SMF服务离线,或监控IP或端口错误",
      "time": "",
    },
    "UPF" : {
      "up" : 1,
      "reason" : "UPF服务离线,或监控IP或端口错误",
      "time": "",
    },
    "UDM" : {
      "up" : 1,
      "reason" : "UDM服务离线,或监控IP或端口错误",
      "time": "",
    },
    "PCF" : {
      "up" : 1,
      "reason" : "PCF服务离线,或监控IP或端口错误",
      "time": "",
    },
    "NRF" : {
      "up" : 1,
      "reason" : "NRF服务离线,或监控IP或端口错误",
      "time": "",
    },
    "node" : {
        "up" : 1,
        "reason" : "设备离线,或监控IP或端口错误",
        "time": "",
      }
  };
let lalerts = [];
exports.addAlert = function(alert) {
    lalerts.push(alert);
};
exports.getAlerts = function() {
    return [...lalerts]; // 返回数组的拷贝
};
exports.clearAlerts = function() {
        lalerts.length = 0; // 清空数组
};
let gcurrentalerts = [];
exports.addcurrentAlert = function(alert) {
    gcurrentalerts.push(alert);
};
exports.getcurrentAlerts = function() {
    return [...gcurrentalerts]; // 返回数组的拷贝
};
exports.clearcurrentAlerts = function() {
    gcurrentalerts.length = 0; // 清空数组
};

 const config = {
    "amf": {   
        "ngap": {
            "server": [
                { "address": "127.0.0.5" },
                { "address": "127.0.0.50" },
                { "address": "127.0.0.51" },
                { "address": "127.0.0.52" },
                { "address": "127.0.0.53" }
            ]
        },

        "guami": [
            {
                "plmn_id": { "mcc": 999, "mnc": 70 },
                "amf_id": { "region": 2, "set": 1 }
            }
        ],
        "tai": [
            {
                "plmn_id": { "mcc": 999, "mnc": 70 },
                "tac": 1
            }
        ],
        "plmn_support": [
            {
                "plmn_id": { "mcc": 999, "mnc": 70 },
                "s_nssai": [
                    { "sst": 1 }
                ]
            }
        ],

    }
};
