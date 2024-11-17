const http = require('http');
const globledata = require('./globle.js'); 

function compareAlerts(lalerts, parsedDataAlerts) {
    const onlyInLalerts = [];        // lalerts中存在但parsedDataAlerts中不存在的对象
    onlyInParsedDataAlerts = []; // parsedDataAlerts中存在但lalerts中不存在的对象
    

    if(lalerts.length===0)
        onlyInParsedDataAlerts=parsedDataAlerts;
    // 遍历 lalerts，将不在 parsedDataAlerts 中的项添加到 onlyInLalerts
    lalerts.forEach(alert => {
       let flag=1;
        parsedDataAlerts.forEach(parsedDataAlert => {
            if(parsedDataAlert.labels.alertname===alert.labels.alertname && parsedDataAlert.labels.level===alert.labels.level &&parsedDataAlert.activeAt===alert.activeAt){
                flag=0;                
            }
        });
        if(flag)
            onlyInLalerts.push(alert);
    });

    // 遍历 parsedDataAlerts，将不在 lalerts 中的项添加到 onlyInParsedDataAlerts
    parsedDataAlerts.forEach(parsedDataAlert => {
        let flag1=1;
        lalerts.forEach(alert => {
            if(alert.labels.alertname===parsedDataAlert.labels.alertname && alert.labels.level===parsedDataAlert.labels.level &&alert.activeAt===parsedDataAlert.activeAt){
                flag1=0;
               // console.log('name>>>>',alert.labels.alertname,parsedDataAlert.labels.alertname);
               // console.log('time>>>>',alert.activeAt,parsedDataAlert.activeAt);
            }       
        });
        if(flag1)
            onlyInParsedDataAlerts.push(parsedDataAlert);
    });
    
    // 返回包含两个对象的结果
    return {
        onlyInLalerts,
        onlyInParsedDataAlerts
    };
}
function fetchAlerts(res) {
  //console.log('> Ready on http://Alarm');
  http.get('http://127.0.0.1:9092/api/v1/alerts', (response) => {
    let data = '';
    response.on('data', (chunk) => {
      data += chunk;
    }).on('error', (error) => { 
        console.log(error);
        res.send(error);
    }); // 处理错误     ;

    response.on('end', () => {
        const parsedData = JSON.parse(data); // 解析 JSON 数据

        const { onlyInLalerts, onlyInParsedDataAlerts } = compareAlerts(globledata.getAlerts(), parsedData.data.alerts);  
        if(onlyInLalerts.length>0)
            console.log('已恢复的告警:', onlyInLalerts);//已恢复的告警
        if(onlyInParsedDataAlerts.length>0)
            console.log('新增的告警:', onlyInParsedDataAlerts);//新增的告警

        globledata.clearAlerts();
        globledata.clearcurrentAlerts();
        parsedData.data.alerts.forEach(alert => {
            globledata.addAlert(alert);   
            let singlealert = {
                "name" : "",
                "id" : "",
                "type" : "",
                "level": "",
                "location": "",
                "description": "",
                "time": "",
            };
            let currentalerts = [];
            singlealert.name=alert.labels.alertname;
            singlealert.id=alert.labels.id;
            singlealert.type=alert.labels.type;
            singlealert.level=alert.labels.level;
            singlealert.location=alert.labels.instance;
            singlealert.description=alert.annotations.description;
            singlealert.time=alert.activeAt;
            currentalerts.push(singlealert);//currentalerts临时的暂没有使用
            globledata.addcurrentAlert(singlealert);

        });
        
        
    }).on('error', (error) => {
      console.log(error);
      res.send(error);
    });
  });
}

module.exports = fetchAlerts;