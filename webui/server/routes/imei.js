const express = require('express');
const router = express.Router();
const fs = require('fs');
const XLSX = require('xlsx')
const path = require('path');
const multer = require('multer');
const subscriber = require('../models/subscriber');
const profile = require('../models/profile');

//const ims = require('../ims')//环境没有IMS模块，所以暂时注释掉
const secret = process.env.JWT_SECRET_KEY || 'change-me';
const passport = require('passport');
const JWTstrategy = require('passport-jwt').Strategy;
const ExtractJWT = require('passport-jwt').ExtractJwt;
//const co = require('../ims/db.config.js'); //连接//环境没有IMS模块，所以暂时注释掉
// const mysql = require('mysql2');
const mysql = require('mysql2/promise');

passport.use(
    new JWTstrategy(
        {
            secretOrKey: secret,
            jwtFromRequest: ExtractJWT.fromAuthHeaderWithScheme('bearer')
        },
        async (token, done) => {
            try {
                return done(null, token.user);
            } catch (error) {
                done(error);
            }
        }
    )
);

// 设置 multer 存储配置
const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        cb(null, 'server/uploads/'); // 上传文件的存储目录
    },
    filename: (req, file, cb) => {
        cb(null, file.originalname); // 保持原文件名
    }
});

const upload = multer({ storage: storage });

// 默认
const dfDirectory = "../uploads/";

router.post("/importUe",async (req, res) =>{
    const fileName = req.body.fileName; 

    const outputFilePath = newDriname(dfDirectory, fileName);
    if(!fs.existsSync(outputFilePath)){
        res.status(500).send({
            message: '文件不存在，请检查！',
            data: fileName
        });
        return;
    }
    // console.log(fs.existsSync(outputFilePath),"-=====outputFilePath")

    const workbook = XLSX.readFile(outputFilePath)
    const worksheet = workbook.Sheets[workbook.SheetNames[0]]
    const headers = ["userName", "msisdn", "imsi", "temp", "k", "securityType", "securityVal", "ki", "pcTemp", "cmnetIp"];
    const data = XLSX.utils.sheet_to_json(worksheet, { header: headers, range: 1 })

    if (data && Array.isArray(data)) {
        const temps = data.map(item => item.temp);
        const profiles = await profile.find({ title: { $in: temps } });
        const profileMap = new Map(profiles.map(profile => [profile.title, profile]));
        //console.log(profileMap,"=========profileMap")

        const finalData = [];
        data.forEach(item =>{
            //console.log(item,"===================data item")
            const tempMap = profileMap.get(item.temp);
            if(!tempMap){
                res.status(500).send({
                    message: '模板不存在，请检查！',
                    profile: item.imsi+"："+item.temp
                });
                return;
            }
            const msisdn = [item.msisdn];
            const imsi = item.imsi;
            const security = tempMap.security;
            security.k = item.k;
            // securityType 0代表op，1代表opc
            if (item.securityType == 0) {
                security.op = item.securityVal;
                security.opc = '';
            } else if (item.securityType == 1) {
                security.opc = item.securityVal;
                security.op = '';
            }

            const nestedData = {
                ambr: tempMap.ambr ? tempMap.ambr : {},
                schema_version: tempMap.schema_version,
                msisdn: msisdn,
                imeisv: tempMap.imeisv ? tempMap.imeisv : [],
                mme_host: [],
                mme_realm: [],
                purge_flag: [],
                access_restriction_data: 0,
                subscriber_status: 0,
                operator_determined_barring: 0,
                network_access_mode: 0,
                subscribed_rau_tau_timer: 0,
                imsi: Number(imsi) + "",
                security: security,
                slice: tempMap.slice
            };

            finalData.push(nestedData); // 将嵌套数据推入结果数组
        });

        //const imsiData =  finalData.map(o => o.imsi);
        const existingImsi = await subscriber.find({ imsi: { $in: finalData.map(o => o.imsi) } });
        const existingImsis = existingImsi.map(o => o.imsi);
        //console.log(imsiData.length,"imsiData.length====== new Set(imsiData).size", new Set(imsiData).size)

        const resultsImsis = finalData.map(o => o.imsi);

        if (resultsImsis.length !== new Set(resultsImsis).size) {
            res.status(500).send({
                message: 'imsi重复填写，请检查！',
                existingImsis: resultsImsis
            });
            return;
        }

        const newImsis = finalData.filter(o => !existingImsis.includes(o.imsi));
        console.log(newImsis,"newImsis=======")

        if (existingImsis.length > 0) {
            res.status(500).send({
                message: 'imsi已存在，请检查！',
                existingImsis: existingImsis
            });
            return;
        }
        try {
            // 插入新数据
            if (newImsis.length > 0) {
                const imsis = await subscriber.insertMany(newImsis);
                await insertImsi(imsis, res);
            } else {
                res.status(200).send({ message: '没有新的imsi需要导入' });
            }
        } catch (error) {
            res.status(500).send(error);
        } finally {
            // 删除临时文件
            // fs.unlinkSync(req.file.path);
        }

    }
});

