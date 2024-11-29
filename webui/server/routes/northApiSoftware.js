const express = require('express');
const router = express.Router();
const globledata = require('../models/globle.js');
const nfStatus = globledata.nfStatus;
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const yaml = require('js-yaml');
const fetchDataFromTelnet = require('../models/fetchDataFromTelnet');
const { handleError, errorCodes } = require('./northApiCommon');

const fiveGDir   = path.join(__dirname, '../../../');

//4.01 查询设备信息
router.get('/info', (req, res) => {
  const configPath = path.join(__dirname, '../../../install/etc/5gc/amf.yaml'); //TODO:确定最终文件

  try {    
    // 读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    
    const config = yaml.load(fileContents);
    // 检查必需的字段是否存在
    const requiredFields = ['deviceType', 'deviceName', 'deviceSeq', 'version'];
    for (const field of requiredFields) {
      if (!config.global || !config.global.parameter|| !config.global.parameter[field]) {
        return handleError(res, 500, 1001, `${configPath} Missing required field: ${field}`, 'info', { desc: `${field} not found in ${configPath}` });
      }
    }
    let response = {
        "result": "OK",
        "result_set": {
            "deviceType": config.global.parameter.deviceType,
            "deviceName": config.global.parameter.deviceName,
            "deviceSeq": config.global.parameter.deviceSeq,
            "version": config.global.parameter.version
        }
    };

    res.json(response);
  } catch (error) {
    handleError(res, 500, 1002, 'Failed to read or parse YAML file', 'info', { desc: error.message });
  }
});

//4.02 修改设备信息
router.put('/info', (req, res) => {
  const configPath = path.join(__dirname, '../../../install/etc/5gc/amf.yaml');
  const newDeviceName = req.body.deviceName;

  if (!newDeviceName) {
    return handleError(res, 400, 1003,'deviceName is required','info', { desc: 'deviceName is required' });
  }

  try {
    // 读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    let config = yaml.load(fileContents);

    // 更新 deviceName
    config.global.deviceName = newDeviceName;

    // 将更新后的配置写回 YAML 文件
    const updatedContents = yaml.dump(config);
    fs.writeFileSync(configPath, updatedContents, 'utf8');

    res.json({
      "result": "OK",
      "result_set": null
    });
  } catch (error) {
    handleError(res, 500, 1004,'Failed to update YAML file', 'info', { desc: error.message });
  }
});

//4.03 查询本机状态信息
router.get('/device', (req, res) => {
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
    if(nfStatus['AMF'].up==='1'){
      deviceStatus.result_set.amf.Status = "START";
      deviceStatus.result_set.amf.Uptime = nfStatus['AMF'].time;
    }else{
      deviceStatus.result_set.amf.Status = "STOP";
      deviceStatus.result_set.amf.Uptime = "";
    }
    if(nfStatus['PCF'].up==='1'){
        deviceStatus.result_set.pcf.Status = "START";
        deviceStatus.result_set.pcf.Uptime = nfStatus['PCF'].time;
      }else{
        deviceStatus.result_set.pcf.Status = "STOP";
        deviceStatus.result_set.pcf.Uptime = "";
    }
    if(nfStatus['SMF'].up==='1'){
        deviceStatus.result_set.smf.Status = "START";
        deviceStatus.result_set.smf.Uptime = nfStatus['SMF'].time;
      }else{
        deviceStatus.result_set.smf.Status = "STOP";
        deviceStatus.result_set.smf.Uptime = "";
    }
    if(nfStatus['UDM'].up==='1'){
        deviceStatus.result_set.udm.Status = "START";
        deviceStatus.result_set.udm.Uptime = nfStatus['UDM'].time;
      }else{
        deviceStatus.result_set.udm.Status = "STOP";
        deviceStatus.result_set.udm.Uptime = "";
    }
    if(nfStatus['UPF'].up==='1'){
        deviceStatus.result_set.upf.Status = "START";
        deviceStatus.result_set.upf.Uptime = nfStatus['UPF'].time;
      }else{
        deviceStatus.result_set.upf.Status = "STOP";
        deviceStatus.result_set.upf.Uptime = "";
    }
    if(nfStatus['node'].up==='1'){
        deviceStatus.result_set.system.Status = "START";
        deviceStatus.result_set.system.Uptime = nfStatus['node'].time;
      }else{
        deviceStatus.result_set.system.Status = "STOP";
        deviceStatus.result_set.system.Uptime = "";
    }
    res.send(deviceStatus);
    
  });

//4.04
router.get('/networkStatus', async (req, res) => {
  try {
    const dataString = await fetchDataFromTelnet(2300, '127.0.0.5', '5gc', 'getnetworkStatus()');
    // 解析 JSON 字符串为对象
    const data = JSON.parse(dataString);
    const result = Object.keys(data).length === 0 ? "FAIL" : "OK";
    const result_set = Object.keys(data).length === 0 ? {} : data;

    // 构建响应对象
    res.json({ result, result_set });
  } catch (err) {
    console.error('Error fetching data from Telnet:', err);
    handleError(res, 500, 1005,'Failed to fetch data', 'networkStatus', { desc: err.message });
  }
});

