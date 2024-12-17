const express = require('express');
const router = express.Router();
const url = require('url');
const fs = require('fs');
const path = require('path');
const downloadDir  = path.join(__dirname, '../../../../../download');
//定义一个数组，存放允许下载的目录
const allowDownloadDir = ['/download', '/home/5gc/webui/server/uploads'];
let currentDir = "";
const isSubdirectory = (baseDir, targetDir) => {
    const relative = path.relative(baseDir, targetDir);
    return relative && !relative.startsWith('..') && !path.isAbsolute(relative);
  };
function getAllFiles(dir) {
    let results = [];
    const list = fs.readdirSync(dir);
  
    list.forEach(file => {
      const filePath = path.join(dir, file);
      const stat = fs.statSync(filePath);
      
      if (stat && stat.isDirectory()) {
        // 如果是目录，递归调用
        results = results.concat(getAllFiles(filePath));
      } else {
        // 如果是文件，添加到结果数组
        results.push(path.relative(currentDir, filePath));
      }
    });
    
  return results;
}
router.get('/filelist', (req, res) => {
  const query = url.parse(req.url, true).query;
  console.log(query);
  const dir = query.dir || downloadDir;
  const allowed = allowDownloadDir.some(allowedDir => {
    return allowedDir === dir || isSubdirectory(allowedDir, dir);
  });

  if (!allowed) {
    res.status(403).send('目录不允许下载');
    return; 
  }
  currentDir=dir;
  const files = getAllFiles(dir);
  console.log(files);
  res.json(files);
});

module.exports = router;