router.get('/getUe',async (req, res) =>{
    const startDate = req.query.startDate;
    const endDate = req.query.endDate;
    await refresh(res, startDate, endDate);
})

const toBoolean = (value) => {
    if (typeof value === 'string') {
        return value.toUpperCase() === 'TRUE';
    }
    return Boolean(value);
};

const newDriname = (dirname, filename) => {
    let finalDir = "";
    let dirName = path.join(__dirname, "/" + dirname);
    console.log(dirName, "dirName=====")
    if (!fs.existsSync(dirName)) {
        fs.mkdirSync(path.join(dirName));
        finalDir = path.join(dirName + "/" + filename)
    } else {
        finalDir = path.join(dirName + "/" + filename)
    }
    return finalDir;
}

// 导出
router.post('/exportUe', async (req, res) => {
    const useDir = req.body.result_set
    const data = await subscriber.find();

    if (!data && data.length == 0) {
        res.status(201).send({ "result": "OK", "result_set": "data is null" });
    }
    
    const tableData = [];

    data.forEach(item =>{
        const security = item.security;
        const temp = {
            "用户名": item.imsi, "电话号码": (item.msisdn && Array.isArray(item.msisdn)) ? item.msisdn[0] : '', "SUPI": item.imsi, "业务模板": '',
            "鉴权Key":(security.k)?security.k:'',"op/opc类型（0代表op，1代表opc）":(security.op)?0:1,"op/opc值":(security.op)?security.op:security.opc,
            "k4 (ki) 0不加密":'',"RC模板":'',"cmnet静态IP":'',
        };

        tableData.push(temp);
    })
 
      let wb = XLSX.utils.book_new();
      let ws= XLSX.utils.json_to_sheet(tableData);
      XLSX.utils.book_append_sheet(wb, ws, "Sheet1");
      const outputFilePath = newDriname(useDir ? useDir : dfDirectory, "5gc.subscribers.xls");
      console.log(outputFilePath);
      XLSX.writeFile(wb, outputFilePath);
      res.status(200).send({ "result": "OK", "result_set": "ue" });
})

router.get('/delUe',async (req, res) =>{
    //const imsis = await subscriber.find({ imsi: { $in: results.map(o => o.imsi) } });
    const id = req.query._id;
    await delImsiByMgId(id);
})

router.get('/upUe',async (req, res) =>{
    const id = req.query._id;
    await updateImsiByMgId(id);
})

router.use('/wxCm/v1/config', passport.authenticate('jwt', { session: false }), router);
module.exports = router;

async function delImsiByMgId(id) {
    //const imsis = await subscriber.findById(id);
    const imsis = await subscriber.findOne({"imsi":id});
    console.log(imsis,"=======id",id)
    await delImsi(imsis);
}

async function updateImsiByMgId(id) {
    // 测试
    //console.log(id,"=======id")
    const imsis = await subscriber.findById(id);

    // update `ims_hss_db`.`subscriber` set imsi=?,auc_id=?,default_apn=?,apn_list=?,msisdn=? where imsi=?
    imsis.msisdn[0] = '987654'
    imsis.slice[0].session[0].name= "xxxxxx1";
    imsis.slice[0].session[1].name= "xxxxxx2";
    imsis.security.k="aaaa";
    updateImsi(imsis);
}

