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

//3.02 修改参数配置
router.put('/info', (req, res) => {
  console.log('configinfo PUT.........!!' );
  // 处理 PUT 请求的逻辑

  let newConfiginfo = req.body;

  //修改amf.yaml文件
  let configPath = path.join(__dirname, '../../../install/etc/5gc/amf.yaml');

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

//3.03 导入配置文件
router.post('/import', (req, res) => {
  // 从请求体中获取file信息
  const fileName = req.body.fileName;
  const fileType = req.body.fileType;

  if (fileType === 'T1'){//T1模式，按本系统yaml配置文件处理
    //1、导入tar包
    //2、解压tar包
    //3、拷贝yaml文件至path.join(__dirname, '../../../install/etc/5gc')
  }
  else if (fileType === 'T2'){//T2模式，根据csv配置文件处理
    //1、读取csv文件
    //2、解析csv文件
    //3、生成yaml文件
    //4、导入yaml文件
  }else{
    return handleError(res, 400, 1003,'Invalid file type', 'import', { desc: 'Invalid file type' });
  }

/*
  //备份当前系统配置
  //获取当前日期和时间
  const now = new Date();
  const dateTime = now.toISOString().replace(/:/g, '-').replace(/\.\d+Z/, '');

  // 创建备份目录名
  //const dirName = '../../../install/etc/5gc/config_bak_${dateTime}';
  const bakdirName = '../../nfconfig/config_bak_${dateTime}';

  // 创建备份目录的路径
  const bakdirPath = path.join(__dirname, bakdirName);

  try {
    // 同步创建目录
    fs.mkdirSync(bakdirPath, { recursive: true });
    console.log(`Directory created successfully: ${bakdirPath}`);
  } catch (err) {
    console.error('Directory creation failed', err);
  }

  // 复制配置文件到备份目录
  //const configPath = path.join(__dirname, '../../../install/etc/5gc');
  const configPath = path.join(__dirname,'../../nfconfig');
  let entries = fs.readdirSync(configPath, { withFileTypes: true });

  try {
    for (let entry of entries) {
      const sourcePath = path.join(configPath, entry.name);
      const targetPath = path.join(bakdirPath, entry.name);

      if (entry.isDirectory()) {
        // 如果是目录，则递归复制
        //fs.mkdirSync(targetPath, { recursive: true });
        //copyDirectoryContentsSync(sourcePath, targetPath);
      } else {
        // 如果是文件，则复制文件
        fs.copyFileSync(sourcePath, targetPath);
      }
    } 
  }catch (error) {
    handleError(res, 500, 1004,'Failed to copy YAML file', 'info', { desc: error.message });
  }

  //执行恢复出厂设置
  const defaultConfigPath = path.join(__dirname, '../../../configs/5gc');
  entries = fs.readdirSync(defaultConfigPath, { withFileTypes: true });
  for (let entry of entries) {
    const sourcePath = path.join(defaultConfigPath, entry.name);
    const targetPath = path.join(configPath, entry.name);

    try {
      if (entry.isDirectory()) {
        // 如果是目录，则递归复制
        //fs.mkdirSync(targetPath, { recursive: true });
        //copyDirectoryContentsSync(sourcePath, targetPath);
      } else {
        // 如果是文件，则复制文件
        fs.copyFileSync(sourcePath, targetPath);
        fs.renameSync(targetPath.replace(/\.yaml$/, '.in'), targetPath);
      }
    } catch (error) {
      handleError(res, 500, 1004,'Failed to copy YAML file', 'info', { desc: error.message });
    }
  }
*/

  // 构建成功响应
  const response = {
    result: "OK",
    result_set: null,
  };

  // 返回JSON响应
  res.json(response);
});

//3.04 导出配置文件
router.post('/export', (req, res) => {

});

//3.05 配置文件应用，此接口用于模式切换
router.put('/role', (req, res) => {
  // 从请求体中获取"role"
  const setRole = req.body;

  //读取并解析sacc.yaml文件
  let configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');

  if (!setRole) {
    return handleError(res, 400, 1003,'setRoleInfo is required','role', { desc: 'Configinfo is required' });
  }

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);
    config.global.parameter.role = setRole.role;

    if (setRole.role === 'T2') {
      config.global.parameter.group = setRole.group;
      config.global.parameter.node  = setRole.node;
    }

    // 将更新后的配置写回 YAML 文件
    const updatedContents = yaml.dump(config);
    fs.writeFileSync(configPath, updatedContents, 'utf8');

    //通知前台业务系统切换模式
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'watch', { desc: error.message });
  }

  // 构建成功响应
  const response = {
    result: "OK",
    result_set: null,
  };

  // 返回JSON响应
  res.json(response);
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
    handleError(res, 500, 1004,'Failed to update YAML file', 'role', { desc: error.message });
  }

  // 返回JSON响应
  res.json(response);
});

