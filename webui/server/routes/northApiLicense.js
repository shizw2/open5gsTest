const express = require('express');
const router = express.Router();
const net = require('net');
const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');
const fetchDataFromTelnet = require('../models/fetchDataFromTelnet'); 

//5.01
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

//5.02
const licenseDir   = path.join(__dirname, '../../../license');
const binDir       = path.join(__dirname, '../../../install/bin');
const downloadDir  = path.join(__dirname, '../../../download');

router.get('/file', (req, res) => {
    // 读取目录
    fs.readdir(licenseDir, (err, files) => {
      if (err) {
          // 如果读取失败，返回错误信息
          res.status(500).send({
              "result": "ERROR",
              "message": "Failed to read license files."
          });
      } else {
          let licenseFiles = files.filter(file => file.endsWith('.dat'));

          // 获取文件及其修改时间的数组
          let filesWithModTime = [];

          // 遍历文件列表，获取每个文件的修改时间
          licenseFiles.forEach((file) => {
              fs.stat(path.join(licenseDir, file), (err, stats) => {
                  if (err) {
                      // 如果获取文件状态失败，返回错误信息
                      res.status(500).send({
                          "result": "ERROR",
                          "message": "Failed to get file stats."
                      });
                  } else {
                      const modTime = stats.mtime.toLocaleString('zh-CN', {
                          year: 'numeric',
                          month: '2-digit',
                          day: '2-digit',
                          hour: '2-digit',
                          minute: '2-digit',
                          second: '2-digit',
                          hour12: false // 使用24小时制
                      }).replace(/\//g, '-'); // 将日期中的斜杠替换为短横线以符合格式要求

                      // 将文件名和修改时间添加到数组中
                      filesWithModTime.push({
                          fileName: file,
                          modTime: modTime
                      });

                      // 检查是否所有文件都已处理完毕
                      if (filesWithModTime.length === licenseFiles.length) {
                          // 如果读取成功，返回文件列表和修改时间
                          res.json({
                              "result": "OK",
                              "result_set": filesWithModTime
                          });
                      }
                  }
              });
          });
      }
  });
});

//5.03
// 全局变量，存储当前激活的文件名
let activatedFileName;


router.post('/activate', (req, res) => {
  const { fileName } = req.body;

  if (!fileName) {
      return res.status(400).json({
          "result": "FAIL",
          "message": "File name is required."
      });
  }

  const file = path.join(licenseDir, fileName);

  // 检查文件是否存在
  fs.access(file, fs.constants.F_OK, (err) => {
      if (err) {
          return res.status(404).json({
              "result": "FAIL",
              "message": "File not found."
          });
      }

      // 激活license
      activateLicense (file, (err) => {
          if (err) {
              return res.status(500).json({
                  "result": "FAIL",
                  "message": "Activation failed."
              });
          }
          activatedFileName = fileName
          // 返回成功响应
          res.json({
              "result": "OK",
              "result_set": null
          });
      });
  });
});

function activateLicense(file, callback) {
  // 构建目标文件路径  
  const destFile = path.join(binDir, 'License.dat');

  // 拷贝文件
  fs.copyFile(file, destFile, (err) => {
    if (err) {
      console.error('Error copying license file:', err);
      return callback(err);
    }

    console.log(`License file ${file} copied successfully to ${destFile}.`);

    // 构建相对于当前执行脚本的路径
    const shellCmd = path.join(__dirname, '../../../service.sh');
    // 执行重启命令
    exec(`${shellCmd} restart`, (err, stdout, stderr) => {
      if (err) {
        console.error('Error restarting service:', err);
        return callback(err);
      }
      console.log('Service restarted successfully:', stdout);
      callback(null); // 激活成功
    });
  });
}

//5.04
// 删除文件的路由
router.delete('/file', (req, res) => {
  const { fileName } = req.body;

  if (!fileName) {
      return res.status(400).json({
          "result": "FAIL",
          "message": "File name is required."
      });
  }

  const filePath = path.join(licenseDir, fileName);

  // 检查文件是否存在
  fs.access(filePath, fs.constants.F_OK, (err) => {
      if (err) {
          return res.status(404).json({
              "result": "FAIL",
              "message": "File not found."
          });
      }

      // 删除文件
      fs.unlink(filePath, (err) => {
          if (err) {
              return res.status(500).json({
                  "result": "FAIL",
                  "message": "Failed to delete the file."
              });
          }

          console.log(`File ${fileName} deleted successfully.`);
          // 如果是激活文件，删除对应 bin 目录下的文件
          if (fileName === activatedFileName) {
            fs.unlink(path.join(binDir, 'License.dat'), (err) => {
                if (err) {
                    return res.status(500).json({
                        "result": "FAIL",
                        "message": `Failed to delete the activated file: ${err.message}`
                    });
                }
                console.log(`Activated file License.dat deleted successfully from ${binDir}.`);
                res.json({
                    "result": "OK",
                    "result_set": null
                });
            });
          } else {
            res.json({
                "result": "OK",
                "result_set": null
            });
          }
      });
  });
});


//5.6
// 导入文件的路由
router.post('/import', (req, res) => {

  const { fileName } = req.body;
  const srcFilePath = path.join(downloadDir, fileName);
  const destFilePath = path.join(licenseDir, fileName);

  // 检查文件是否存在
  fs.access(srcFilePath, fs.constants.F_OK, (err) => {
      if (err) {
          return res.status(404).json({
              "result": "FAIL",
              "message": "File not found in download directory."
          });
      }

      // 拷贝文件
      fs.copyFile(srcFilePath, destFilePath, (err) => {
          if (err) {
              return res.status(500).json({
                  "result": "FAIL",
                  "message": `Failed to copy the file: ${err.message}`
              });
          }

          console.log(`File ${fileName} imported successfully to ${licenseDir}.`);

          res.json({
              "result": "OK",
              "result_set": null
          });
      });
  });
});

// 导出路由
module.exports = router;