const { config } = require('process');
const globledata = require('./globle.js'); 
const configinfo = globledata.configinfo;
const http = require('http');
const { session } = require('passport');
const { NOTFOUND } = require('dns');
const passport = require('passport');

function fetchConfigInfo(req, res) {
  //console.log('> Ready on fetchConfiginfo');
  
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

      configinfo.role  = parsedData.global.parameter.role;
      configinfo.group = parsedData.global.parameter.group;
      configinfo.node  = parsedData.global.parameter.node;
      
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
            "numberOfAddresses": "",
          };

          [singleueAddrPool.ipStart, singleueAddrPool.netMask] = session.subnet.split('/');
          singleueAddrPool.numberOfAddresses = session.num;

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

module.exports = fetchConfigInfo;