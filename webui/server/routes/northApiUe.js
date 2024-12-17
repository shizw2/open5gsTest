const express = require('express');
const router = express.Router();
const url = require('url');
const Subscriber=require('../models/subscriber.js');
const Profile=require('../models/profile.js');
//const { legacy_createStore } = require('redux');
const fillSubscriber = require('../models/fillsubscriber');
//连续开户
const { handleError, errorCodes } = require('./northApiCommon');
router.put('/regSub', (req, res) => {
    console.log('regSub PUT.........!!' );
    // 处理 PUT 请求的逻辑

    const data = {         
        imsiRange: req.body.imsiRange,
        msisdnRange: req.body.msisdnRange,
        ueNum: req.body.ueNum,       
        ki: req.body.ki,
        opType: req.body.opType,        
        op: req.body.op|| "",
        opc: req.body.opc|| "",
        tpl: req.body.tpl,
    } ;// 假设从请求体中获取数据
    //注释掉的代码不要删，方便检查属性的赋值
    let ue = {
       imsi:"",
        msisdn: [ ],
    
        security: {
   /*       k:"",
          op:"",
          opc:"",
          amf:"",
          rand:"",
          sqn:""*/
        },
      
        ambr: {
          downlink:1,
          uplink:1
        },
        slice: [{
        }],
   /*      
        slice: [{
          sst:1,
          sd:"",
          default_indicator:true,
          session: [{
            name:"", // DNN or APN
            type:0, // 0: IPv4, 1: IPv6, 2: IPv4v6
            qos: {
              index:1, // 5QI or QCI
              arp: {
                priority_level:1,
                pre_emption_capability:1,
                pre_emption_vulnerability:1,
              }
            },
            ambr: {
              downlink:1,
              uplink:1
            },
            ipv4_framed_routes: [  ],
            ipv6_framed_routes: [  ],
            ue: {
              ipv4: "",
              ipv6: ""
            },
            smf: {
              ipv4: "",
              ipv6: ""
            },
            pcc_rule: [{
              flow: [{
                direction: 0,
                description: ""
              }],
              qos: {
                index: 9, // 5QI or QCI
                arp: {
                  priority_level: 9,
                  pre_emption_capability: 1,
                  pre_emption_vulnerability: 1,
                },
                mbr: {
                  downlink: { value: 1, unit: 2 },
                  uplink: { value: 1, unit: 2 }
                },
                gbr: {
                  downlink: { value: 1, unit: 2 },
                  uplink: { value: 1, unit: 2 }
                },
              },
            }]
          }]
        }],
     */ 
        access_restriction_data: 32,
        subscriber_status: 0,
        operator_determined_barring: 0,
        network_access_mode: 0,
        subscribed_rau_tau_timer: 12,
        tpl:data.tpl
        
    };
    //注释掉的代码不要删，方便检查属性的赋值
    /*let  profile = {
        title: "",

        msisdn: [  ],
        imeisv: [  ],
      
        security: {
          k: "",
          op: "",
          opc: "",
          amf: "",
        },
      
        ambr: {
          downlink: { value: 1, unit: 2 },
          uplink: { value: 1, unit: 2 }
        },
      
        slice: [{
          sst: 1,
          sd: "",
          default_indicator: false,
          session: [{
            name: "", // DNN or APN
            type: 1,
            qos: {
              index: 9, // 5QI or QCI
              arp: {
                priority_level: 1,
                pre_emption_capability: 1,
                pre_emption_vulnerability: 1,
              }
            },
            ambr: {
              downlink: { value: 1, unit: 3 },
              uplink: { value: 1, unit: 3 }
            },
            ue: {
              ipv4: "",
              ipv6: ""
            },
            smf: {
              ipv4: "",
              ipv6: ""
            },
            pcc_rule: [{
              flow: [{
                direction: 1,
                description: "Downlink"
              }],
              qos: {
                index: 9,
                arp: {
                  priority_level: 2,
                  pre_emption_capability: 2,
                  pre_emption_vulnerability: 2,
                },
                mbr: {
                  downlink: { value: 1, unit: 1 },
                  uplink: { value: 1, unit: 1 }
                },
                gbr: {
                  downlink: { value:1, unit: 1 },
                  uplink: { value: 1, unit: 1 }
                },
              },
            }]
          }]
        }]
    };
 
    let  profile = {
        title: "",

        msisdn: [  ],
        imeisv: [  ],
      
        security: {
  
        },
      
        ambr: {
          downlink: { },
          uplink: {  }
        },
      
        slice: [ ]
    };
  
    Profile.find({"title" : data.tpl}, function(err, profile1) {
        if (err) return console.error(err);
        console.log('profile1',profile1.slice);
        profile.title=profile1[0].title;
        profile.msisdn=profile1[0].msisdn; 
        console.log('kkkkkk',profile1[0].security);        
        profile.security.k=profile1[0].security.k;

        profile.security.amf=profile1[0].security.amf;       
        profile.ambr.downlink=profile1[0].ambr.downlink;
        profile.ambr.uplink=profile1[0].ambr.uplink;

        for(let i=0;i<profile1[0].slice.length;i++){
            console.log('i',i);
            let slice = {
                sst: 1,
                sd: "",
                default_indicator: false,
                session: []
            };
            slice.sst=profile1[0].slice[i].sst;
            slice.sd=profile1[0].slice[i].sd ;
            slice.default_indicator=profile1[0].slice[i].default_indicator;   
           
            for(let j=0;j<profile1[0].slice[i].session.length;j++){
                //profile.slice[i].session[j]=JSON.parse(JSON.stringify(profilenull.slice[i].session[0]));
                let session = {
                    name: "", // DNN or APN
                    type: 1,
                    qos: {
                        index: 9, // 5QI or QCI
                        arp: {
                            priority_level: 2,
                            pre_emption_capability: 2,
                            pre_emption_vulnerability: 2,
                        }
                    },
                    ambr: {
                        downlink: { },
                        uplink: {  }
                    },
                    ue: {
        
                    },
                    smf: {
        
                    },
                    pcc_rule: []
                      
                };
            
                session.name=profile1[0].slice[i].session[j].name;
                session.type=profile1[0].slice[i].session[j].type;
                session.qos.index=profile1[0].slice[i].session[j].qos.index;
                session.qos.arp.priority_level=profile1[0].slice[i].session[j].qos.arp.priority_level;
                session.qos.arp.pre_emption_capability=profile1[0].slice[i].session[j].qos.arp.pre_emption_capability;
                session.qos.arp.pre_emption_vulnerability=profile1[0].slice[i].session[j].qos.arp.pre_emption_vulnerability;
                session.ambr.downlink=profile1[0].slice[i].session[j].ambr.downlink;
                session.ambr.uplink=profile1[0].slice[i].session[j].ambr.uplink;
                session.ue.ipv4=profile1[0].slice[i].session[j].ue.ipv4;
                session.ue.ipv6=profile1[0].slice[i].session[j].ue.ipv6;
                session.smf.ipv4=profile1[0].slice[i].session[j].smf.ipv4;
                session.smf.ipv6=profile1[0].slice[i].session[j].smf.ipv6;             

                for(let k=0;k<profile1[0].slice[i].session[j].pcc_rule.length;k++){
                    let pcc_rule={
        
                        flow: [],
                        qos: {
                        index: 9, // 5QI or QCI
                        arp: {
            
                        },
                        mbr: {
                            downlink: {  },
                            uplink: {  }
                        },
                        gbr: {
                            downlink: {},
                            uplink: {  }
                        },
                        },           
            
                    }; 
                   
                    for(let l=0;l<profile1[0].slice[i].session[j].pcc_rule[k].flow.length;l++){
                        let flow = {
                            direction: 1,
                            description: "Downlink"
                          };
                          flow.direction=profile1[0].slice[i].session[j].pcc_rule[k].flow[l].direction; 
                          flow.description=profile1[0].slice[i].session[j].pcc_rule[k].flow[l].description;
                          pcc_rule.flow.push(flow);       
                    }    
                    pcc_rule.qos.index=profile1[0].slice[i].session[j].pcc_rule[k].qos.index;
                    pcc_rule.qos.arp.priority_level=profile1[0].slice[i].session[j].pcc_rule[k].qos.arp.priority_level;
                    pcc_rule.qos.arp.pre_emption_capability=profile1[0].slice[i].session[j].pcc_rule[k].qos.arp.pre_emption_capability;
                    pcc_rule.qos.arp.pre_emption_vulnerability=profile1[0].slice[i].session[j].pcc_rule[k].qos.arp.pre_emption_vulnerability;              
                    pcc_rule.qos.mbr.downlink.value=profile1[0].slice[i].session[j].pcc_rule[k].qos.mbr.downlink.value;
                    pcc_rule.qos.mbr.downlink.unit=profile1[0].slice[i].session[j].pcc_rule[k].qos.mbr.downlink.unit;
                    pcc_rule.qos.mbr.uplink.value=profile1[0].slice[i].session[j].pcc_rule[k].qos.mbr.uplink.value;
                    pcc_rule.qos.mbr.uplink.unit=profile1[0].slice[i].session[j].pcc_rule[k].qos.mbr.uplink.unit;
                    pcc_rule.qos.gbr.downlink.value=profile1[0].slice[i].session[j].pcc_rule[k].qos.gbr.downlink.value;
                    pcc_rule.qos.gbr.downlink.unit=profile1[0].slice[i].session[j].pcc_rule[k].qos.gbr.downlink.unit;
                    pcc_rule.qos.gbr.uplink.value=profile1[0].slice[i].session[j].pcc_rule[k].qos.gbr.uplink.value;
                    pcc_rule.qos.gbr.uplink.unit=profile1[0].slice[i].session[j].pcc_rule[k].qos.gbr.uplink.unit;
                    
                    session.pcc_rule.push(pcc_rule);
                }
                slice.session.push(session);
            }
            profile.slice.push(slice);
       }
        
        for(let i=0;i<data.ueNum;i++){
            let imsi = (parseInt(data.imsiRange) + i).toString();
            let msisdn = (parseInt(data.msisdnRange) + i).toString();
            ue.imsi=imsi;
            ue.msisdn.push(msisdn);
            ue.security.k=data.ki;
            ue.security.op=data.op;
            ue.security.opc=data.opc;
            ue.security.amf=profile.security.amf;
            ue.ambr.downlink=profile.ambr.downlink;
            ue.ambr.uplink=profile.ambr.uplink;
            for(let j=0;j<profile.slice.length;j++){
                console.log(j,profile.slice[j]);
                ue.slice[j]=JSON.parse(JSON.stringify(profile.slice[j]));
            }
            Subscriber.create(ue, function(err, subscriber) {
                if (err) return console.error(err);
                console.log('> create subscriber');
                console.log(subscriber);
            });
            ue.msisdn = [];
        } 

        
    });
    */
    fillSubscriber(data, ue,res,'addcontinue');    

});
//连续销户
router.delete('/cancelSub', (req, res) => {
   
    console.log('cancelSub.........!!' );
    //res.setHeader('Content-Type', 'application/json; charset=utf-8');  
    let fail={
        "result": "FAIL",
        "error": {
            "code": 1600,
            "target": "wxCmAgent",
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
   // const { imsiRange, ueNum } = req.body;
    const parsedUrl = url.parse(req.url, true);
    const imsiRange = parsedUrl.query.imsiRange;
    const ueNum = parsedUrl.query.ueNum;
    console.log('cancelSub req.body',imsiRange,ueNum);
    let ret=1;
    for(let i=0;i<ueNum;i++){
        let imsi = (parseInt(imsiRange) + i).toString();
        Subscriber.deleteOne({"imsi":imsi}, function(err, result) {
            if (err){
                fail.error.message = "数据库操作失败";
                fail.error.detail.desc = "数据库操作失败";
                fail.error.detail.invalidParam.param = "imsi"+imsi;
                fail.error.detail.invalidParam.reason = "数据库操作失败";
                ret=0;
                //res.status(500).json(fail);
                handleError(res, 500, 1600, '数据库操作失败', 'cancelSub', { desc: '数据库操作失败'});
                return console.error(err);
            } 
            /*//不做判断，无论是否存在都执行删除操作
            if (result.deletedCount == 0) {
                fail.error.code = 1601;
                fail.error.message = "IMSI不存在";
                fail.error.detail.desc = "IMSI不存在";
                fail.error.detail.invalidParam.param = "imsi"+imsi;
                fail.error.detail.invalidParam.reason = "IMSI不存在";
                ret=0;
                res.json(fail);
                return console.error('IMSI不存在');
            }
            */
        });
        if(ret===1){
            ret=0;
            res.json({
                "result": "OK",
                "result_set": null
            });
        }
    }
    if(ret===1){
      fail.error.message = "携带的用户数量不对";
      fail.error.detail.desc = "数据库操作失败";
      fail.error.detail.invalidParam.param = "ueNum"+ueNum;
      fail.error.detail.invalidParam.reason = "携带参数不对";    
      res.status(400).json(fail);
    }

});
//全量销户
router.delete('/regSub', (req, res) => {
    console.log('regSub delete all.........!!' );   

    Subscriber.deleteMany({}, function(err, result) {
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