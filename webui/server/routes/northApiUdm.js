const express = require('express');
const router = express.Router();

router.get('/subscriber', (req, res) => {
    //res.send('Hello World2');
    //fetchAlerts(res);
    console.log('subscriber.........!!' );
    //res.setHeader('Content-Type', 'application/json; charset=utf-8');
    //res.send("11111");
    const response = {
        result: "OK",
        result_set: true
    };
    
    // 返回JSON响应
    res.json(response); 
});
router.post('/subscriber', (req, res) => {
    console.log('subscriber POST.........!!' );
    // 处理 POST 请求的逻辑
    const { data } = req.body; // 假设从请求体中获取数据
    const response = {
        result: "OK",
        result_set: null //返回接收到的数据
    };
    res.json(response); 
});

router.put('/subscriber', (req, res) => {
    console.log('subscriber PUT.........!!' );
    // 处理 PUT 请求的逻辑
    const { data } = req.body; // 假设从请求体中获取数据
    const response = {
        result: "OK",
        result_set: null //返回接收到的数据
    };
    res.json(response); 
});

// 导出路由
module.exports = router;