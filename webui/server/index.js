process.env.DB_URI = process.env.DB_URI || 'mongodb://127.0.0.1/5gc';

const _hostname = process.env.HOSTNAME || '0.0.0.0';
const port = process.env.PORT || 3000;

const co = require('co');
const next = require('next');

const dev = process.env.NODE_ENV !== 'production';/* 开发模式还是生产模式 */
const app = next({ dev });
const handle = app.getRequestHandler();

const express = require('express');
const bodyParser = require('body-parser');
const methodOverride = require('method-override');
const morgan = require('morgan');
const session = require('express-session');

const mongoose = require('mongoose');
const MongoStore = require('connect-mongo');

const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;

const csrf = require('lusca').csrf();
const secret = process.env.SECRET_KEY || 'change-me';

const api = require('./routes');

const Account = require('./models/account.js');

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
  
  server.use(bodyParser.json());
  server.use(bodyParser.urlencoded({ extended: true }));
  server.use(methodOverride());

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

  server.use((req, res, next) => {
    req.db = db; /*将 db 对象赋值给了 req.db。这样，在后续的请求处理中，您可以通过 req.db 访问到数据库实例。*/
    next();
  })

  server.use(csrf);/*CSRF（Cross-Site Request Forgery）防护中间件将在每个请求中自动检查 CSRF 令牌的有效性。如果请求中缺少或无效的令牌，它将阻止请求，并返回错误或采取适当的防护措施*/

  server.use(passport.initialize());
  server.use(passport.session());

  passport.use(new LocalStrategy(Account.authenticate()));
  passport.serializeUser(Account.serializeUser());
  passport.deserializeUser(Account.deserializeUser());

  server.use('/api', api);
  server.use(express.static('static'));//必须要放到server.get('*', (req, res)前
  
  server.get('*', (req, res) => {
    return handle(req, res);
  });

  if (dev) {
    server.use(morgan('tiny'));/*morgan 是一个流行的日志记录中间件，它可以用于记录 HTTP 请求的相关信息，如请求路径、响应状态码、响应时间等,'tiny' 是格式*/
  }

  server.listen(port, _hostname, err => {
    if (err) throw err;
    console.log('> Ready on http://' + _hostname + ':' + port);
  });
})
.catch(error => console.error(error.stack));
