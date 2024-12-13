const express = require('express');
const router = express.Router();
const globledata = require('../models/globle.js'); 
const configinfo = globledata.configinfo;
const fetchConfigInfo = require('../models/fetchconfig.js');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const yaml = require('js-yaml');
const { handleError, errorCodes } = require('./northApiCommon');

//3.01 获取参数配置
router.get('/info', (req, res) => {
  // 构建成功响应

  fetchConfigInfo(req, res);
  /*
  const response = {
      result: "OK",
      result_set: configinfo,
      //result_set: true,
  };
  
  // 返回JSON响应
  res.json(response);
  */
});

router.put('/info', (req, res) => {
  console.log('configinfo PUT.........!!' );
  // 处理 PUT 请求的逻辑

  let newConfiginfo = req.body;

  //修改amf.yaml文件
  let configPath = path.join(__dirname, '../../../install/etc/5gc/amf.yaml');
  //let configPath = path.join(__dirname,'../../nfconfig/amf.yaml');

  if (!newConfiginfo) {
    return handleError(res, 400, 1003,'Configinfo is required','info', { desc: 'Configinfo is required' });
  }

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);

    // 更新amf相关配置
    config.amf.metrics.server[0].address = newConfiginfo.oamIp;
    config.amf.metrics.server[0].mask    = newConfiginfo.oamNetmask;

    config.amf.sbi.server[0].address = newConfiginfo.serviceIp;
    config.amf.sbi.server[0].mask    = newConfiginfo.serviceNetmask;

    config.amf.guami[0].amf_id.region  = newConfiginfo.guami.amf_region_id;
    config.amf.guami[0].amf_id.set     = newConfiginfo.guami.amf_set_id;
    config.amf.guami[0].amf_id.pointer = newConfiginfo.guami.amf_pointer;

    config.amf.tai = [];

    newConfiginfo.plmnList.forEach(plmn => {
      let singletai = {
        "plmn_id": {
          "mcc": "",
          "mnc": "",
        },
        "tac": [],
      };

      const tacArray = plmn.tac_list.split(',');
      const tacStringArray = tacArray.map(item => String(item));
      singletai.tac = tacStringArray;
      singletai.plmn_id.mcc = plmn.mcc;
      singletai.plmn_id.mnc = plmn.mnc;

      config.amf.tai.push(singletai);
    });

    config.amf.plmn_support[0].s_nssai = [];
    newConfiginfo.nssaiList.forEach(nssai => {
      let singles_nssai = {
        "sst": "",
        "sd": "",
      };

      singles_nssai.sst = parseInt(nssai.sst);
      singles_nssai.sd = nssai.sd;

      config.amf.plmn_support[0].s_nssai.push(singles_nssai);
    });

    // 将更新后的配置写回 YAML 文件
    const updatedContents = yaml.dump(config);
    fs.writeFileSync(configPath, updatedContents, 'utf8');
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'info', { desc: error.message });
  }

  //修改upf.yaml文件
  configPath = path.join(__dirname, '../../../install/etc/5gc/upf.yaml');
  //configPath = path.join(__dirname,'../../nfconfig/upf.yaml');

  if (!newConfiginfo) {
    return handleError(res, 400, 1003,'Configinfo is required','info', { desc: 'Configinfo is required' });
  }

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);

    // 更新upf相关配置
    config.upf.gtpu.server[0].address = newConfiginfo.dataplanIp;
    config.upf.gtpu.server[0].mask    = newConfiginfo.dataplanNetmask;

    // 将更新后的配置写回 YAML 文件
    const updatedContents = yaml.dump(config);
    fs.writeFileSync(configPath, updatedContents, 'utf8');
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'info', { desc: error.message });
  }

  //修改smf.yaml文件
  configPath = path.join(__dirname, '../../../install/etc/5gc/smf.yaml');
  //configPath = path.join(__dirname,'../../nfconfig/smf.yaml');

  if (!newConfiginfo) {
    return handleError(res, 400, 1003,'Configinfo is required','info', { desc: 'Configinfo is required' });
  }

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);

    // 更新smf相关配置
    config.smf.pfcp.client.upf[0].dnn = [];

    newConfiginfo.dnnList.forEach(dnn => {
      config.smf.pfcp.client.upf[0].dnn.push(dnn);
    });

    config.smf.session = [];

    newConfiginfo.ueAddrPoolList.forEach(ueAddrPool => {
      let singleSession = {
        "subnet": "", 
        "num": "",
      };

      singleSession.subnet = ueAddrPool.ipStart+'/'+ ueAddrPool.mask;
      singleSession.num = ueAddrPool.numberOfAddresses;
      config.smf.session.push(singleSession);
    });

    // 将更新后的配置写回 YAML 文件
    const updatedContents = yaml.dump(config);
    fs.writeFileSync(configPath, updatedContents, 'utf8');
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'info', { desc: error.message });
  }

  res.json({
    "result": "OK",
    "result_set": null
  });
});

//3.06 获取模式身份信息
router.get('/role', (req, res) => {
  // 从请求体中获取"group"
  const reqgroup = req.body;

  let response = {
    result: "OK",
    result_set: [],
  };

  //读取并解析sacc.yaml文件
  let configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');
  //let configPath = path.join(__dirname,'../../nfconfig/sacc.yaml');

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);

    config.amfs.forEach(role => {
      let singleRole = {
        "group": "", 
        "node": "",
        "ip": "",
      };
      
      // 根据下划线分割字符串
      let parts = role.node.split("_");

      // 提取第二个和第三个元素
      let group = parts[1];
      let node  = parts[2];
      if (reqgroup.group){
        if(reqgroup.group == group){
          singleRole.group = group;
          singleRole.node  = node;
          singleRole.ip = role.sbi.server[0].address;
          response.result_set.push(singleRole);
        }
      }else{
        singleRole.group = group;
        singleRole.node  = node;
        singleRole.ip = role.sbi.server[0].address;
        response.result_set.push(singleRole);
      }
    });
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'info', { desc: error.message });
  }

  // 返回JSON响应
  res.json(response);
});


//3.10 获取救援模式
router.get('/rescueMode', (req, res) => {
  let configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');
  //let configPath = path.join(__dirname,'../../nfconfig/sacc.yaml');

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);

    // 构建成功响应
    const response = {
      result: "OK",
      result_set: config.global.parameter.rescueMode,
    };

    // 返回JSON响应
    res.json(response);
  } catch (error) {
    handleError(res, 500, 1004,'Failed to read YAML file', 'info', { desc: error.message });
  }
});

//3.11 更改救援模式
router.post('/rescueMode', (req, res) => {
  let newConfiginfo = req.body;
  //const configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');
  let configPath = path.join(__dirname,'../../nfconfig/sacc.yaml');

  if (!newConfiginfo) {
    return handleError(res, 400, 1003,'Configinfo is required','info', { desc: 'Configinfo is required' });
  }

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);
    config.global.parameter.rescueMode = newConfiginfo.rescueMode;

    // 将更新后的配置写回 YAML 文件
    const updatedContents = yaml.dump(config);
    fs.writeFileSync(configPath, updatedContents, 'utf8');
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'info', { desc: error.message });
  }

  // 构建成功响应
  const response = {
    result: "OK",
    result_set: null,
  };

  // 返回JSON响应
  res.json(response);
});

// 导出路由
module.exports = router;