const insertImsi =  async function insertImsi(imsis, res) {
    let connection;
    try {
        connection = await mysql.createConnection(co)
        await connection.beginTransaction();
        const apns = [];
        const aucs = [];
        const apnImsiMap = new Map();
        const subscriberData = [];
        const subscriberImsData = [];
        const imsiItems = [];
        if (imsis && Array.isArray(imsis)) {
            for (let o = 0; o < imsis.length; o++) {
                // add apn 数据库apn唯一索引保证再次导入唯一
                var slices = imsis[o].slice;
                if (slices && Array.isArray(slices)) {
                    const imsiApn = [];
                    for (let i = 0; i < slices.length; i++) {
                        let session = slices[i].session;
                        if (session && Array.isArray(session)) {
                            for (let j = 0; j < session.length; j++) {
                                let name = session[j].name;
                                apns.push(name);
                                imsiApn.push(name);
                            }
                        }
                    }
                    apnImsiMap.set(imsis[o].imsi, imsiApn);
                }

                // add auc
                var security = imsis[o].security;
                if (security) {
                    aucs.push({ "ki": security.k, "amf": security.amf, "sqn": 0, "opc": security.opc, "imis": imsis[o].imsi })
                }

                imsiItems.push(imsis[o].imsi);
            }

            // 修改apn-确保修改aucName存在auc-id,即修改新名字查库存在就不增加，不存在新增
            const uniqueImsiApn = [...new Set(apns)];
            console.log(uniqueImsiApn); // 输出去重后的数组
            const apnEx = await ims.findApnByApnName(uniqueImsiApn, connection);
            if (apnEx && apnEx.length > 0) {
                // 部分存在
                if (uniqueImsiApn.length != apnEx.length) {
                    // imsiApn=names => apnEx.apn
                    const apnNameEx = apnEx.map(o => o.apn);
                    //console.log("apnNameEx=======",apnNameEx,"imsiApn=============",imsiApn);

                    const newApn = [];
                    uniqueImsiApn.forEach(name => {
                        if (!apnNameEx.includes(name)) {
                            newApn.push(name);
                        }
                    })

                    ims.addApn(newApn, connection);

                }
            } else {
                ims.addApn(uniqueImsiApn, connection);
            }
            
            //ims.addApn(new Set(apns), connection);
            ims.addAuc(aucs, connection);

            // 获取auc-id
            const dataArray = Array.from(new Set(imsiItems));
            const aucResult = await ims.findAucByimsis(dataArray, connection);
            const aucMap = aucProcessResults(aucResult);

            const dataArray2 = Array.from(new Set(apns));
            const apnResult = await ims.findApnByApnName(dataArray2, connection);
            const apnMap = apnProcessResults(apnResult);
            //console.log(apnMap,"=====apnMap")
            // 获取subscriber
            for (let o = 0; o < imsis.length; o++) {

                var dfApn, msisdn;
                if (imsis[o].slice && imsis[o].slice[0].session) {
                    dfApn = imsis[o].slice[0].session[0].name;
                }

                const apnList = apnImsiMap.get(imsis[o].imsi);
                const apnIdsList = [];
                let apn_list;
                if (apnList && Array.isArray(apnList)) {
                    apnList.forEach(apnName => {
                        apnIdsList.push(apnMap.get(apnName));
                    })
                }
                if (apnIdsList && Array.isArray(apnIdsList) && apnIdsList.length > 0) {
                    apn_list = apnIdsList.join(',');
                }

                if (imsis[o].msisdn && imsis[o].msisdn[0]) {
                    msisdn = imsis[o].msisdn[0];
                }else{
                    msisdn = 0;
                }

                subscriberData.push({ "imsi": imsis[o].imsi, "auc_id": aucMap.get(imsis[o].imsi), "default_apn": apnMap.get(dfApn), "apn_list": apn_list, "msisdn": msisdn });
                console.log("subscriberData=====", subscriberData)
                var scscf, scscf_realm, scscf_peer;
                scscf = formatImsiForScscf(imsis[o].imsi, 6060);
                scscf_peer = formatImsiForPeer(imsis[o].imsi);
                scscf_realm = formatImsiForRealm(imsis[o].imsi);
                const msisdnStr = imsis[o].msisdn;
                const msisdnNumber = msisdnStr.map(item => parseInt(item, 10));// 十进制
                subscriberImsData.push({ "msisdn": msisdn, "msisdn_list": msisdnNumber, "imsi": imsis[o].imsi, "scscf": scscf, "scscf_realm": scscf_realm, "scscf_peer": scscf_peer });
            }

            ims.addSubscriber(subscriberData, connection);
            ims.addImsSubscriber(subscriberImsData, connection);
            await connection.commit();
            console.log("ok");

            if (res) { res.status(201).send({ message: 'success', imsis }); }
            return true;
        }
    } catch (error) {
        console.error('Transaction:', err);
        await connection.rollback();
        if (res) { res.status(201).send({ message: 'fail', "error": error }); }
        return false;
    } finally {
        // 释放连接
        if (connection) {
            await connection.end();
        }
    }
}

