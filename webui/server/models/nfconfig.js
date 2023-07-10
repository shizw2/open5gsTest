const fs = require('fs');
const yaml = require('js-yaml');

const NFConfig = {
  schema_version: {
    type: 'number', // 使用普通的 JavaScript 类型
    default: 1 // Current Schema Version
  },
  title: { type: 'string', required: true },
  msisdn: ['string'],
  imeisv: ['string'],
  security: {
    k: 'string',
    op: 'string',
    opc: 'string',
    amf: 'string'
  },
  ambr: {
    downlink: { value: 'number', unit: 'number' },
    uplink: { value: 'number', unit: 'number' }
  },
  slice: [
    {
      sst: { type: 'number', required: true },
      sd: 'string',
      default_indicator: 'boolean',
      session: [
        {
          name: { type: 'string', required: true }, // DNN or APN
          type: 'number',
          qos: {
            index: 'number', // 5QI or QCI
            arp: {
              priority_level: 'number',
              pre_emption_capability: 'number',
              pre_emption_vulnerability: 'number'
            }
          },
          ambr: {
            downlink: { value: 'number', unit: 'number' },
            uplink: { value: 'number', unit: 'number' }
          },
          ue: {
            addr: 'string',
            addr6: 'string'
          },
          smf: {
            addr: 'string',
            addr6: 'string'
          },
          pcc_rule: [
            {
              flow: [
                {
                  direction: 'number',
                  description: 'string'
                }
              ],
              qos: {
                index: 'number',
                arp: {
                  priority_level: 'number',
                  pre_emption_capability: 'number',
                  pre_emption_vulnerability: 'number'
                },
                mbr: {
                  downlink: { value: 'number', unit: 'number' },
                  uplink: { value: 'number', unit: 'number' }
                },
                gbr: {
                  downlink: { value: 'number', unit: 'number' },
                  uplink: { value: 'number', unit: 'number' }
                }
              }
            }
          ]
        }
      ]
    }
  ]
};

// 将对象转换为 YAML 格式的字符串
const yamlString = yaml.dump(NFConfig);

// 指定保存路径和文件名
const filePath = '/home/test/nfconfig.yaml';

// 保存到文件
fs.writeFileSync(filePath, yamlString);
