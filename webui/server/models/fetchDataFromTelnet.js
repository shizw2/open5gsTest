const net = require('net');
const { promisify } = require('util');

// 创建一个新的函数，该函数返回一个已经连接的 socket 的 Promise
function createConnectedSocket(port, host) {
  return new Promise((resolve, reject) => {
    const client = new net.Socket();
    client.connect(port, host, () => {
      resolve(client);
    });
    client.on('error', (err) => {
      reject(err);
    });
  });
}

/**
 * 通过 Telnet 从远程服务器获取数据
 * @param {number} port - Telnet 服务器的端口号
 * @param {string} host - Telnet 服务器的主机名或 IP 地址
 * @param {string} password - 用于 Telnet 服务器认证的密码
 * @param {string} command - 要发送给 Telnet 服务器的命令
 * @returns {Promise<string>} - 从服务器接收到的数据或错误
 */
async function fetchDataFromTelnet(port, host, password, command) {
  const timeout = 5000; // 设置超时时间为5秒
  const timeoutPromise = new Promise((_, reject) => {
    setTimeout(() => reject(new Error('Connection timed out')), timeout);
  });

  try {
    // 连接到 Telnet 服务器
    const client = await Promise.race([createConnectedSocket(port, host), timeoutPromise]);
    console.log('Connected to server successfully.');

    let allData = ''; // 用于存储所有接收到的数据
    // 监听数据
    return new Promise((resolve, reject) => {
      let cmdSent = false;
      client.on('data', (data) => {
        dataStr = data.toString();
        console.log('Received:', dataStr);
        dataStr = dataStr.replace(/[^\x20-\x7E]+/g, ''); // 移除所有非打印字符
        console.log('cleaned data:', dataStr);

        if (dataStr.includes('Password:')) {
          console.log('Sending password...');
          client.write(password + '\n');         
        } else if (dataStr.includes('aaa>') && cmdSent == false) {
          console.log('Sending command:',command);
          client.write(command + '\n');
          cmdSent = true;
        } else if (dataStr.includes('aaa>')) { 
            const index = dataStr.indexOf('>aaa>');
            console.log('slice index:', index);
            console.log('slice data:', dataStr.slice(0, index));
            allData += dataStr.slice(0, index); // 截断 'aaa>' 及其之后的数据
            console.log('all data:', allData);
            client.end();
            resolve(allData);
        } else {
            console.log('Received data:', dataStr);
            allData += dataStr;
        }
      });

      client.on('error', (err) => {
        console.error('Error with telnet:', err);
        reject(err);
        client.end();
      });

      client.on('close', () => {
        console.log('Connection closed.');
      });
    });
  } catch (err) {
    throw err;
  }
}

// 导出 fetchDataFromTelnet 函数
module.exports = fetchDataFromTelnet;