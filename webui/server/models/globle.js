exports.configinfo ={
    "role": "T2",
    "group": "G1",
    "node": "N1",
    "oamIp": "192.168.100.1", //amf metrics ip
    "serviceIp": "192.168.100.2", //amf sbi ip
    "dataplanIp": "192.168.100.1", //upf ip
    "oamNetmask": "",
    "dataplanNetmask": "",
    "serviceNetmask": "",
    "guami": {
        "amf_region_id": 1,
        "amf_set_id": 1,
        "amf_pointer": 1
    },
    "plmnList": [
        {
            "mcc": "460",
            "mnc": "00",
            "tac_list": "0001FE,0001FF"
        }
    ],
    "nssaiList": [
        {
            "sst": "1",
            "sd": "000001"
        }
    ],
    "dnnList": [
        "ims",
        "net"
    ],
    "ueAddrPoolList": [
        {
            "ipStart": "10.0.0.1",
            "netMask": 16,
            "numberOfAddresses": 500
        },
        {
            "ipStart": "10.0.0.1",
            "netMask": 8,
            "numberOfAddresses": 500
        }
    ]
};

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