//3.07 查询无人值守
router.get('/watch', (req, res) => {
  let configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);

    // 构建成功响应
    const response = {
      result: "OK",
      result_set: config.global.parameter.watchType,
    };

    // 返回JSON响应
    res.json(response);
  } catch (error) {
    handleError(res, 500, 1004,'Failed to read YAML file', 'watch', { desc: error.message });
  }
});

//3.08 救援模式
router.post('/watch', (req, res) => {
  let newConfiginfo = req.body;
  let configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');

  if (!newConfiginfo) {
    return handleError(res, 400, 1003,'Configinfo is required','watch', { desc: 'Configinfo is required' });
  }

  try {
    //读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);
    config.global.parameter.watchType = newConfiginfo.watchType;

    // 将更新后的配置写回 YAML 文件
    const updatedContents = yaml.dump(config);
    fs.writeFileSync(configPath, updatedContents, 'utf8');
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'watch', { desc: error.message });
  }

  // 构建成功响应
  const response = {
    result: "OK",
    result_set: null,
  };

  // 返回JSON响应
  res.json(response);
});

//3.09 恢复出厂设置
router.post('/reset', (req, res) => {
  //备份当前系统配置
  //获取当前日期和时间
  const now = new Date();
  const dateTime = now.toISOString().replace(/:/g, '-').replace(/\.\d+Z/, '');

  // 创建备份目录名
  const bakdirName = `../../../install/etc/5gc/config_bak_${dateTime}`;

  // 创建备份目录的路径
  const bakdirPath = path.join(__dirname, bakdirName);

  try {
    // 同步创建目录
    fs.mkdirSync(bakdirPath, { recursive: true });
    console.log(`Directory created successfully: ${bakdirPath}`);
  } catch (err) {
    console.error('Directory creation failed', err);
  }

  // 复制配置文件到备份目录
  const configPath = path.join(__dirname, '../../../install/etc/5gc');
  let entries = fs.readdirSync(configPath, { withFileTypes: true });

  try {
    for (let entry of entries) {
      const sourcePath = path.join(configPath, entry.name);
      const targetPath = path.join(bakdirPath, entry.name);

      if (entry.isDirectory()) {
        // 如果是目录，则递归复制
        //fs.mkdirSync(targetPath, { recursive: true });
        //copyDirectoryContentsSync(sourcePath, targetPath);
      } else {
        // 如果是文件，则复制文件
        fs.copyFileSync(sourcePath, targetPath);
      }
    } 
  }catch (error) {
    handleError(res, 500, 1004,'Failed to copy YAML file', 'reset', { desc: error.message });
  }

  //执行恢复出厂设置
  const defaultConfigPath = path.join(__dirname, '../../../configs/5gc');
  entries = fs.readdirSync(defaultConfigPath, { withFileTypes: true });
  for (let entry of entries) {
    const sourcePath = path.join(defaultConfigPath, entry.name);
    const targetPath = path.join(configPath, entry.name.replace(/\.in$/, ''));

    try {
      if (entry.isDirectory()) {
        // 如果是目录，则递归复制
        //fs.mkdirSync(targetPath, { recursive: true });
        //copyDirectoryContentsSync(sourcePath, targetPath);
      } else {
        // 如果是文件，则复制文件
        fs.copyFileSync(sourcePath, targetPath);
      }
    } catch (error) {
      handleError(res, 500, 1004,'Failed to copy YAML file', 'reset', { desc: error.message });
    }
  }

  // 构建成功响应
  const response = {
    result: "OK",
    result_set: null,
  };

  // 返回JSON响应
  res.json(response);
});

//3.10 获取救援模式
router.get('/rescueMode', (req, res) => {
  let configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');

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
    handleError(res, 500, 1004,'Failed to read YAML file', 'rescueMode', { desc: error.message });
  }
});

//3.11 更改救援模式
router.post('/rescueMode', (req, res) => {
  let newConfiginfo = req.body;
  let configPath = path.join(__dirname, '../../../install/etc/5gc/sacc.yaml');

  if (!newConfiginfo) {
    return handleError(res, 400, 1003,'Configinfo is required','rescueMode', { desc: 'Configinfo is required' });
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
    handleError(res, 500, 1004,'Failed to update YAML file', 'rescueMode', { desc: error.message });
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