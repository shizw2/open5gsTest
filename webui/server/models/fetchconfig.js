const { config } = require('process');
const globledata = require('./globle.js'); 
const configinfo = globledata.configinfo;
const http = require('http');
const { session } = require('passport');
const { NOTFOUND } = require('dns');
const passport = require('passport');
const fs = require('fs');
const path = require('path');
const yaml = require('js-yaml');
const { handleError, errorCodes } = require('../routes/northApiCommon');

function fetchConfigInfo(req, res) {
  //console.log('> Ready on fetchConfiginfo');

  //const configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');
  let configPath = path.join(__dirname,'../../nfconfig/sacc.yaml');
  
  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let configfile = yaml.load(fileContents);

    configinfo.role  = configfile.global.parameter.role;
    configinfo.group = configfile.global.parameter.group;
    configinfo.node  = configfile.global.parameter.node; 
  } catch (error) {
    handleError(res, 500, 1004,'Failed to read YAML file', 'info', { desc: error.message });
  }
  
  http.get('http://127.0.0.1:3000/api/yaml/NFConfig/amf', (response) => {
    let data = '';
    response.on('data', (chunk) => {
      data += chunk;
    }).on('error', (error) => { 
        console.log(error);
        res.send(error);
    });
    response.on('end', () => {
      const parsedData = JSON.parse(data); // 解析 JSON 数据

      configinfo.oamIp      = parsedData.amf.metrics.server[0].address;
      configinfo.oamNetmask = parsedData.amf.metrics.server[0].mask;
      
      configinfo.serviceIp      = parsedData.amf.sbi.server[0].address;
      configinfo.serviceNetmask = parsedData.amf.sbi.server[0].mask;

      let guamis = parsedData.amf.guami;
      configinfo.guami.amf_region_id = guamis[0].amf_id.region;
      configinfo.guami.amf_set_id    = guamis[0].amf_id.set;
      configinfo.guami.amf_pointer   = guamis[0].amf_id.pointer;

      //configinfo.guami.amf_region_id = 10;

      configinfo.plmnList = [];
      if (parsedData.amf.tai && parsedData.amf.tai.length > 0) {
        parsedData.amf.tai.forEach(tai => {
          let singleplmn = {
            "mcc": "",
            "mnc": "",
            "tac_list": "",
          };
          
          singleplmn.mcc = tai.plmn_id.mcc;
          singleplmn.mnc = tai.plmn_id.mnc;
          singleplmn.tac_list = tai.tac.join(',');

          configinfo.plmnList.push(singleplmn);
        });
      }

      configinfo.nssaiList = [];

      if (parsedData.amf.plmn_support && parsedData.amf.plmn_support.length > 0 && parsedData.amf.plmn_support[0].s_nssai.length > 0) {
        parsedData.amf.plmn_support[0].s_nssai.forEach(s_nssai => {
          let singles_nssai = {
            "sst": "",
            "sd": "",
          };

          singles_nssai.sst = s_nssai.sst;
          singles_nssai.sd = s_nssai.sd;

          configinfo.nssaiList.push(singles_nssai);
        });
      }
      //console.log(data);
      //res.send(data);
    }).on('error', (error) => {
      console.log(error);
      res.send(error);
    });
  });

  http.get('http://127.0.0.1:3000/api/yaml/NFConfig/upf', (response) => {
    let data = '';
    response.on('data', (chunk) => {
      data += chunk;
    }).on('error', (error) => { 
        console.log(error);
        res.send(error);
    });
    response.on('end', () => {
      const parsedData = JSON.parse(data); // 解析 JSON 数据
      
      configinfo.dataplanIp      = parsedData.upf.gtpu.server[0].address;
      configinfo.dataplanNetmask = parsedData.upf.gtpu.server[0].mask;
    }).on('error', (error) => {
      console.log(error);
      res.send(error);
    });
  });

  /*
  http.get('http://127.0.0.1:3000/api/yaml/NFConfig/nssf', (response) => {
    let data = '';
    response.on('data', (chunk) => {
      data += chunk;
    }).on('error', (error) => { 
        console.log(error);
        res.send(error);
    });
    response.on('end', () => {
      const parsedData = JSON.parse(data); // 解析 JSON 数据

      configinfo.nssaiList = [];
      parsedData.nssf.sbi.client.nsi.forEach(nsi => {
        let singles_nssai = {
          "sst": "",
          "sd": "",
        };

        singles_nssai.sst = nsi.s_nssai.sst;
        singles_nssai.sd = nsi.s_nssai.sd;

        configinfo.nssaiList.push(singles_nssai);
      });
    }).on('error', (error) => {
      console.log(error);
      res.send(error);
    });
  });
  */

  http.get('http://127.0.0.1:3000/api/yaml/NFConfig/smf', (response) => {
    let data = '';
    response.on('data', (chunk) => {
      data += chunk;
    }).on('error', (error) => { 
        console.log(error);
        res.send(error);
    });
    response.on('end', () => {
      const parsedData = JSON.parse(data); // 解析 JSON 数据

      configinfo.dnnList = [];
      configinfo.ueAddrPoolList = [];

      if (parsedData.smf.pfcp.client.upf && parsedData.smf.pfcp.client.upf[0].dnn && parsedData.smf.pfcp.client.upf[0].dnn.length > 0) {
        parsedData.smf.pfcp.client.upf[0].dnn.forEach(dnn => {
          if (configinfo.dnnList.includes(dnn) == false) {
            configinfo.dnnList.push(dnn);
          }
        });
      }

      if (parsedData.smf.session && parsedData.smf.session.length > 0) {
        parsedData.smf.session.forEach(session => {
          let singleueAddrPool = {
            "ipStart": "",
            "netMask": "",
            "numberOfAddresses": null,
          };

          [singleueAddrPool.ipStart, singleueAddrPool.netMask] = session.subnet.split('/');
          if (session.range && session.range[0]) {
            const [start, end] = session.range[0].split('-');
            singleueAddrPool.numberOfAddresses = ipV4ToNumber(end) - ipV4ToNumber(start) + 1;
          }

          configinfo.ueAddrPoolList.push(singleueAddrPool);
        });
      }

      const response = {
        result: "OK",
        result_set: configinfo,
        //result_set: true,
      };
    
      // 返回JSON响应
      res.json(response);
    }).on('error', (error) => {
      console.log(error);
      res.send(error);
    });
  });
}

function ipV4ToNumber(ipAddress) {
  const parts = ipAddress.split('.');
  return (parts[0] << 24) + (parts[1] << 16) + (parts[2] << 8) + parseInt(parts[3]);
}

module.exports = fetchConfigInfo;