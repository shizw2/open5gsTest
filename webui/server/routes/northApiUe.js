const express = require('express');
const router = express.Router();
const Subscriber=require('../models/subscriber.js');
const Profile=require('../models/profile.js');
router.put('/regSub', (req, res) => {
    console.log('regSub PUT.........!!' );
    // 处理 PUT 请求的逻辑
    const data = {         
        imsiRange: req.body.imsiRange,
        msisdnRange: req.body.msisdnRange,
        ueNum: req.body.ueNum,       
        ki: req.body.ki,
        opType: req.body.opType,        
        op: req.body.op,
        opc: req.body.opc,
        tpl: req.body.tpl,
    } ;// 假设从请求体中获取数据
    Profile.find({"title" : data.tpl}, function(err, profile) {
        if (err) return console.error(err);
        console.log('profile',profile);
    });
    console.log('regSub data',data);
    
    const response = {
        "result": "OK",
        "result_set": null //返回接收到的数据
    };
    res.json(response); 
});
router.delete('/cancelSub', (req, res) => {
   
    console.log('cancelSub.........!!' );
    //res.setHeader('Content-Type', 'application/json; charset=utf-8');  
    
    const { imsiRange, ueNum } = req.body;
    console.log('cancelSub req.body',imsiRange,ueNum);
    //data.imsiRange=data.imsiRange.toString();
    for(let i=0;i<ueNum;i++){
        let imsi = (parseInt(imsiRange) + i).toString();
        Subscriber.deleteOne({"imsi":imsi}, function(err, subscriber) {
            if (err) return console.error(err);
            
        });
    }
    const response =  {
        "result": "ok",
        "result_set":null
    }
    // 返回JSON响应
    res.json(response); 
});
router.delete('/regSub', (req, res) => {
    console.log('regSub delete all.........!!' );
    // 处理 POST 请求的逻辑

    Subscriber.deleteMany({}, function(err, subscribers) {
        if (err) return console.error(err);
        console.log('> delete all  subscribers');
        
      });
    const response = {
        "result": "OK",
        "result_set": null //返回接收到的数据
    };
    res.json(response); 
});



// 导出路由
module.exports = router;