//4.05 查询基站状态
router.get('/ranNode', async (req, res) => {
  try {
    // 从请求中获取分页参数
    const pageSize = parseInt(req.query.page_size, 10) || 0; 
    const pageNum = parseInt(req.query.page_num, 10) || 0; 

    const command = `getRanNode(${pageSize},${pageNum})`;

    // 调用异步函数获取数据
    const dataString = await fetchDataFromTelnet(2300, '127.0.0.5', '5gc', command);
    const data = JSON.parse(dataString);

    // 检查数据是否为空
    const result = Object.keys(data).length === 0 ? "FAIL" : "OK";
    const result_set = Object.keys(data).length === 0 ? {} : data;

    // 构建响应对象
    res.json({ result, result_set });    
    
  } catch (err) {
    console.error('Error fetching data from Telnet:', err);
    handleError(res, 500, 1005,'Failed to fetch data', 'ranNode', { desc: err.message });
  }
});

//4.06 查询用户状态
router.get('/ueStatus', async (req, res) => {
  try {
    // 从请求中获取分页参数
    const pageSize = parseInt(req.query.limit, 10) || 0; // 使用limit作为页面大小参数
    const pageNum = parseInt(req.query.page, 10) || 0; // 使用page作为页码参数

    const command = `getUeInfo(${pageSize},${pageNum})`; // 假设后端支持getUeInfo命令来获取UE信息

    // 调用异步函数获取数据
    const dataString = await fetchDataFromTelnet(2300, '127.0.0.4', '5gc', command);
    const data = JSON.parse(dataString);

    // 检查数据是否为空
    const result = Object.keys(data).length === 0 ? "FAIL" : "OK";
    const result_set = Object.keys(data).length === 0 ? {} : data;

    // 构建响应对象
    res.json({ result, result_set });    

  } catch (err) {
    console.error('Error fetching data from Telnet:', err);
    handleError(res, 500, 1005, 'Failed to fetch data', 'ueStatus', { desc: err.message });
  }
});

//4.07 日志文件目录
const logDir = path.join(__dirname, '../../../install/var/log/5gc/');

router.get('/debug/file', (req, res) => {
    // 读取日志文件目录
    fs.readdir(logDir, (err, files) => {
      if (err) {
          // 如果读取失败，返回错误信息
          return handleError(res, 500, 1006, 'Failed to read log files','debug', { desc: err.message });
      }
      // 根据 history 参数的值过滤日志文件
      let logFiles = files.filter(file => {
        if (req.query.history === 'true') {
            return file.endsWith('.gz');
        } else {
            return file.endsWith('.log');
        }
      }).map(file => {
        // 返回文件的全路径
        return path.join(logDir, file);
      });

      // 如果读取成功，返回日志文件列表
      res.json({
          "result": "OK",
          "result_set": logFiles
      });
    });
});

//4.08 定义执行系统命令的函数
const executeCommand = (command, callback) => {
  exec(command, (error, stdout, stderr) => {
    if (error) {
      return callback(error, null);
    }
    if (stderr) {
      return callback(new Error(stderr), null);
    }
    callback(null, stdout);
  });
};

router.post('/system', (req, res) => {
  const { actionType } = req.body; // 从请求体中获取 actionType

  if (!['start', 'stop', 'restart', 'reboot'].includes(actionType)) {
    return handleError(res, 400, 1007,'Invalid actionType', 'system', { desc: 'Invalid actionType',invalidParam: { param: 'actionType', reason: 'Invalid actionType' } });
  }

  let command;
  switch (actionType) {
    case 'start':
      command = `${fiveGDir}service.sh start`; 
      break;
    case 'stop':
      command = `${fiveGDir}service.sh stop`; 
      break;
    case 'restart':
      command = `${fiveGDir}service.sh restart`;
      break;
    case 'reboot':
      command = 'reboot';
      break;
  }

  // 执行系统命令
  executeCommand(command, (error, stdout) => {
    if (error) {
      return handleError(res, 500, 1008, 'Failed to execute command', 'system', { desc: error.message });
    }

    const response = {
      result: "OK",
      result_set: null
    };
    res.json(response);
  });
});

//4.13 查看磁盘空间
router.get('/diskUsage', (req, res) => {
  getDiskUsage((error, diskInfo) => {
    if (error) {
      return handleError(res, 500, 1009, 'Failed to get disk usage','diskUsage', { desc: error.message });
    }
    const response = {
      result: "OK",
      result_set: diskInfo
    };
    res.json(response);
  });
});

//定义 getDiskUsage 函数
const getDiskUsage = (callback) => {
  exec('df -B 1 / | tail -n +2', (error, stdout, stderr) => {
    if (error) {
      return callback(error, null);
    }
    if (stderr) {
      return callback(new Error(stderr), null);
    }

    const lines = stdout.trim().split('\n');
    let diskInfo = {
      total: 0,
      used: 0,
      available: 0,
      usage: 0
    };

    lines.forEach(line => {
      const columns = line.split(/\s+/);
      if (columns.length >= 6) { // 确保列数足够
        diskInfo.total += parseInt(columns[1], 10);
        diskInfo.used += parseInt(columns[2], 10);
        diskInfo.available += parseInt(columns[3], 10);
      }
    });

    // 计算使用率
    diskInfo.usage = (diskInfo.used / diskInfo.total) * 100;

    callback(null, diskInfo);
  });
};

// 导出路由
module.exports = router;