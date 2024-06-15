const express = require('express');
const router = express.Router();

const yaml = require('js-yaml');
const fs = require('fs');
const path = require('path');
// 指定目录路径
//const directoryPath = '/home/test/config'; // yaml配置文件的目录，修改为实际的目录路径
const configFileData = fs.readFileSync('config.json', 'utf8');
const config = JSON.parse(configFileData);

// 获取配置文件的目录路径
const directoryPath = config.directoryPath;


/*Express本身并没有针对YAML文件的专门CRUD路由处理。使用express和js-yaml模块来创建针对YAML文件的基本CRUD路由处理*/

const NFConfig = {
    detail: async (req, res, next) => {
      try {
        const nfconfigId = req.params.id;
        console.log(nfconfigId); // 打印 nfconfigId 的值到控制台
      
        const filePath = path.join(directoryPath, `${nfconfigId}.yaml`);
    
        const yamlData = fs.readFileSync(filePath, 'utf8');
        const configData = yaml.load(yamlData);
    
        const jsonData = {
          _id: nfconfigId, 
          ...configData,
        };
        res.status(200).json(jsonData);
      } catch (error) {
        console.error(`Error loading NFConfig ${desiredFileName}:`, error);
        res.status(500).json({ error: 'Failed to load NFConfig data' });
      }
    },
    
    detailAll: async (req, res, next) => {
        const allConfigData = [];
        const desiredFileNames = config.desiredFileNames; // 使用配置文件中的desiredFileNames

        try {
          // 读取目录中的所有文件
          const fileNames = fs.readdirSync(directoryPath);

          // 筛选出 YAML 文件并且只返回5GC相关的yaml文件
          const yamlFileNames = fileNames.filter((fileName) =>
            path.extname(fileName).toLowerCase() === '.yaml' && desiredFileNames.includes(fileName)
          );
          
          //测试发现,'tag:yaml.org,2002:int'可以匹配到数字和字段名。		  
          //检查数据是否以 '0' 开头并且是字符串类型，如果是，则将其转换为字符串类型	
          //否则，检查数据是否可以转换为数字类型，如果可以，则将其转换为数字类型。
          //如果都不满足，则保持原始数据不变。
          const options = {
            schema: yaml.DEFAULT_SCHEMA.extend({
              implicit: [
                new yaml.Type('tag:yaml.org,2002:int', {
                  kind: 'scalar',
                  construct: data => {
                    if (typeof data === 'string' && data.startsWith('0')) {
                      //console.log("trun to string:",data)
                      return String(data);
                    }
                    if (!isNaN(data)){
                      //console.log("trun to number:",data)
                      return Number(data);
                    }
                    return data;
                  },
                }),
              ],
            }),
          };


          for (const fileName of yamlFileNames) {
            const filePath = path.join(directoryPath, fileName);

            const yamlData = fs.readFileSync(filePath, 'utf8');
            //const updatedYamlData = yamlData.replace(/(\[[^\]]+\])/g, "'$1'");//原本想将[]表示的数组转成字符串的，后还是直接将其改成数组类型
            const configData = yaml.load(yamlData,options);

            // 提取纯文件名作为 _id（不包括目录和扩展名）
            const fileId = path.parse(fileName).name;
            let nfInsId;
            let nfName;
            let nfType;
            if (isNaN(fileId[fileId.length - 1])) //文件名结尾不是数字
            {
              nfName = fileId + '1';
              nfType = fileId;
            }
            else
            {
              nfInsId = parseInt(fileId[fileId.length - 1])+1;
              nfName = fileId.slice(0, -1) + nfInsId.toString();
              nfType = fileId.slice(0, -1);
            }

            // 添加 schema_version 和 _id 字段到 configData，避免死循环
            const jsonData = {
              schema_version: '1.0',
              _id: fileId, // 使用文件名作为 _id
              _name: nfName, // 配置显示第几个NF
              _nf: nfType, // NF的类型
              ...configData,
            };

            allConfigData.push(jsonData);
          }

          res.status(200).json(allConfigData);
        } catch (error) {
          console.error('Error loading NFConfig:', error);
          res.status(500).json({ error: 'Failed to load NFConfig data' });
        }
    },    
    
    create: (req, res, next) => {
    try {
        const newConfigData = req.body; // Assuming the request body contains the new configuration data
        const nfconfigId = req.params.id;
        console.log(nfconfigId); // 打印 nfconfigId 的值到控制台

        // 使用 nfconfigId 来确定要操作的 YAML 文件路径
        
        const yamlFilePath = path.join(directoryPath, `${nfconfigId}.yaml`);

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
  

    update: (req, res, next) => {
      try {
        const newConfigData = req.body; // 假设请求体中包含了新的配置数据
        const nfconfigId = req.params.id;
        console.log(nfconfigId); // 打印 nfconfigId 的值到控制台

        const fileName = path.join(directoryPath, `${nfconfigId}.yaml`);// 根据 nfconfigId 构造文件路径
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

       res.status(200).json(updatedConfigData);
      } catch (error) {
        console.error("Error updating NFConfig:", error);
        res.status(500).json({ error: "Failed to update NFConfig" });
      }
    }

};


router.get('/NFConfig', NFConfig.detailAll);

router.get('/NFConfig/:id', NFConfig.detail);

router.post('/NFConfig/:id', NFConfig.create);

router.post('/NFConfig', NFConfig.update);

router.patch('/NFConfig/:id', NFConfig.update);

module.exports = router;