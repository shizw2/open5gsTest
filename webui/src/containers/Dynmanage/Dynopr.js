import React ,{ useState } from 'react';
import PropTypes from 'prop-types';
import styled from 'styled-components';
import { media } from 'helpers/style-utils';
import { Modal, Tooltip} from 'components';
import oc from 'open-color';

import axios from 'axios';
import path from 'path';

const inputStyle = {
    width: '300px',  // 设置宽度
    border: 'none', // 无边框
    outline: 'none', // 去掉聚焦时的轮廓
    //background: 'transparent', // 背景透明
    marginRight: '10px'
};
const inputnumberStyle = {
  width: '150px',  // 设置宽度
  border: 'none', // 无边框
  outline: 'none', // 去掉聚焦时的轮廓
  //background: 'transparent', // 背景透明
  marginRight: '10px'
};

const Drawer = styled.div`
  background-color: ${oc.gray[1]};

  padding: 1rem;
  margin: 0.5rem 0;
  border: 1px solid ${oc.gray[3]};
`;

const ToggleButton = styled.button`
  width: 100%;
  padding: 10px 15px;
  cursor: pointer;
  border: none;
  border-radius: 0;
  background-color: #007bff;
  color: #fff;
  box-shadow: 2px 2px 4px rgba(0, 0, 0, 0.6); /* 添加盒式阴影 */
`;

const DynoprButtons = () => {
  const ip = window.location.hostname; // 获取当前页面的 IP
  const port = window.location.port; // 获取当前页面的 port
  const [openDrawer, setOpenDrawer] = useState(null); // 用于跟踪打开的抽屉
  const [errorMessage, setErrorMessage] = useState('');
  const [filePathName, setFilePathName] = useState('/download');

  const [selectedFile, setSelectedFile] = useState(null);
  const [drawerWidth, setDrawerWidth] = useState('300px'); // 默认宽度
  const [imsi, setImsi] = useState('');
  const [page, setPage] = useState('');
  const [limit, setLimit] = useState('');
  const [userData, setUserData] = useState(null);
  const [bspage, setBsPage] = useState('');
  const [bslimit, setBsLimit] = useState('');
  const [baseStationData, setBaseStationData] = useState(null);
  const [fileList, setFileList] = useState([]); 
 // const handleToggleDrawer = (drawerName) => {
  //  setOpenDrawer(openDrawer === drawerName ? null : drawerName); // 切换抽屉打开状态
 
 // };
  const handleToggleDrawer = (title) => {
    if (openDrawer === title) {
      // 如果已经打开，则关闭并恢复宽度
      setOpenDrawer(null);
      setDrawerWidth('300px'); // 恢复默认宽度
    } else {
      // 打开新的drawer，同时增加宽度
      setOpenDrawer(title);
      setDrawerWidth('800px'); // 设置新的宽度
    }
  };
  const handleUploadFile = () => {
    const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/file/upload`; // 注意这个地址的拼写需要与题目一致
    
    if (!selectedFile) {
      setErrorMessage('请先选择一个文件进行上传');
      return;
    }
    const formData = new FormData(); 
    formData.append('file', selectedFile); // 直接添加文件对象，不需要编码文件名
    
    fetch(url, {
      method: 'POST',
      body: formData, // 直接传递FormData
    })
    .then(response => {
      if (!response.ok) {
        setErrorMessage('文件上传失败，状态码:' + response.status);
        throw new Error('网络响应不是 ok');
      }
      return response.json();
    })
    .then(data => {
      if (data.result === "FAIL") {
        setErrorMessage(data.error.message);
      } else {
        //console.log('文件上传成功:', data);
        setErrorMessage('文件上传成功');
        setSelectedFile(null); // 清除已选择的文件
        // 可以在此关闭 Drawer 或者显示成功信息
      }
    })
    .catch(error => {
      console.error('发生错误:', error);
    });
};
const fetchFileList = async () => {
  const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/file/filelist`;
  try {
    const response = await axios.get(url);
    console.log('获取文件列表成功:', response.data);
    if (Array.isArray(response.data)) {      
      setFileList(response.data); // 假设返回的文件列表在data属性中
    } else {
      setErrorMessage('获取文件列表失败');
    }
  } catch (error) {
    setErrorMessage('请求错误:' + error.message);
  }
};
const handleDownloadFile =async () => {
  const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/file/filelist`;
  try {
    const response = await axios.get(url,{params: {dir : filePathName}});
    //console.log('获取文件列表成功:', response.data);
    if (Array.isArray(response.data)) {    
      setErrorMessage('');  
      setFileList(response.data); // 假设返回的文件列表在data属性中
    } else {
      setErrorMessage('获取文件列表失败');
    }
  } catch (error) {
    setErrorMessage('请求错误:' + error.message);
    
  }
};
const handleImportFileClick = (e) => {
  const file = e.target.files[0];
  if (file) {
    setSelectedFile(file);
    setErrorMessage('');
  } else {
    setSelectedFile(null);
  }
};
const handleFilePathChange = (e) => {
  setFilePathName(e.target.value); // 更新文件名状态
};
const handleQueryue = async () => {
  try {
    const response = await axios.get(`http://${ip}:${port}/coreNetwork/wxCm/v1/software/ueStatus`, {
      params: {
        imsi,
        page_size: limit,
        page_num: page,
      },
    });
    
    // 假设成功返回数据的格式
    if (response.data.result_set) {
      setUserData(response.data.result_set); // 根据实际返回数据设置
      setErrorMessage(null);
    } else {
      setErrorMessage('查询失败，状态码:' + response.status);
      throw new Error(response.data.errorCode); // 假设返回的错误信息
    }
  } catch (error) {
    setErrorMessage(error.message);
    setUserData(null);
    //console.error('错误:', error);
  }
};
const handleQuerybs = async () => {
  try {
    const response = await axios.get(`http://${ip}:${port}/coreNetwork/wxCm/v1/software/ranNode`, {
      params: {
        page_size: bslimit,
        page_num: bspage,
      },
    });
    
    // 假设成功返回数据的格式
    if (response.data.result_set) {
      setBaseStationData(response.data.result_set); // 根据实际返回数据设置
      setErrorMessage(null);
    } else {
      setErrorMessage('查询失败，状态码:' + response.status);
      throw new Error(response.data.errorCode); // 假设返回的错误信息
    }
  } catch (error) {
    setErrorMessage(error.message);
    setBaseStationData(null);
    //console.error('错误:', error);
  }
};
const [systemAction, setSystemAction] = useState('start'); // 默认选中值

