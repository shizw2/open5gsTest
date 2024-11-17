process.env.DB_URI = process.env.DB_URI || 'mongodb://127.0.0.1/5gc';

//const _hostname = process.env.HOSTNAME || '0.0.0.0';
const _hostname = '::';
const port = process.env.PORT || 3000;
const fs = require('fs');
const morgan = require('morgan');
const co = require('co');
const next = require('next');

const dev = process.env.NODE_ENV !== 'production';/* 开发模式还是生产模式 */
const app = next({ dev });
const handle = app.getRequestHandler();

const express = require('express');
const bodyParser = require('body-parser');
const methodOverride = require('method-override');

const session = require('express-session');

const mongoose = require('mongoose');
const MongoStore = require('connect-mongo');

const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;

const csrf = require('lusca').csrf();
const secret = process.env.SECRET_KEY || 'change-me';

const api = require('./routes');
const northApi = require('./routes/northApi'); // 引入北向接口的路由处理器
const fetchAlerts = require('./models/fetchalerts.js');
const fetchNfStatus = require('./models/fetchnfstatus.js');
const Account = require('./models/account.js');
const path = require('path');
const logDirectory = path.join(__dirname, 'logs');
fs.mkdirSync(logDirectory, { recursive: true });
// 创建一个可写流，将日志写入到文件
const accessLogStream = fs.createWriteStream(path.join(logDirectory, 'web.log'), { flags: 'a' });
function checkLogFileSize() {
  const stats = fs.statSync(path.join(logDirectory, 'web.log'));
  const fileSizeInBytes = stats.size;
  const fileSizeInMegabytes = fileSizeInBytes / (1024*1024);
  console.log('> check log size ....'+fileSizeInMegabytes);
  if (fileSizeInMegabytes > 10) {
    fs.truncateSync(path.join(logDirectory, 'web.log')); // 清空文件内容
  }
}
const intervalInMilliseconds = 24*60*60*1000; 
co(function* () {
  yield app.prepare();
  console.log("process.env.DB_URI:"+process.env.DB_URI+",process.env.HOSTNAME:"+process.env.HOSTNAME)
  mongoose.Promise = global.Promise;
  if (dev) {
    mongoose.set('debug', true);
  }
  const db = yield mongoose.connect(process.env.DB_URI, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
    serverSelectionTimeoutMS: 1000
    /* other options */
  })

  /*在开发模式下检查数据库中是否存在管理员账户，并在没有任何账户时创建一个默认的管理员账户*/
  //rel版本也进行数据库检查(后续可在安装脚本中通过mongo脚本添加管理员账户)
  //if (dev) {
    Account.count((err, count) => {
      if (err) {
        console.error(err);
        throw err;
      }

      if (!count) {
        const newAccount = new Account();
        newAccount.username = 'admin';
        newAccount.roles = [ 'admin' ];
        Account.register(newAccount, '1423', err => {
          if (err) {
            console.error(err);
            throw err;
          }
        })
      }
    })
  //}

  const server = express();
  //server.disable('DELETE');
  //server.disable('PUT');
  server.disable('HEAD');
  server.disable('OPTIONS');
  server.use(bodyParser.json());
  server.use(bodyParser.urlencoded({ extended: true }));
  server.use(methodOverride());
  server.use(morgan('combined', {stream: accessLogStream })); 
  checkLogFileSize();
  setInterval(checkLogFileSize, intervalInMilliseconds); 
  // 定时执行 fetchAlerts 函数
  setInterval(() => {
    fetchNfStatus();
    fetchAlerts(); // 调用 fetchAlerts 函数
  }, 3000); // 每 3000 毫秒（3 秒）执行一次
  server.use(session({
    secret: secret,
    store: MongoStore.create({
      mongoUrl: process.env.DB_URI,
      ttl: 60 * 60 * 24 * 7 * 2
    }),
    resave: false,
    rolling: true,
    saveUninitialized: true,
    httpOnly: true,
    cookie: {
      maxAge: 1000 * 60 * 60 * 24 * 7 * 2  // 2 weeks
    }
  }));
  server.disable('x-powered-by');
  server.use((req, res, next) => {
    if ( req.method === 'HEAD' || req.method === 'OPTIONS') {
      res.status(405).send('Method Not Allowed');
    } else {
      req.db = db;
      next();
    }
    //req.db = db; /*将 db 对象赋值给了 req.db。这样，在后续的请求处理中，您可以通过 req.db 访问到数据库实例。*/
    //next();
  })
  const csrfIgnore = (req, res, next) => {
    //console.log('req.path====',req.path);
    if (req.path.startsWith('/coreNetwork/')) {
      //console.log('eeeeeeeeee',req.path);
        return next();
    }
    return csrf(req, res, next);
  };
  server.use(csrfIgnore);
  //server.use(csrf);/*CSRF（Cross-Site Request Forgery）防护中间件将在每个请求中自动检查 CSRF 令牌的有效性。如果请求中缺少或无效的令牌，它将阻止请求，并返回错误或采取适当的防护措施*/

 
  server.use(passport.initialize());
  server.use(passport.session());

  passport.use(new LocalStrategy(Account.authenticate()));
  passport.serializeUser(Account.serializeUser());
  passport.deserializeUser(Account.deserializeUser());

  server.use('/api', api);
  server.use('/coreNetwork', northApi);
  server.use(express.static('static'));//必须要放到server.get('*', (req, res)前
  
  server.get('*', (req, res) => {
    return handle(req, res);
  });
  server.listen(port, _hostname, err => {
    if (err) throw err;
    console.log('> Ready on http://' + _hostname + ':' + port);
  });

})
.catch(error => console.error(error.stack));
