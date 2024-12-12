const Profile=require('../models/profile.js');
const Subscriber=require('../models/subscriber');
function fillSubscriber(data, ue,res,optype){
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
    Profile.find({"title" : data.tpl}, function(err, profile1) {
        if (err){
            fail.error.code=1603;
            fail.error.message="数据库查询失败";
            fail.error.detail.desc="数据库查询失败";
            fail.error.detail.invalidParam.param="tpl:"+data.tpl;
            fail.error.detail.invalidParam.reason="数据库查询失败"; 
            res.json(fail);
             return console.error(err);
        }
        if(profile1.length==0){
            fail.error.code=1602;
            fail.error.message="模板不存在";
            fail.error.detail.desc="模板不存在";
            fail.error.detail.invalidParam.param="tpl:"+data.tpl;
            fail.error.detail.invalidParam.reason="模板不存在"; 
            res.json(fail);
            return;
        }
        //console.log('profile1',profile1.slice);

        profile.security.amf=profile1[0].security.amf;       
        profile.ambr.downlink=profile1[0].ambr.downlink;
        profile.ambr.uplink=profile1[0].ambr.uplink;

        for(let i=0;i<profile1[0].slice.length;i++){
            
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


        for(let j=0;j<profile.slice.length;j++){
            //console.log(j,profile.slice[j]);
            ue.slice[j]=JSON.parse(JSON.stringify(profile.slice[j]));
        }

        if(optype=='modify'){
            ue.security.amf=profile.security.amf;
            ue.ambr.downlink=profile.ambr.downlink;
            ue.ambr.uplink=profile.ambr.uplink;
            ue.tpl=data.tpl;
            Subscriber.updateOne({imsi:data.imsi},ue,function(err,result){
                
                if(err){
                    console.log(err);
                    fail.error.message = "数据库操作失败";
                    fail.error.detail.desc = "数据库操作失败";
                    fail.error.detail.invalidParam.param = "imsi"+imsi;
                    fail.error.detail.invalidParam.reason = "数据库操作失败";
                    res.json(fail);
                    return;
                }else{
                    console.log('result:',result);
                    if (result.nModified === 0) {
                        // 没有找到对应的 imsi，抛出自定义错误
                        fail.error.code = 1601;
                        fail.error.message = "用户不存在";
                        fail.error.detail.desc = "用户不存在";
                        fail.error.detail.invalidParam.param = "imsi:" + data.imsi;
                        fail.error.detail.invalidParam.reason = "指定的 imsi 不存在";
                        res.json(fail);
                        return;
                    }else{
                        //console.log(result);
                        res.json({
                            "result": "OK",
                            "result_set": null
                        });
                    }
                }
            });
        }else if(optype=='addcontinue'){
            ue.security.k=data.ki;
            ue.security.op=data.op;
            ue.security.opc=data.opc;
            ue.security.amf=profile.security.amf;
            ue.ambr.downlink=profile.ambr.downlink;
            ue.ambr.uplink=profile.ambr.uplink;
            let re=1;
            for(let i=0;i<data.ueNum;i++){
                let imsi = (parseInt(data.imsiRange) + i).toString();
                let msisdn = (parseInt(data.msisdnRange) + i).toString();
                ue.imsi=imsi;
                ue.msisdn.push(msisdn);
                Subscriber.create(ue, function(err, subscriber) {
                    if (err) {
                        fail.error.message = "数据库操作失败";
                        fail.error.detail.desc = "数据库操作失败";
                        fail.error.detail.invalidParam.param = "imsi"+imsi;
                        fail.error.detail.invalidParam.reason = "数据库操作失败";
                        res.json(fail);
                        return;
                    }return console.error(err);
                    console.log('> create subscriber');
                    //console.log(subscriber);
                });
                ue.msisdn = [];
                if(re==1){
                    //连续操作，第一个成功则返回成功
                    ret=0;
                    res.json({
                        "result": "OK",
                        "result_set": null
                    });
               }
            } 
        }
    });
}
module.exports = fillSubscriber;