const handleSystemChange = (e) => {
  setSystemAction(e.target.value); 
  setErrorMessage('');
};

const handleSubmitSystem = async () => {
  try {
    const response = await axios.post(`http://${ip}:${port}/coreNetwork/wxCm/v1/software/system`, {
      actionType: systemAction, // 请求体参数
    });
    
    // 处理成功响应
    if (response.data.result==="OK") {
      setErrorMessage('操作成功');
      //console.log('提交成功:', response.data.message);
    } else {
      setErrorMessage(error.message);
      //console.error('提交失败:', response.data.error);
    }
  } catch (error) {
    setErrorMessage(error.message);
    //console.error('请求错误:', error);
  }
}
return (
    <div style={styles.container}>
      <ToggleButton onClick={() =>{setErrorMessage(''); handleToggleDrawer('文件上传')}}>
        文件上传
      </ToggleButton>
      {openDrawer === '文件上传' && (
        <Drawer >
            <label style={{ marginRight: '10px' }}> 
              <input type="file" onChange={handleImportFileClick} required  />   
            </label>
            <button onClick={handleUploadFile} style={styles.uploadButton}>上传文件</button>
            {errorMessage && <div style={{ color: 'red' }}>{errorMessage}</div>}
        </Drawer> 
      )}

      <ToggleButton onClick={() =>{
        setErrorMessage(''); 
        handleToggleDrawer('文件下载');
        fetchFileList();
        }}>
        文件下载
      </ToggleButton>
      {openDrawer === '文件下载' && (
        <Drawer>
          <label style={{ marginRight: '10px' }}>
            路径名:
            <input 
              type="text" 
              onChange={handleFilePathChange}
              value={filePathName}  
              style={inputStyle} 
            />
          </label>
          <button onClick={handleDownloadFile} style={styles.uploadButton}>查询</button>
          <div style={{ marginTop: '10px', maxHeight: '200px', overflowY: 'auto' }}>
            {fileList.map(file => {
              // 使用 path.parse 解析文件路径
              const { dir, base } = path.parse(file);              
              // 更新 filePathName，追加路径部分
              const updatedFilePathName = `${filePathName}/${dir}`;
              return (
                <div key={file} style={{ margin: '5px 0' }}>
                  <a 
                    href={`http://${ip}:${port}/download/${base}?dir=${encodeURIComponent(updatedFilePathName)}`} 
                    style={{ color: 'blue', textDecoration: 'underline' }}
                    target="_blank" 
                    rel="noopener noreferrer"
                  >
                    {file}
                  </a>
                </div>
              );
            })}
          </div>    
          
          {errorMessage && <div style={{ color: 'red' }}>{errorMessage}</div>}
        </Drawer>
      )}

      <ToggleButton onClick={() =>{setErrorMessage('');handleToggleDrawer('用户状态查询')}}>
        用户状态查询
      </ToggleButton>
      {openDrawer === '用户状态查询' && (
        <Drawer>
          <input 
            placeholder="输入IMSI"
            value={imsi}
            onChange={e => setImsi(e.target.value)}
            style={inputStyle}
            title="请输入查询用户IMSI"
          />
          <input 
            placeholder="输入页码"
            type="number"
            value={page}
            onChange={e => setPage(e.target.value)}
            min="0"
            title="请输入页码"
            style={inputnumberStyle}
          />
          <input 
            placeholder="输入每页数量"
            type="number"
            value={limit}
            onChange={e => setLimit(e.target.value)}
            min="0"
            title="请输入每页数量"
            style={inputnumberStyle}
          />
          <button type="primary" onClick={handleQueryue} style={styles.queryButton}>
            查询
          </button>
          {userData && (
            <div>
              <h4>用户信息：</h4>
              <pre>{JSON.stringify(userData, null, 2)}</pre>
            </div>
          )}
          {errorMessage && <div style={{ color: 'red' }}>{errorMessage}</div>}
        </Drawer>
      )}

      <ToggleButton onClick={() =>{setErrorMessage(''); handleToggleDrawer('基站状态查询')}}>
        基站状态查询
      </ToggleButton>
      {openDrawer === '基站状态查询' && (
        <Drawer>
          <input 
            placeholder="输入页码"
            type="number"
            value={bspage}
            onChange={e => setBsPage(e.target.value)}
            min="0"
            title="请输入页码"
            style={inputnumberStyle}
          />
          <input 
            placeholder="输入每页数量"
            type="number"
            value={bslimit}
            onChange={e => setBsLimit(e.target.value)}
            min="0"
            title="请输入每页数量"
            style={inputnumberStyle}
          />
          <button type="primary" onClick={handleQuerybs} style={styles.uploadButton}>
            查询
          </button>
          {baseStationData && (
            <div>
              <h4>基站状态信息：</h4>
              <pre>{JSON.stringify(baseStationData, null, 2)}</pre>
            </div>
          )}
          {errorMessage && <div style={{ color: 'red' }}>{errorMessage}</div>}
        </Drawer>
      )}
      <ToggleButton onClick={() =>{setErrorMessage(''); handleToggleDrawer('系统维护')}}>
        系统维护
      </ToggleButton>
      {openDrawer === '系统维护' && (
        <Drawer >          
          <label style={{ marginRight: '10px' }}>
          系统维护:
          <select onChange={handleSystemChange} style={{ marginLeft: '5px',border: 'none', }}>
            <option value="start">start</option>
            <option value="stop">stop</option>            
            <option value="restart">restart</option>
            <option value="reboot">reboot</option>            
          </select>
          </label>
          <h3 style={{ color: 'red' }}>危险操作，请慎重执行！    </h3>
          <button onClick={handleSubmitSystem} style={styles.submitButton}>提交</button>
          {errorMessage && <div style={{ color: 'red' }}>{errorMessage}</div>}
        </Drawer>
      )}
    </div>
  );
};

