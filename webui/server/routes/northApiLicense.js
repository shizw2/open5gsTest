const express = require('express');
const router = express.Router();
const net = require('net');
const fetchDataFromTelnet = require('../models/fetchDataFromTelnet'); 

// 收到info请求，从telnet服务器获取信息
router.get('/info', async (req, res) => {
  try {
    const data = await fetchDataFromTelnet(2300, '127.0.0.5', '5gc', 'showLicenseInfo()');
    const parsedData = parseLicenseInfo(data);
    //res.json({ result: "OK", result_set: parsedData });
    // 检查 parsedData 是否为空
    const result = Object.keys(parsedData).length === 0 ? "FAIL" : "OK";
    const result_set = Object.keys(parsedData).length === 0 ? {} : parsedData;

    // 构建响应对象
    res.json({ result, result_set });
  } catch (err) {
    console.error('Error fetching data from Telnet:', err);
    res.status(500).json({ result: "ERROR", message: err.message });
  }

  function parseLicenseInfo(data) {
    const infoParts = data.split(',');

    // 检查字段数量是否至少为12个
    if (infoParts.length < 12) {
      return {}; // 如果字段少于12个，返回空对象
    }

    const parsedData = {}; // 初始化 parsedData 对象

    // 提取每个属性的值
    parsedData.licBaseStart = infoParts[0].split(':')[1].trim();
    parsedData.licBaseExpire = infoParts[1].split(':')[1].trim();
    parsedData.licBaseDays = parseInt(infoParts[2].split(':')[1].trim(), 10);
    parsedData.licBaseValid = infoParts[3].split(':')[1].trim();
    parsedData.licBaseCreate = infoParts[4].match(/:(.*)/)[1].trim()
    parsedData.licBaseType = infoParts[5].split(':')[1].trim();
    parsedData.licBaseToday = infoParts[6].match(/:(.*)/)[1].trim()
    parsedData.licBaseCustomer = infoParts[7].split(':')[1].trim();
    parsedData.licBaseSerialno = infoParts[8].split(':')[1].trim();
    parsedData.maximumRanNodes = parseInt(infoParts[9].split(':')[1].trim(), 10);
    parsedData.maximumSubscriptions = parseInt(infoParts[10].split(':')[1].trim(), 10);
    parsedData.maximumRegistrations = parseInt(infoParts[11].split(':')[1].trim(), 10);    

    return parsedData;
  }
});

// 导出路由
module.exports = router;