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


const NFConfig = {
  // Define the route handler for reading NFConfig
  detail: async (req, res, next) => {
    try {
      const yamlData = fs.readFileSync('/home/test/nfconfig.yaml', 'utf8');
      const configData = yaml.load(yamlData);

      // 添加 schema_version 和 _id 字段到 configData,否则会引起死循环
      const jsonData = {
        schema_version: '1.0',
        _id: '123456',
        ...configData
      };

      res.status(200).json([jsonData]);//要返回数组，否则有问题
    } catch (error) {
      console.error("Error loading NFConfig:", error);
      res.status(500).json({ error: "Failed to load NFConfig data" });
    }
  },
  
  // Define the route handler for writing NFConfig
  update: (req, res, next) => {
    try {
      const newConfigData = req.body; // Assuming the request body contains the new configuration data
      const newYamlData = yaml.dump(newConfigData);
      fs.writeFileSync('/home/test/nfconfig.yaml', newYamlData, 'utf8');
      res.status(200).json({ message: "NFConfig updated successfully" });
    } catch (error) {
      console.error("Error updating NFConfig:", error);
      res.status(500).json({ error: "Failed to update NFConfig" });
    }
  }
};

// Create a custom endpoint for reading NFConfig
router.get('/NFConfig', NFConfig.detail);

// Create a custom endpoint for updating NFConfig
router.post('/NFConfig', NFConfig.update);

module.exports = router;