DynoprButtons.propTypes = {
  visible: PropTypes.bool,
  onHide: PropTypes.func,
  onLogout: PropTypes.func,
};

const styles = {
  container: {
    position: 'absolute',
    left: '280px',
    top: '100px',
    display: 'flex',
    flexDirection: 'column',
    gap: '10px',
    zIndex: 10,
    
  },
  uploadButton: {
    marginTop: '10px',
    marginLeft: '300px',
    padding: '7px 20px',
    cursor: 'pointer',
    border: 'none',
    borderRadius: '5px',
    backgroundColor: '#ff0000',
    color: '#fff',
    width: '150px', // 设置固定宽度
    whiteSpace: 'nowrap', // 防止换行
  },
  submitButton: {
    marginTop: '10px',
    marginLeft: '480px',
    padding: '7px 20px',
    cursor: 'pointer',
    border: 'none',
    borderRadius: '5px',
    backgroundColor: '#ff0000',
    color: '#fff',
    width: '150px', // 设置固定宽度
    whiteSpace: 'nowrap', // 防止换行
  },
  queryButton: {
    marginTop: '10px',
    marginLeft: '30px',
    padding: '7px 20px',
    cursor: 'pointer',
    border: 'none',
    borderRadius: '5px',
    backgroundColor: '#ff0000',
    color: '#fff',
    width: '150px', // 设置固定宽度
    whiteSpace: 'nowrap', // 防止换行
  },
};
export default DynoprButtons;
