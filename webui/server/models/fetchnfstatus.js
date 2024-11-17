const globledata = require('./globle.js'); 
const nfStatus = globledata.nfStatus;
const http = require('http');

function fetchNfStatus(res) {
  //console.log('> Ready on fetchNfStatus');
  http.get('http://127.0.0.1:9092/api/v1/query?query=up', (response) => {
    let data = '';
    response.on('data', (chunk) => {
      data += chunk;
    }).on('error', (error) => { 
        console.log(error);
        res.send(error);
    }); 
    response.on('end', () => {    
      const parsedData = JSON.parse(data); // 解析 JSON 数据
      parsedData.data.result.forEach(result => {
          if (nfStatus[result.metric.job]) {
            if(nfStatus[result.metric.job].up!== result.value[1]){
              nfStatus[result.metric.job].up = result.value[1];
              const date = new Date(result.value[0]*1000);
              nfStatus[result.metric.job].time = date;
              console.log('> job:%s, nfStatus[result.metric.job].up:%d',result.metric.job,nfStatus[result.metric.job].up);

            }
          }
      });
      //console.log(data);
      //res.send(data);
    }).on('error', (error) => {
      console.log(error);
      res.send(error);
    });
  });
}

module.exports = fetchNfStatus;