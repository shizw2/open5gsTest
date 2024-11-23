const express = require('express');
const router = express.Router();

router.get('/subscriber', (req, res) => {
    //res.send('Hello World2');
    //fetchAlerts(res);
    console.log('subscriber.........!!' );
    //res.setHeader('Content-Type', 'application/json; charset=utf-8');
    //res.send("11111");
    const { data } = req.body;
    const response =  {
        "result": "ok",
        "result_set": {
            "total": 100,
            "ueInfo": [
                {
                    "imsi": "460000123456001",
                    "msisdn": "13800138000",
                    "ki": "0123456789abcdefgh",
                    "oPType": "op",
                    "op": "0123456789abcdefgh",
                    "tpl": "1"
                }
            ]
        }
    }
    // 返回JSON响应
    res.json(response); 
});
router.put('/subscriber', (req, res) => {
    console.log('subscriber PUT.........!!' );
    // 处理 PUT 请求的逻辑
    const { data } = req.body; // 假设从请求体中获取数据
    const response = {
        "result" : "OK",
        "result_set" : null //返回接收到的数据
    };
    res.json(response); 
});
router.get('/ipManger', (req, res) => {
    //res.send('Hello World2');
    //fetchAlerts(res);
    console.log('ipManger.........!!' );
    //res.setHeader('Content-Type', 'application/json; charset=utf-8');
    //res.send("11111");
    const { data } = req.body;
    const response =  {
        "result": "ok",
        "result_set":[
            {
                "imsi": "460070123456001",
                "msisdn": "18032114312",
                "dnn" : "cmnet",
                "ip": ""
            }
        ]
    }
    // 返回JSON响应
    res.json(response); 
});
router.post('/ipManger', (req, res) => {
    console.log('ipManger POST.........!!' );
    // 处理 POST 请求的逻辑
    const { data } = req.body; // 假设从请求体中获取数据
    const response = {
        "result": "OK",
        "result_set": null //返回接收到的数据
    };
    res.json(response); 
});



// 导出路由
module.exports = router;