async function updateImsi(imsiData){
    let connection;
    try{
        if (imsiData) {
            console.log(imsiData,"=======imsiData")
            connection = await mysql.createConnection(co)
            await connection.beginTransaction();
            const imsi = imsiData.imsi;
            const apnName = [];
            const slices = imsiData.slice;
            var dfApn, msisdn;
            const apnImsiMap = new Map();
            const imsiApn = [];

            if (slices && Array.isArray(slices)) {
                for (let i = 0; i < slices.length; i++) {
                    let session = slices[i].session;
                    if (session && Array.isArray(session)) {
                        for (let j = 0; j < session.length; j++) {
                            let name = session[j].name;
                            apnName.push(name);
                            imsiApn.push(name);
                        }
                    }
                }

                apnImsiMap.set(imsi, imsiApn);

                if (slices[0].session) {
                    dfApn = slices[0].session[0].name;
                }
            }

            if (imsiData.msisdn && imsiData.msisdn[0]) {
                msisdn = imsiData.msisdn[0];
            }

            // 修改apn-确保修改aucName存在auc-id,即修改新名字查库存在就不增加，不存在新增
            const uniqueImsiApn = [...new Set(imsiApn)];
            console.log(uniqueImsiApn); // 输出去重后的数组
            const apnEx =await ims.findApnByApnName(uniqueImsiApn, connection);
            if (apnEx && apnEx.length > 0) {
                // 部分存在
                if (uniqueImsiApn.length != apnEx.length) {
                    // imsiApn=names => apnEx.apn
                    const apnNameEx = apnEx.map(o => o.apn);
                    //console.log("apnNameEx=======",apnNameEx,"imsiApn=============",imsiApn);

                    const newApn = [];
                    uniqueImsiApn.forEach(name =>{
                        if(!apnNameEx.includes(name)){
                            newApn.push(name);
                        }
                    })

                    ims.addApn(newApn, connection);

                }
            } else{
                ims.addApn(uniqueImsiApn,connection);
            }
           

            // 获取auc-id
            const dataArray = Array.from(new Set([imsi]));
            const aucResult = await ims.findAucByimsis(dataArray, connection);
            const aucMap = aucProcessResults(aucResult);

            const dataArray2 = Array.from(new Set(apnName));
            const apnResult = await ims.findApnByApnName(dataArray2, connection);
            const apnMap = apnProcessResults(apnResult);

            var aucId = aucMap.get(imsi);

            const apnList = apnImsiMap.get(imsi);
            const apnIdsList = [];
            let apn_list;
            if (apnList && Array.isArray(apnList)) {
                apnList.forEach(apnName => {
                    apnIdsList.push(apnMap.get(apnName));
                })
            }
            if (apnIdsList && Array.isArray(apnIdsList) && apnIdsList.length > 0) {
                apn_list = apnIdsList.join(',');
            }

            var scscf, scscf_realm, scscf_peer;
            scscf = formatImsiForScscf(imsi, 6060);
            scscf_peer = formatImsiForPeer(imsi);
            scscf_realm = formatImsiForRealm(imsi);
            const msisdnStr = imsiData.msisdn;
            const msisdnNumber = msisdnStr.map(item => parseInt(item, 10));// 十进制
            //console.log(typeof msisdnNumber, msisdnNumber ,"===========typeof=====value")
            let temp = {"imsi":imsi,"auc_id":aucId,"default_apn":apnMap.get(dfApn),"apn_list": apn_list,"msisdn": msisdn};
            ims.updateSubscriber(temp, connection);

            let ImsTemp = {"imsi":imsi,"msisdn":msisdn,"msisdn_list":JSON.stringify(msisdnNumber),"scscf":scscf,"scscf_realm":scscf_realm};
            ims.updateImsSubscriber(ImsTemp, connection);

            //aucs.push({ "ki": security.k, "amf": security.amf, "sqn": 0, "opc": security.opc, "imis": imsis[o].imsi })
            let aucTemp = {"ki": imsiData.security.k,"amf": imsiData.security.amf,"opc": imsiData.security.opc, "imsi":imsi};
            ims.updateAuc(aucTemp, connection);

            await connection.commit();
        }

    } catch(error){
        console.error('Transaction:', error);
        await connection.rollback();
    } finally {
        // 释放连接
        if (connection) {
            await connection.end();
        }
    }
}

