const express = require('express');
const router = express.Router();
const url = require('url');
const Subscriber=require('../models/subscriber');
const fillSubscriber = require('../models/fillsubscriber');
//签约数据查询
router.get('/subscriber', (req, res) => {
    console.log('subscriber.........!!' );
    const parsedUrl = url.parse(req.url, true);
    const imsi = parsedUrl.query.imsi;
    const page = parseInt(parsedUrl.query.page)|| 1; // 默认页码为 1
    const limit = parseInt(parsedUrl.query.limit)|| 20; // 默认限制为 20
    const skip = (page- 1) * limit; // 计算跳过的条数
    const response =  {
        "result": "OK",
        "result_set": {
            "total": 0,
            "ueInfo": [                
            ]
            /*
            "imsi": "460000123456001",
            "msisdn": "13800138000",
            "ki": "0123456789abcdefgh",
            "oPType": "op",
            "op": "0123456789abcdefgh",
            "tpl": "1"
            */
        }
    };
    let filter = { };
    if (imsi) {
        filter={"imsi" : imsi};
    };
    const option ={
        "imsi":1,
        "msisdn":1,
        "security.k":1,
        "security.opc":1,
        "security.op":1,
        "tpl":1
    };
    let fail={
        "result": "FAIL",
        "error": {
            "code": 1600,
            "target": "subscriber",
            "message": "",
            "detail": {
                "desc": "",
                "invalidParam": {
                    "param": "",
                    "reason": ""
                }
            }
        }
    }
    Subscriber.find(filter,option)
                .skip(skip)
                .limit(limit)
                .exec(function(err, subscribers){     
       if (err) {
            console.log(err);
            fail.error.message = "数据库操作失败";
            fail.error.detail.desc = "数据库操作失败";
            fail.error.detail.invalidParam.param = "imsi";
            fail.error.detail.invalidParam.reason = "数据库操作失败,请检查参数携带是否正确";
            res.status(500).json(fail);
            return console.error(err);
        };
       // 处理查询结果
       response.result_set.total = subscribers.length;
        if (subscribers) {
            subscribers.forEach(subscriber => {
                const ueInfoEntry = {
                    "imsi": subscriber.imsi,
                    "msisdn": subscriber.msisdn[0] || "",
                    "ki": subscriber.security.k,
                    "oPType": subscriber.security.op ? "op" : "opc",
                    "tpl": subscriber.tpl || ""
                };
                if (subscriber.security.op) {
                    ueInfoEntry.op = subscriber.security.op;
                }
                if (subscriber.security.opc) {
                    ueInfoEntry.opc = subscriber.security.opc;
                }
                response.result_set.ueInfo.push(ueInfoEntry); 

            });
            res.json(response); 
        }
    });
});
//签约数据修改
router.put('/subscriber', (req, res) => {
    console.log('subscriber PUT.........!!' );
    // 处理 PUT 请求的逻辑
    const data = {
        "imsi": req.body.imsi,   
        "msisdn": req.body.msisdn,    
        "ki": req.body.ki,
        "opType": req.body.opType,   
        "op": req.body.op,
        "opc": req.body.opc,
        "tpl": req.body.tpl,              
    };
    let ue = {
        imsi:"",
        msisdn: [ ],        
        security: {

        },
    
        ambr: {
        downlink:1,
        uplink:1
        },
        slice: [{
        }],
        access_restriction_data: 32,
        subscriber_status: 0,
        operator_determined_barring: 0,
        network_access_mode: 0,
        subscribed_rau_tau_timer: 12,
        tpl:data.tpl
            
    };
    ue.imsi = data.imsi;
    ue.msisdn.push(data.msisdn);
    ue.security.k = data.ki;
    if(data.opType=="op"){
        ue.security.op = data.op;
    }else{
        ue.security.opc = data.opc;
    }
    fillSubscriber(data, ue,res,'modify');
});
//获取用户的静态IP地址
router.get('/staticIp', (req, res) => {
    console.log('staticIp.........!!' );
    res.setHeader('Content-Type', 'application/json');    
    const parsedUrl = url.parse(req.url, true);
    const imsi = parsedUrl.query.imsi;
    console.log('IMSI:',imsi);
    let fail={
        "result": "FAIL",
        "error": {
            "code": 1600,
            "target": "staticIp",
            "message": "",
            "detail": {
                "desc": "",
                "invalidParam": {
                    "param": "",
                    "reason": ""
                }
            }
        }
    }
    Subscriber.find({"imsi" : imsi}, function(err, subscribers) {
        if(subscribers.length==0){
            fail.error.code = 1601;
            fail.error.message = "用户不存在";
            fail.error.detail.desc = "用户不存在";
            fail.error.detail.invalidParam.param = "imsi:"+imsi;
            fail.error.detail.invalidParam.reason = "用户不存在";
            res.status(400).json(fail);
            return;
        }
        const response =  {
            "result": "OK",
            "result_set":[
            ]
        }  
        if (err) {
            fail.error.message = "数据库操作失败";
            fail.error.detail.desc = "数据库操作失败";
            fail.error.detail.invalidParam.param = "imsi"+imsi;
            fail.error.detail.invalidParam.reason = "数据库操作失败";
            res.status(500).json(fail);
            return console.error(err);
        } 
        console.log(subscribers);
        
        let result ={
   /*        "imsi": "",
            "msisdn": "",
            "dnn" : "",
            "ip": ""
            */
        }

        
        for(let i=0;i<subscribers[0].slice.length;i++){           

            for(let j=0;j<subscribers[0].slice[i].session.length;j++){                
                result = {};
                result.imsi = subscribers[0].imsi;
                if(subscribers[0].msisdn && subscribers[0].msisdn.length>0){
                    result.msisdn = subscribers[0].msisdn[0];
                }else{
                    result.msisdn = "";
                }
                if(subscribers[0].slice[i].session[j].name){
                    result.dnn = subscribers[0].slice[i].session[j].name;
                }else{
                    result.dnn = "";
                }
                if(subscribers[0].slice[i].session[j].ue && subscribers[0].slice[i].session[j].ue.ipv4){
                    result.ip = subscribers[0].slice[i].session[j].ue.ipv4;
                }else{
                    result.ip = "";
                }
                console.log('result',result.dnn,result.ip);
                response.result_set.push(result);
                console.log(response.result_set);
            }            
        }
        // 返回JSON响应
        res.json(response); 
        
      });    

});
//静态IP修改
function usefirstIp(startIp) {
    
    let ipParts = startIp.split('.').map(Number); // 将IP地址拆分并转换为数字
    if(ipParts[3]!==0&&ipParts[3]!==255){
        ipParts[3]= ipParts[3]-1; // 增加最后一个部分
    }
    return ipParts.join('.');
};
function getValidIp(startIp) {
    
    let ipParts = startIp.split('.').map(Number); // 将IP地址拆分并转换为数字
    

   // 处理每个部分，递增IP地址
   if (ipParts[3] < 254) {
        ipParts[3]++; // 直接增加最后一个部分
    } else {
        ipParts[3] = 1; // 重置最后一个部分为1
        if (ipParts[2] < 254) {
            ipParts[2]++; // 增加第三个部分
        } else {
            ipParts[2] = 0; // 重置第三个部分
            if (ipParts[1] < 254) {
                ipParts[1]++; // 增加第二个部分
            } else {           
                return null; // 第一部分不变，返回null表示没有可用的IP
            
            }
        }
    }
    return ipParts.join('.');// 返回新的有效IP地址
};


