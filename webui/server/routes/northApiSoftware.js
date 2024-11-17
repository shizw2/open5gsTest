const express = require('express');
const router = express.Router();
const globledata = require('../models/globle.js'); 
const nfStatus = globledata.nfStatus;

router.get('/device', (req, res) => {
    //res.send('Hello World2');
    //fetchAlerts(res);
    console.log('device.........!!' );
    const deviceStatus={
        "result": "OK",
        "result_set": {
            "amf": {
                "Status": "STOP",
                "Uptime": ""
            },
            "pcf": {
                "Status": "STOP",
                "Uptime": ""
            },
            "smf": {
                "Status": "STOP",
                "Uptime": ""
            },
            "system": {
                "Status": "STOP",
                "Uptime": ""
            },
            "udm": {
                "Status": "STOP",
                "Uptime": ""
            },
            "upf": {
                "Status": "STOP",
                "Uptime": ""
            }
        }
      };
    res.setHeader('Content-Type', 'application/json; charset=utf-8');
    //console.log(nfStatus);
    if(nfStatus['AMF'].up==='1'){
      deviceStatus.result_set.amf.Status = "";
      deviceStatus.result_set.amf.Uptime = nfStatus['AMF'].time;
    }else{
      deviceStatus.result_set.amf.Status = "STOP";
      deviceStatus.result_set.amf.Uptime = "";
    }
    if(nfStatus['PCF'].up==='1'){
        deviceStatus.result_set.pcf.Status = "";
        deviceStatus.result_set.pcf.Uptime = nfStatus['PCF'].time;
      }else{
        deviceStatus.result_set.pcf.Status = "STOP";
        deviceStatus.result_set.pcf.Uptime = "";
    }
    if(nfStatus['SMF'].up==='1'){
        deviceStatus.result_set.smf.Status = "";
        deviceStatus.result_set.smf.Uptime = nfStatus['SMF'].time;
      }else{
        deviceStatus.result_set.smf.Status = "STOP";
        deviceStatus.result_set.smf.Uptime = "";
    }
    if(nfStatus['UDM'].up==='1'){
        deviceStatus.result_set.udm.Status = "";
        deviceStatus.result_set.udm.Uptime = nfStatus['UDM'].time;
      }else{
        deviceStatus.result_set.udm.Status = "STOP";
        deviceStatus.result_set.udm.Uptime = "";
    }
    if(nfStatus['UPF'].up==='1'){
        deviceStatus.result_set.upf.Status = "";
        deviceStatus.result_set.upf.Uptime = nfStatus['UPF'].time;
      }else{
        deviceStatus.result_set.upf.Status = "STOP";
        deviceStatus.result_set.upf.Uptime = "";
    }
    if(nfStatus['node'].up==='1'){
        deviceStatus.result_set.system.Status = "";
        deviceStatus.result_set.system.Uptime = nfStatus['node'].time;
      }else{
        deviceStatus.result_set.system.Status = "STOP";
        deviceStatus.result_set.system.Uptime = "";
    }
    res.send(deviceStatus);
    
  });


// 导出路由
module.exports = router;