const express = require('express');
const router = express.Router();
const fetchAlerts = require('../models/fetchalerts.js');
const fetchNfstauts = require('../models/fetchnfstatus.js');
const fetchdb = require('../models/fetchdb.js');
const globledata = require('../models/globle.js'); 
const nfStatus = globledata.nfStatus;

router.get('/alarms', (req, res) => {
    //res.send('Hello World2');
    //fetchAlerts(res);
    res.setHeader('Content-Type', 'application/json; charset=utf-8');
    res.end(JSON.stringify(globledata.getcurrentAlerts()));
    
  });
router.get('/up', (req, res) => {
    res.setHeader('Content-Type', 'application/json; charset=utf-8');
    res.end(JSON.stringify(nfStatus));
    console.log('up up' );
  });
router.get('/sub', (req, res) => {
    //fetchdb.fetchommlogs(res);
    fetchdb.fetchsubscribers(res);
    //res.setHeader('Content-Type', 'application/json; charset=utf-8');
    //res.end(JSON.stringify(nfStatus));
    //console.log('up up' );
  });
router.get('/log', (req, res) => {
    fetchdb.fetchommlogs(res);    
    //res.setHeader('Content-Type', 'application/json; charset=utf-8');
    //res.end(JSON.stringify(nfStatus));
    //console.log('up up' );
  });
module.exports = router;