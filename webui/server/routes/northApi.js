const express = require('express');
const router = express.Router();
const fetchDataFromTelnet = require('../models/fetchDataFromTelnet');
const { handleError, errorCodes } = require('./northApiCommon');

// 引入模块
const config = require('./northApiConfig');
const software = require('./northApiSoftware');
const license = require('./northApiLicense');
const udm = require('./northApiUdm');
const ue = require('./northApiUe');

// 挂载北向接口各个模块的路由
router.use('/wxCm/v1/config', config);
router.use('/wxCm/v1/software', software);
router.use('/wxCm/v1/forward/nrf', software);

router.use('/wxCm/v1/license', license);
router.use('/wxCm/v1/forward/udm', udm);
router.use('/wxCm/v1/forward/ue', ue);


//4.06 查询用户状态

router.get('/wxCm/v1/ueStatus', async (req, res) => {
    try {
        // 从请求中获取分页参数
        const pageSize = parseInt(req.query.limit, 10) || 0; // 使用limit作为页面大小参数
        const pageNum = parseInt(req.query.page, 10) || 0; // 使用page作为页码参数

        const command = `getUeInfo(${pageSize},${pageNum})`;

        // 调用异步函数获取数据
        const dataString = await fetchDataFromTelnet(2301, '127.0.0.5', '5gc', command);
        const data = JSON.parse(dataString);

        // 检查数据是否为空
        const result = Object.keys(data).length === 0 ? "FAIL" : "OK";
        let result_set = Object.keys(data).length === 0 ? {} : data;

        if (result === "OK" && data.ue_infos && data.ue_infos.length > 0) {
            console.log("get dnn and ip from smf.")
            // 对每个UE信息进行额外的Telnet调用以获取dnnList和ueIpAddrlist
            const promises = data.ue_infos.map(async (ueInfo) => {
                try {
                    const imsi = ueInfo.imsi
                    const command2 = `getUeInfo(${imsi})`; 

                    // 调用异步函数获取额外的数据
                    const extraDataString = await fetchDataFromTelnet(2300, '127.0.0.4', '5gc', command2);
                    console.log("extraDataString:",extraDataString)
                    const extraData = JSON.parse(extraDataString);
                    console.log("extraData:",extraData)
                    if (extraData) {
                        ueInfo.dnnList = extraData.dnnList || [];
                        ueInfo.ueIpAddrlist = extraData.ueIpAddrlist || [];
                        ueInfo.ranNodeIp = extraData.ranNodeIp;
                    }
                } catch (err) {
                    console.error(`Error fetching extra data for IMSI ${ueInfo.imsi}:`, err);
                    ueInfo.dnnList = [];
                    ueInfo.ueIpAddrlist = [];
                }
            });

            // 等待所有额外的Telnet调用完成
            await Promise.all(promises);

            result_set = {
                total: data.total,
                ue_infos: data.ue_infos
            };
        }

        // 构建响应对象
        res.json({ result, result_set });    

    } catch (err) {
        console.error('Error fetching data from Telnet:', err);
        handleError(res, 500, 1005, 'Failed to fetch data', 'ueStatus', { desc: err.message });
    }
});
// 导出路由
module.exports = router;