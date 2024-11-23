const express = require('express');
const router = express.Router();
const net = require('net');
const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');
const fetchDataFromTelnet = require('../models/fetchDataFromTelnet');
const { handleError, errorCodes } = require('./northApiCommon');

const licenseDir   = path.join(__dirname, '../../../license');
const binDir       = path.join(__dirname, '../../../install/bin');
const downloadDir  = path.join(__dirname, '../../../download');
// 全局变量，存储当前激活的文件名
let activatedFileName;



//5.01 收到info请求，从telnet服务器获取信息
router.get('/info', async (req, res) => {
  try {
    const data = await fetchDataFromTelnet(2300, '127.0.0.5', '5gc', 'showLicenseInfo()');

    // 解析 JSON 字符串为对象
    const parsedData = JSON.parse(data);
    const result = Object.keys(parsedData).length === 0 ? "FAIL" : "OK";
    const result_set = Object.keys(parsedData).length === 0 ? {} : parsedData;

    // 构建响应对象
    res.json({ result, result_set });
  } catch (err) {
    console.error('Error fetching data from Telnet:', err);
    handleError(res, 500, errorCodes.COMMON_ERROR, 'Failed to fetch data', '', { desc: err.message});
  }
});

//5.02 获取文件列表
router.get('/file', (req, res) => {
    // 读取目录
    fs.readdir(licenseDir, (err, files) => {
      if (err) {
          // 如果读取失败，返回错误信息
          return handleError(res, 500, errorCodes.FILE_ERROR, 'Failed to read directory', '', { desc: err.message } );
      }
      let licenseFiles = files.filter(file => file.endsWith('.dat'));
      let filesWithModTime = [];

      // 遍历文件列表，获取每个文件的修改时间
      licenseFiles.forEach((file) => {
          fs.stat(path.join(licenseDir, file), (err, stats) => {
              if (err) {
                  // 如果获取文件状态失败，返回错误信息
                  return handleError(res, 500, errorCodes.FILE_ERROR, 'Failed to get stats', 'file', { desc: err.message });
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
                          result: "OK",
                          result_set: filesWithModTime
                      });
                  }
              }
          });
      });
  });
});

//5.03 激活 License
router.post('/activate', (req, res) => {
  const { fileName } = req.body;

  if (!fileName) {
      return handleError(res, 400, errorCodes.COMMON_ERROR, 'File name is required','', { desc: 'File name is required',invalidParam: { param: 'fileName', reason: 'fileName is null' }  });
  }

  // 校验文件名后缀
  if (!fileName.endsWith('.dat') && !fileName.endsWith('.data')) {
    return handleError(res, 400, errorCodes.PARAMETER_ERROR, 'Invalid file name extension', '', { desc: 'File name must end with .dat or .data', invalidParam: { param: 'fileName', reason: `fileName must end with .dat or .data, got ${fileName}` } });
  }  

  const file = path.join(licenseDir, fileName);

  // 检查文件是否存在
  fs.access(file, fs.constants.F_OK, (err) => {
      if (err) {
          return handleError(res, 404, 1005, 'File not found', '', { desc: err.message });
      }

      // 激活license
      activateLicense(file, (err) => {
          if (err) {
              return handleError(res, 500, 1006, 'Activation failed', '', { desc: err.message });
          }
          activatedFileName = fileName;
          // 返回成功响应
          res.json({
              result: "OK",
              result_set: null
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

//5.04 删除文件的路由
router.delete('/file', (req, res) => {
  const { fileName } = req.body;

  if (!fileName) {
      return handleError(res,  400, 1007, 'File name is required','', { desc: 'File name is required',invalidParam: { param: 'fileName', reason: 'File name is required' }});
  }

  const filePath = path.join(licenseDir, fileName);

  // 检查文件是否存在
  fs.access(filePath, fs.constants.F_OK, (err) => {
      if (err) {
          return handleError(res, 404, 1008, 'File not found', '', { desc: err.message });
      }

      // 删除文件
      fs.unlink(filePath, (err) => {
          if (err) {
              return handleError(res, 500, 1009, 'Failed to delete file', '', { desc: err.message });
          }

          console.log(`File ${fileName} deleted successfully.`);
          // 如果是激活文件，删除对应 bin 目录下的文件
          if (fileName === activatedFileName) {
            fs.unlink(path.join(binDir, 'License.dat'), (err) => {
                if (err) {
                    return handleError(res,  500, 1010, 'Failed to delete activated file', '', { desc: err.message });
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

//5.5 导出防伪信息
router.post('/export', (req, res) => {
    // 读取 binDir 目录下的所有文件
    fs.readdir(binDir, (err, files) => {
      if (err) {
        return handleError(res, 500, 1011, 'Failed to read bin directory', 'export', { desc: err.message });
      }

      // 过滤出以 .id 结尾的文件
      const idFiles = files.filter(file => path.extname(file) === '.id');

      if (idFiles.length === 0) {
        return handleError(res, 404, 1012, 'No .id files found', '', { desc: `No .id files found in ${binDir} directory` });
      }

      // 假设我们只处理第一个 .id 文件，您可以根据需要调整逻辑
      const srcFilePath = path.join(binDir, idFiles[0]);
      const destFilePath = path.join(downloadDir, idFiles[0]);

      // 拷贝文件
      fs.copyFile(srcFilePath, destFilePath, (err) => {
        if (err) {
          return handleError(res, 500, 1013,'Failed to copy the file', 'export', { desc: err.message });
        }
  
        console.log(`File ${srcFilePath} exported successfully to ${downloadDir}.`);
  
        res.json({
          "result": "OK",
          "result_set": idFiles[0] 
        });
      });
    });
  });
  

//5.6 导入文件的路由
router.post('/import', (req, res) => {
  const { fileName } = req.body;
  const srcFilePath = path.join(downloadDir, fileName);
  const destFilePath = path.join(licenseDir, fileName);

  // 检查文件是否存在
  fs.access(srcFilePath, fs.constants.F_OK, (err) => {
      if (err) {
          return handleError(res, 404, 1014, `File ${fileName} not found in download directory`, '', { desc: err.message });
      }

      // 拷贝文件
      fs.copyFile(srcFilePath, destFilePath, (err) => {
          if (err) {
              return handleError(res, 500, 1015, 'Failed to copy the file', 'import', { desc: err.message } );
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