router.post('/ipManager', (req, res) => {
    console.log('ipManager POST.........!!' );
    // 处理 POST 请求的逻辑
    const parsedUrl = url.parse(req.url, true);
    const mode = parsedUrl.query.mode;
    console.log(mode);
    let mode1 = {
        ip: req.body.ip,
    }
    let mode2 = {
        "req_data":[  ]
        }

    let fail={
        "result": "FAIL",
        "error": {
            "code": 1600,
            "target": "ipManager",
            "message": "",
            "detail": {
                "desc": "",
                "invalidParam": {
                    "param": "",
                    "reason": ""
                }
            }
        }
    }
    let ret=1;
    if(mode==="mode2"){
        for(let i=0;i<req.body.req_data.length;i++){
            let req_data={
                "imsi": "",
                "dnn": "",
                "ip": ""
            }
            req_data.imsi = req.body.req_data[i].imsi;
            req_data.ip = req.body.req_data[i].ip;
            req_data.dnn = req.body.req_data[i].dnn;
            console.log(mode2);
            Subscriber.find({"imsi" : req_data.imsi}, function(err, subscribers) {
                if(subscribers.length==0){
                    fail.error.code = 1601;
                    fail.error.message = "用户不存在";
                    fail.error.detail.desc = "用户不存在";
                    fail.error.detail.invalidParam.param = "imsi:"+req_data.imsi;
                    fail.error.detail.invalidParam.reason = "用户不存在";
                    res.status(400).json(fail);
                    ret=0;
                    return;
                }
                for(let j=0;j<subscribers[0].slice.length;j++){
                    for(let k=0;k<subscribers[0].slice[j].session.length;k++){
                        if(subscribers[0].slice[j].session[k].name==req_data.dnn){
                            Subscriber.updateOne({imsi:req_data.imsi},{"$set": {  ["slice." + j + ".session." + k + ".ue.ipv4"]:req_data.ip}},function(err,result){
                                if(err){
                                    console.log(err);
                                    fail.error.message = "数据库操作失败";
                                    fail.error.detail.desc = "数据库操作失败";
                                    fail.error.detail.invalidParam.param = "imsi"+req_data.imsi;
                                    fail.error.detail.invalidParam.reason = "数据库操作失败,请检查参数携带是否正确";
                                    res.json(fail);
                                    ret=0;
                                    return;
                                }
                                if (result.nModified === 0) {
                                    // 没有找到对应的 imsi，抛出自定义错误
                                    fail.error.code = 1601;
                                    fail.error.message = "用户不存在";
                                    fail.error.detail.desc = "用户不存在";
                                    fail.error.detail.invalidParam.param = "imsi:" + data.imsi;
                                    fail.error.detail.invalidParam.reason = "指定的 imsi 不存在";
                                    res.status(400).json(fail);
                                    ret=0;
                                    return;
                                }
                                if(ret===1){
                                    ret=0;
                                    res.json({
                                        "result": "OK",
                                        "result_set": null
                                    });
                                }
                            });
             
                        }
                    }
                }

            });

      }

    }else if(mode==="mode1"){
        let retflag=1;
        mode1.ip=usefirstIp(mode1.ip);//如果使用第一个IP地址
        Subscriber.find({}, function(err, subscribers){
            if(err){
                console.log(err);
                fail.error.message = "数据库操作失败";
                fail.error.detail.desc = "数据库操作失败";
                fail.error.detail.invalidParam.param = "mode1";
                fail.error.detail.invalidParam.reason = "数据库操作失败,请检查参数携带是否正确";
                res.status(500).json(fail);
                retflag=0;
                return;
            }
            for(let i=0;i<subscribers.length;i++){
                for(let j=0;j<subscribers[i].slice.length;j++){
                    for(let k=0;k<subscribers[i].slice[j].session.length;k++){
                        let validIp = getValidIp(mode1.ip);
                        if(validIp){                        
                            Subscriber.updateOne({imsi:subscribers[i].imsi},{"$set": {  ["slice." + j + ".session." + k + ".ue.ipv4"]:validIp}},function(err,result){
                                if(err){
                                    console.log(err);
                                    fail.error.message = "数据库操作失败";
                                    fail.error.detail.desc = "数据库操作失败";
                                    fail.error.detail.invalidParam.param = "mode1";
                                    fail.error.detail.invalidParam.reason = "数据库操作失败,请检查参数携带是否正确";
                                    res.status(500).json(fail);
                                    retflag=0;
                                    return;
                                }

                            });
                            mode1.ip=validIp;
                        }else{
                            fail.error.code = 1606;
                            fail.error.message = "静态IP地址已用完";
                            fail.error.detail.desc = "静态IP地址已用完";
                            fail.error.detail.invalidParam.param = "mode1:"+mode1.ip;
                            fail.error.detail.invalidParam.reason = "静态IP地址已用完";
                            res.status(500).json(fail);
                            retflag=0;
                            return;
                        }
                    }
                }
            }
            if(retflag===1){ 
                retflag=0;           
                res.json({
                    "result": "OK",
                    "result_set": null
                }); 
            }
        });
    
    }else{
        fail.error.code = 1607;
        fail.error.message = "携带的mode参数错误";
        fail.error.detail.desc = "携带的mode参数错误";
        fail.error.detail.invalidParam.param = "mode:"+mode;
        fail.error.detail.invalidParam.reason = "携带的mode参数错误";
        res.status(500).json(fail);
        retflag=0;
    }

        
});



// 导出路由
module.exports = router;