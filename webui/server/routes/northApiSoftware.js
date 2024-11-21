const express = require('express');
const router = express.Router();
const globledata = require('../models/globle.js'); 
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const yaml = require('js-yaml');
const fetchDataFromTelnet = require('../models/fetchDataFromTelnet'); 

//4.01
// 配置文件路径
router.get('/info', (req, res) => {
  const configPath = path.join(__dirname, '../../../install/etc/5gc/amf.yaml');//TODO:确定最终文件

  try {    
    // 读取并解析 YAML 配置文件
    const fileContents = fs.readFileSync(configPath, 'utf8');
    
    const config = yaml.load(fileContents);
    // 检查必需的字段是否存在
    const requiredFields = ['deviceType', 'deviceName', 'deviceSeq', 'version'];
    for (const field of requiredFields) {
      if (!config.global || !config.global[field]) {
        return res.status(500).json({
          "result": "FAIL",
          "message": `Missing required field: ${field}`
        });
      }
    }
    let response = {
        "result": "OK",
        "result_set": {
            "deviceType": config.global.deviceType,
            "deviceName": config.global.deviceName,
            "deviceSeq": config.global.deviceSeq,
            "version": config.global.version
        }
    };

    res.json(response);
  } catch (error) {
    res.status(500).json({ "result": "FAIL", "message": error.message });
  }
});

//4.02
router.put('/info', (req, res) => {
  const configPath = path.join(__dirname, '../../../install/etc/5gc/amf.yaml');
  const newDeviceName = req.body.deviceName;

  if (!newDeviceName) {
    return res.status(400).json({ "result": "FAIL", "message": "deviceName is required" });
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
    res.status(500).json({ "result": "FAIL", "message": error.message });
  }
});


//4.03
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
    res.status(500).json({ result: "ERROR", message: err.message });
  }
});

//4.07
// 日志文件目录
const logDir = path.join(__dirname, '../../../install/var/log/5gc/');

router.get('/debug/file', (req, res) => {
    // 读取日志文件目录
    fs.readdir(logDir, (err, files) => {
      if (err) {
          // 如果读取失败，返回错误信息
          res.status(500).send({
              "result": "ERROR",
              "message": "Failed to read log files."
          });
      } else {
          // // 根据 history 参数的值过滤日志文件
          // let logFiles;
          // if (req.query.history === 'true') {
          //     logFiles = files.filter(file => file.endsWith('.gz'));
          // } else {
          //     logFiles = files.filter(file => file.endsWith('.log'));
          // }
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
      }
  });
});

//4.08
// 定义执行系统命令的函数
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

  if (['start', 'stop', 'restart', 'reboot'].includes(actionType)) {
    let command;
    switch (actionType) {
      case 'start':
        command = '/home/5gc/service.sh start'; 
        break;
      case 'stop':
        command = '/home/5gc/service.sh stop'; 
        break;
      case 'restart':
        command = '/home/test/service.sh restart'; //TODO:需要替换为5gc
        break;
      case 'reboot':
        command = 'reboot';
        break;
      default:
        return res.status(400).json({
          result: "FAIL",
          error: "Invalid actionType"
        });
    }

    // 执行系统命令
    executeCommand(command, (error, stdout) => {
      if (error) {
        return res.status(500).json({
          result: "FAIL",
          error: error.message
        });
      }

      // 构建成功响应
      const response = {
        result: "OK",
        result_set: null
      };

      // 返回JSON响应
      res.json(response);
    });
  } else {
    // 如果 actionType 不是有效的操作，返回失败响应
    res.status(400).json({
      result: "FAIL",
      error: "Invalid actionType"
    });
  }
});


//4.13
// 定义 getDiskUsage 函数
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


// 路由处理
router.get('/diskUsage', (req, res) => {
  getDiskUsage((error, diskInfo) => {
    if (error) {
      res.status(500).json({
        result: "FAIL",
        error: error.message
      });
    } else {
      const response = {
        result: "OK",
        result_set: diskInfo
      };
      res.json(response);
    }
  });
});



// 导出路由
module.exports = router;