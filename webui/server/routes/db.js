const express = require('express');
const router = express.Router();
/*一个用于生成 RESTful API 路由的库*/
/*RESTful API 路由是一种用于处理客户端请求并提供对资源进行 CRUD（创建、读取、更新、删除）操作的服务器端路由*/
/*RESTful API 路由示例,可通过 req.params.id 获取相应的参数:
GET /api/db/Profile：获取所有Profile的列表。
GET /api/db/Profile/:id：获取特定 ID 的Profile*/
const restify = require('express-restify-mongoose')

const yaml = require('js-yaml');
const fs = require('fs');
const path = require('path');
// 指定目录路径
//const directoryPath = '/home/test/config'; // yaml配置文件的目录，修改为实际的目录路径
const configFileData = fs.readFileSync('config.json', 'utf8');
const config = JSON.parse(configFileData);

// 获取配置文件的目录路径
const directoryPath = config.directoryPath;

const Subscriber = require('../models/subscriber');
restify.serve(router, Subscriber, {
  prefix: '',
  version: '',
  idProperty: 'imsi'
});

const Profile = require('../models/profile');
restify.serve(router, Profile, {
  prefix: '',
  version: ''
});

const Account = require('../models/account');
restify.serve(router, Account, {
  prefix: '',
  version: '',
  idProperty: 'username'
});

const Ommlog = require('../models/ommlog');
restify.serve(router, Ommlog, {
  prefix: '',
  version: ''
});

const Imeicheck = require('../models/imeicheck');
restify.serve(router, Imeicheck, {
  prefix: '',
  version: '',
  idProperty: 'imei'
});
module.exports = router;