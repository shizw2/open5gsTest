const express = require('express');
const router = express.Router();


// 引入模块
const config = require('./northApiConfig');
const software = require('./northApiSoftware');
const license = require('./northApiLicense');
const udm = require('./northApiUdm');
const ue = require('./northApiUe');
const imei = require('./imei.js');
//自定义文件列表获取接口
const fileList = require('./northApiFileList');

// 挂载自定义文件列表获取接口
router.use('/wxCm/v1/file', fileList);

// 挂载北向接口各个模块的路由
router.use('/wxCm/v1/config', config);
router.use('/wxCm/v1/config', imei.imei);
router.use('/wxCm/v1/software', software);
router.use('/wxCm/v1/forward/nrf', software);
router.use('/wxCm/v1/license', license);
router.use('/wxCm/v1/forward/udm', udm);
router.use('/wxCm/v1/ue', ue);

router.get('/wxCm/v1/ueStatus', (req, res, next) => {
    // 改变请求的 URL 以匹配 software 模块中的路由    
    req.url = '/wxCm/v1/software/ueStatus';
    console.log("change url to",req.url)
    router.handle(req, res, next);
  });

// 导出路由
module.exports = router;