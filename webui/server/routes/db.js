const express = require('express');
const router = express.Router();

const restify = require('express-restify-mongoose')

const yaml = require('js-yaml');
const fs = require('fs');


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




module.exports = router;