async function delImsi(imsiData){
    //console.log(imsiData,"=====imsiData")
    let connection;
    try{
        if (imsiData) {
            connection = await mysql.createConnection(co)
            await connection.beginTransaction();
            const imsi = imsiData.imsi;
            const apnName = [];
            ims.delImsSubscriber(imsi, connection);
            ims.delSubscriber(imsi, connection);
            ims.delAuc(imsi, connection);
            
            const slices = imsiData.slice;
            if (slices && Array.isArray(slices)) {
                for (let i = 0; i < slices.length; i++) {

                    let session = slices[i].session;
                    if (session && Array.isArray(session)) {
                        for (let j = 0; j < session.length; j++) {
                            let name = session[j].name;
                            apnName.push(name);
                        }
                    }
                }
            }
            //console.log("apnName=====", apnName)
            const apnNames = Array.from(new Set(apnName));
            const apnResult = await ims.findApnByName(apnNames, connection);
            console.log(apnResult);

            if (apnResult) {
                // all-apnId
                const apnIds = apnResult.map(item => item.apn_id);

                // 外键apnId
                const existApnResult = await ims.findSubscriberByApnIds(apnIds, imsi , connection);
                const existApnIds = [];
                existApnResult.forEach(item=>{
                    const apnStr = item.apn_list;
                    const arr = apnStr.split(',');
                    arr.map(Number).forEach(item =>{
                        existApnIds.push(item);
                    })
                })

                // console.log(apnIds,"apnIds----")

                // console.log(existApnIds,"existApnIds---- 长度",existApnIds.length)

                // 删除
                const finalApnIds = apnIds.filter(item => !existApnIds.includes(item));

                // del
                if (finalApnIds && finalApnIds.length > 0) {
                    ims.delApnByid(finalApnIds, connection);
                }
               
            }

            await connection.commit();
            console.log("ok");
        }

    } catch(error){
        console.error('Transaction:', error);
        await connection.rollback();
    } finally {
        // 释放连接
        if (connection) {
            await connection.end();
        }
    }
}

function formatImsiForRealm(imsi) {
    imsi = imsi.toString();
    const mcc = imsi.slice(0, 3);
    const mnc = imsi.slice(3, 5);
    const formattedMnc = mnc.padStart(3, '0');
    const formattedUrl = `ims.mnc${formattedMnc}.mcc${mcc}.3gppnetwork.org`;
    return formattedUrl; 
}

function formatImsiForPeer(imsi) {
    imsi = imsi.toString();
    const mcc = imsi.slice(0, 3);
    const mnc = imsi.slice(3, 5);
    const formattedMnc = mnc.padStart(3, '0');
    const formattedUrl = `scscf.ims.mnc${formattedMnc}.mcc${mcc}.3gppnetwork.org`;
    return formattedUrl;
}

function formatImsiForScscf(imsi, host) {
    // 处理字符串成"sip:scscf.ims.mnc001.mcc001.3gppnetwork.org:6060" 方法传入端口号6060
    imsi = imsi.toString();
    const mcc = imsi.slice(0, 3);
    const mnc = imsi.slice(3, 5);
    const formattedMnc = mnc.padStart(3, '0');
    const formattedUrl = `sip:scscf.ims.mnc${formattedMnc}.mcc${mcc}.3gppnetwork.org:${host}`;
    return formattedUrl;
}

function aucProcessResults(result) {
    let map = new Map();
    result.forEach(item => {
        map.set(item.imsi, item.auc_id);
    });
    return map;
}

function apnProcessResults(result) {
    let map = new Map();
    result.forEach(item => {
        map.set(item.apn, item.apn_id);
    });
    return map;
}


module.exports = {
    "imei":router,
    addDbImsi: function(imsis, res){
        console.log("insertImsi")
        insertImsi(imsis, res);
    },
    updateDbImsi: function(imsis){
        console.log("updateDbImsi")
        updateImsi(imsis);
    },
    delDbImsi: function(imsis){
        console.log("delDbImsi")
        delImsi(imsis);
    }
}
