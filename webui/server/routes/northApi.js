const express = require('express');
const router = express.Router();


// 引入模块
const config = require('./northApiConfig');
const software = require('./northApiSoftware');
const license = require('./northApiLicense');
const udm = require('./northApiUdm');

// 挂载北向接口各个模块的路由
router.use('/wxCm/v1/config', config);
router.use('/wxCm/v1/software', software);
router.use('/wxCm/v1/license', license);
router.use('/wxCm/v1/forward/udm', udm);
router.use('/wxCm/v1/forward/nrf', software);
// 导出路由
module.exports = router;