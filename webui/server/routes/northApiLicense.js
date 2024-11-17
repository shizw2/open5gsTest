const express = require('express');
const router = express.Router();
const net = require('net');
const fetchDataFromTelnet = require('../models/fetchdataFromTelnet'); 

// 收到info请求，从telnet服务器获取信息
router.get('/info1', (req, res) => {
  // 为每个请求创建一个新的客户端连接
  const client = new net.Socket();
  console.log("receiving info request");
  // 设置超时时间为1秒
  const timeoutId = setTimeout(() => {
    const errorMessage = 'setTimeout Connection timed out';
    console.error(errorMessage);
    res.status(408).json({ result: "ERROR", message: errorMessage });
    client.end(); // 关闭连接
  }, 5000);


  // 连接到服务端
  client.connect(2300, '127.0.0.5', () => {
    console.log('Connected to server sucess.');
    
    // 连接成功后清除超时计时器
    clearTimeout(timeoutId);
  });

  let cmdSend = false; 
  // 监听数据
  client.on('data', (data) => {
    const dataStr = data.toString();
    console.log('Received: ' + dataStr);

    // 检查是否收到了密码验证提示
    if (dataStr.includes('Password:')) {
      // 发送密码
      console.log('Received password prompt, sending password: ' + dataStr);
      client.write('5gc\n');
    } else if (dataStr.includes('aaa>') && cmdSend === false) {      
      console.log('Sending "showLicenseInfo()" command');
      client.write('showLicenseInfo()\n');
      cmdSend = true;
    } else if (dataStr.includes('RunTime')) {     
      // 解析数据并构建响应对象
      console.log('Received "showLicenseInfo()" response, parsing data');
      const parsedData = parseLicenseInfo(dataStr);
      res.json({ result: "OK", result_set: parsedData });
      // 关闭连接
      client.end();
    }
    
  });

  // 监听错误
  client.on('error', (err) => {
    console.error('Error for telnet: ' + err);
    res.status(500).json({ result: "ERROR", message: err.message });
    clearTimeout(timeoutId); // 清除超时计时器
    client.end(); // 关闭连接
  });

  function parseLicenseInfo(data) {
    const lines = data.split('\n');
    let parsedData = {};
  
    // 找到包含信息的行
    const infoLine = lines.find(line => line.includes('RunTime'));
  
    if (infoLine) {
      // 分割信息行以获取各个属性
      const infoParts = infoLine.split(',');
  
      // 提取每个属性的值
      parsedData.runTime = parseInt(infoParts[0].split(':')[1].trim(), 10);
      parsedData.durationTime = parseInt(infoParts[1].split(':')[1].trim(), 10);
      parsedData.expireTime = infoParts[2].split(':')[1].trim();
      parsedData.ueNum = parseInt(infoParts[3].split(':')[1].trim(), 10);
    }
  
    return parsedData;
  }

});

// 收到info请求，从telnet服务器获取信息
router.get('/info', async (req, res) => {
  try {
    const data = await fetchDataFromTelnet(2300, '127.0.0.5', '5gc', 'showLicenseInfo()');
    const parsedData = parseLicenseInfo(data);
    res.json({ result: "OK", result_set: parsedData });
  } catch (err) {
    console.error('Error fetching data from Telnet:', err);
    res.status(500).json({ result: "ERROR", message: err.message });
  }
  
  function parseLicenseInfo(data) {
    const lines = data.split('\n');
    let parsedData = {};
  
    // 找到包含信息的行
    const infoLine = lines.find(line => line.includes('RunTime'));
  
    if (infoLine) {
      // 分割信息行以获取各个属性
      const infoParts = infoLine.split(',');
  
      // 提取每个属性的值
      parsedData.runTime = parseInt(infoParts[0].split(':')[1].trim(), 10);
      parsedData.durationTime = parseInt(infoParts[1].split(':')[1].trim(), 10);
      parsedData.expireTime = infoParts[2].split(':')[1].trim();
      parsedData.ueNum = parseInt(infoParts[3].split(':')[1].trim(), 10);
    }
  
    return parsedData;
  }
});

// 导出路由
module.exports = router;