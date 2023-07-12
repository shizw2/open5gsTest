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
    
    detailAll: async (req, res, next) => {
      try {
        const fileNames = [
          '/home/test/nfconfig1.yaml',
          '/home/test/nfconfig2.yaml',
          '/home/test/nfconfig3.yaml',
          '/home/test/nfconfig4.yaml',
          '/home/test/nfconfig5.yaml',
          '/home/test/nfconfig6.yaml',
        ];

        const allConfigData = [];

        for (const fileName of fileNames) {
          const yamlData = fs.readFileSync(fileName, 'utf8');
          const configData = yaml.load(yamlData);


          // 提取纯文件名作为 _id（不包括目录）
          const fileId = require('path').parse(fileName).name;
          
          // 添加 schema_version 和 _id 字段到 configData,否则会引起死循环
          const jsonData = {
            schema_version: '1.0',
            _id: fileId, // 使用文件名作为 _id
            ...configData
          };

          allConfigData.push(jsonData);
        }

        res.status(200).json(allConfigData);
      } catch (error) {
        console.error("Error loading NFConfig:", error);
        res.status(500).json({ error: "Failed to load NFConfig data" });
      }
    },    
    
    create: (req, res, next) => {
    try {
        const newConfigData = req.body; // Assuming the request body contains the new configuration data
        const nfconfigId = req.params.id;
        console.log(nfconfigId); // 打印 nfconfigId 的值到控制台

        // 使用 nfconfigId 来确定要操作的 YAML 文件路径
        const yamlFilePath = `/home/test/${nfconfigId}.yaml`;

        // Convert the configuration data to YAML format
        const newYamlData = yaml.dump(newConfigData);

        // Write the YAML data to the file
        fs.writeFileSync(yamlFilePath, newYamlData, 'utf8');

        res.status(201).json({ message: "NFConfig created successfully" });
      } catch (error) {
        console.error("Error creating NFConfig:", error);
        res.status(500).json({ error: "Failed to create NFConfig" });
      }
    },
  
    // Define the route handler for updating NFConfig
    update: (req, res, next) => {
      try {
        const newConfigData = req.body; // 假设请求体中包含了新的配置数据
        const nfconfigId = req.params.id;
        console.log(nfconfigId); // 打印 nfconfigId 的值到控制台

        const fileName = `/home/test/${nfconfigId}.yaml`; // 根据 nfconfigId 构造文件路径
        const yamlData = fs.readFileSync(fileName, 'utf8'); // 读取 YAML 文件的内容
        const configData = yaml.load(yamlData); // 解析 YAML 数据为 JavaScript 对象

        // 将新数据合并到现有数据中
        const updatedConfigData = {
          ...configData,
          ...newConfigData,
        };

        // 将合并后的数据转换回 YAML 格式
        const updatedYamlData = yaml.dump(updatedConfigData);

        // 将更新后的 YAML 数据写入文件
        fs.writeFileSync(fileName, updatedYamlData, 'utf8');

        res.status(200).json({ message: "NFConfig updated successfully" });
      } catch (error) {
        console.error("Error updating NFConfig:", error);
        res.status(500).json({ error: "Failed to update NFConfig" });
      }
    }

};

// Create a custom endpoint for reading NFConfig
router.get('/NFConfig', NFConfig.detailAll);

router.post('/NFConfig/:id', NFConfig.create);

// Create a custom endpoint for updating NFConfig
router.post('/NFConfig', NFConfig.update);

// Add a custom endpoint for patching NFConfig
//router.patch('/NFConfig', NFConfig.update);
router.patch('/NFConfig/:id', NFConfig.update);

module.exports = router;