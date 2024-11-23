const express = require('express');
const router = express.Router();

//3.10 获取救援模式
router.get('/rescueMode', (req, res) => {
    // 构建成功响应
    const response = {
        result: "OK",
        result_set: true
    };
    
    // 返回JSON响应
    res.json(response);
});

//3.06 获取模式身份信息
router.get('/role', (req, res) => {
    // 从请求体中获取"group"
    const { group } = req.body;
  
    // 构建成功响应
    const response = {
      result: "OK",
      result_set: [
        {
          group: group,
          node: "N1",
          ip: "192.168.100.2"
        },
        {
          group: group,
          node: "N2",
          ip: "192.168.100.2"
        },
        {
          group: group,
          node: "N3",
          ip: "192.168.100.2"
        }
      ]
    };
    
    // 返回JSON响应
    res.json(response);
});
//6.6用户数据导入
router.post('/importUe', (req, res) => {
  // 构建成功响应
  const response = {
      result: "OK",
      result_set: true
  };
  
  // 返回JSON响应
  res.json(response);
});
//6.7用户数据导出
router.post('/exportUe', (req, res) => {
  // 构建成功响应
  const response = {
      result: "OK",
      result_set: true
  };
  
  // 返回JSON响应
  res.json(response);
});
// 导出路由
module.exports = router;