import React ,{ useState } from 'react';
import PropTypes from 'prop-types';
import styled from 'styled-components';
import { media } from 'helpers/style-utils';
import { Modal, Tooltip} from 'components';
import oc from 'open-color';

const Wrapper = styled.div`
  width: 300px;

  ${media.mobile`
    width: calc(100vw - 2rem);
  `}
`;

const TitleWrapper = styled.div`
  padding-left: 1rem;
  line-height: 3rem;

  font-size: 1.2rem;

  color: white;
  background-color: ${oc.red[7]};
`;

const ContentWrapper = styled.div`
  padding: 1rem 0 0 1rem;
  height: 5rem;

  font-size: 1rem;
  color: ${oc.gray[7]};

  background-color: ${oc.gray[1]};
`;

const ButtonWrapper = styled.div`
  display: flex;
  justify-content: flex-end;
  background-color: ${oc.gray[2]};
`;

const Button = styled.button`
  margin: 0.5rem;
  padding: 0.3rem;
  width: 4rem;

  text-align: center;
  font-size: 0.9rem;

  border-radius: 3px;
  outline: none;
  cursor: pointer;
  
`;

const YesButton = styled(Button)`
  border: 1px solid ${oc.red[9]};
  color: white;
  background: ${oc.red[7]};
  &:hover {
    background: ${oc.red[5]}
  }
  &:active {
    background: ${oc.red[8]}
  }
`;

const NoButton = styled(Button)`
  border: 1px solid ${oc.gray[5]};
  color: black;
  background: ${oc.gray[3]};
  &:hover {
    background: ${oc.gray[2]}
  }
  &:active {
    background: ${oc.gray[4]}
  }
`;
const YesDelButton = styled(Button)`
  border: 1px solid ${oc.gray[5]};
  color: black;
  background: ${oc.gray[3]};
  &:hover {
    background: ${oc.gray[2]}
  }
  &:active {
    background: ${oc.gray[4]}
  }
`;

const NoDelButton = styled(Button)`
  border: 1px solid ${oc.red[9]};
  color: white;
  background: ${oc.red[7]};
  &:hover {
    background: ${oc.red[5]}
  }
  &:active {
    background: ${oc.red[8]}
  }
`;
const inputStyle = {
    width: '300px',  // 设置宽度
    border: 'none', // 无边框
    outline: 'none', // 去掉聚焦时的轮廓
    //background: 'transparent', // 背景透明
    marginRight: '10px'
};
const spanStyle = { 
    width: '70px', 
    textAlign: 'right' 
}
const labelstyle = {
     display: 'flex',
      justifyContent: 'space-between', 
      alignItems: 'center', 
      width: '100%'
     }
const propTypes = {
    visible: PropTypes.bool, 
    onHide: PropTypes.func, 
    onLogout: PropTypes.func,
  };

const MoprButtons = () => {
    const [isModalVisible, setIsModalVisible] = useState(false);
    const [isAddModalVisible, setIsAddModalVisible] = useState(false);
    const [isDelModalVisible, setIsDelModalVisible] = useState(false);
    const [isImportModalVisible, setIsImportModalVisible] = useState(false);
    const [isExportModalVisible, setIsExportModalVisible] = useState(false);
    const [isStaticIPModalVisible, setIsStaticIPModalVisible] = useState(false);
    const [errorMessage, setErrorMessage] = useState('');
    const [formData, setFormData] = useState({
        imsiRange: '',
        msisdnRange: '',
        ueNum: '',
        ki: '',
        opType: '',
        op: '',
        tpl: ''
      });
    const [formdelData, setFormdelData] = useState({
        imsiRange: '',
        ueNum: '',  
      });
    const [formimportData, setFormimportData] = useState({
        fileName: ''      
      });
    const [formstaticipData, setFormstaticipData] = useState({
        ip: ''      
      });
    const ip = window.location.hostname; // 获取当前页面的 IP
    const port = window.location.port; // 获取当前页面的 port
    const handleChange = (e) => {
        const { name, value } = e.target;
        setFormData({ ...formData, [name]: value });
      };
    
    const handleSubmit = (e) => {
        e.preventDefault();
        onSubmit(formData);
      };

    const handledelChange = (e) => {
        const { name, value } = e.target;
        setFormdelData({ ...formdelData, [name]: value });        
      };
    
    const handledelSubmit = (e) => {
        e.preventDefault();
        onSubmit(formdelData);
      };
    const handleImportChange = (e) => {
        const { name, value } = e.target;
        setFormimportData({ ...formimportData, [name]: value });        
      };
    const handleStaticIPChange = (e) => {
        const { name, value } = e.target;
        setFormstaticipData({ ...formstaticipData, [name]: value });        
      };
    const handleimportSubmit = (e) => {
        e.preventDefault();
        onSubmit(formimportData);
      };
    const handlestaticipSubmit = (e) => {
        e.preventDefault();
        onSubmit(formstaticipData);
      };
    const handleButtonClick = (action) => {
    console.log(`${action} 按钮被点击!`);
    // 在这里添加按钮点击后的逻辑
    if (action === '连续开户') {
        setIsAddModalVisible(true);
        console.log('连续开户按钮被点击!');
    }else if (action === '连续销户') {
        setIsDelModalVisible(true);
        console.log('连续销户按钮被点击!');
    }else if (action === '全量销户') {
        setIsModalVisible(true);     
        console.log('全量销被点击!');       
    }else if (action === '静态IP修改') {
        setIsStaticIPModalVisible(true);     
        console.log('全量销被点击!');       
    }else if (action === '导入') {
        setIsImportModalVisible(true);  
        console.log('导入按钮被点击!');
    }else if (action === '导出') {
        setIsExportModalVisible(true);
        console.log('导出按钮被点击!');
    }
  };
  const handleStaticIPSubClick = () => {
    const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/forward/udm/ipManager?mode=mode1`;
    if(formstaticipData.ip===""){      
        setErrorMessage('输入参数不能为空');
        return;  
    };
    fetch(url, {
      method: 'POST', 
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(formstaticipData),
    })
    .then(response => {
      if (!response.ok) {
        response.text().then(body => {         
          const jsonResponse = JSON.parse(body); 
          const reason = jsonResponse.error.detail.invalidParam.reason; 
          setErrorMessage('网络响应错误，状态码:'+response.status+',原因:'+reason);
        });
        throw new Error('网络响应不是 ok');
      }
      return response.json(); // 解析 JSON 响应
    })
    .then(data => {
        if(data.result === "FAIL"){
            setErrorMessage(data.error.message);
        }else{
            setErrorMessage('操作成功,重新刷新页面后查看结果');
            //setIsStaticIPModalVisible(false);
            setFormstaticipData({ // 清空 formData
                ip: '',      
            });
            console.log('导入成功:', data);
        }
    })
    .catch(error => {
      console.error('发生错误:', error);
    });
  };
  const handleImportSubClick = () => {
    const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/config/importUe`;
    if(formimportData.fileName===""){      
        setErrorMessage('输入参数不能为空');
        return;  
    };
    fetch(url, {
      method: 'POST', 
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(formimportData),
    })
    .then(response => {
      if (!response.ok) {
        setErrorMessage('网络响应错误，状态码:'+response.status); 
        response.text().then(body => {         
          //const jsonResponse = JSON.parse(body); 
          //const reason = jsonResponse.error.detail.invalidParam.reason; 
          //setErrorMessage('网络响应错误，状态码:'+response.status+',原因:'+reason);
        }); 
        throw new Error('网络响应不是 ok');
      }
      return response.json(); // 解析 JSON 响应
    })
    .then(data => {
        if(data.result === "FAIL"){
            setErrorMessage(data.error.message);
        }else{
            setErrorMessage('操作成功,重新刷新页面后查看结果');
            //setIsImportModalVisible(false);
            setFormimportData({ // 清空 formData
                fileName: '',      
            });
            console.log('导入成功:', data);
        }
    })
    .catch(error => {
      console.error('发生错误:', error);
    });
  };
  const handleExportAllSubClick = () => {
    const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/config/exportUe`;
    fetch(url, {
      method: 'POST', // 设置请求方法为 DELETE
    })
    .then(response => {
      if (!response.ok) {
        setErrorMessage('网络响应错误，状态码:'+response.status); 
        response.text().then(body => { 
                  
          //const jsonResponse = JSON.parse(body); 
          //const reason = jsonResponse.error.detail.invalidParam.reason; 
          //setErrorMessage('网络响应错误，状态码:'+response.status+',原因:'+reason);
        });
        throw new Error('网络响应不是 ok');
      }
      return response.json(); // 解析 JSON 响应
    })
    .then(data => {
        if(data.result === "FAIL"){
            setErrorMessage(data.error.message);
        }else{
            //setErrorMessage('');
            //setIsExportModalVisible(false);
            setErrorMessage('操作成功,到动态管理页面下载导出文件');
            console.log('导出成功:', data);
        }
    })
    .catch(error => {
      console.error('发生错误:', error);
    });
  };
  const handleDelAllSubClick = () => {
    const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/ue/regSub`;
    fetch(url, {
      method: 'DELETE', // 设置请求方法为 DELETE
    })
    .then(response => {
      if (!response.ok) {
        setErrorMessage('网络响应错误，状态码:'+response.status); 
        throw new Error('网络响应不是 ok');
      }
      return response.json(); // 解析 JSON 响应
    })
    .then(data => {
        if(data.result === "FAIL"){
            setErrorMessage(data.error.message);
        }else{
            //setErrorMessage('');
            //setIsModalVisible(false);
            setErrorMessage('操作成功,重新刷新页面后查看结果');
            console.log('删除成功:', data);
        }
    })
    .catch(error => {
      console.error('发生错误:', error);
    });
  };
  const handleAddContinueSubClick = () => {
    const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/ue/regSub`;
    console.log('连续开户表单数据:', formData);
    if(formData.imsiRange==="" || formData.msisdnRange==="" || formData.ueNum==="" || formData.ki==="" || formData.opType==="" || formData.op==="" || formData.tpl===""){      
        setErrorMessage('输入参数不能为空');
        return;  
    };
    if(formData.opType === 'opc'){
        //在formData中增加opc字段，删除op字段，将op的值赋值给opc字段
        formData.opc = formData.op;        
        delete formData.op;        
    };
   
    fetch(url, {
      method: 'PUT',
      headers: {
        'Content-Type': 'application/json',
      },
     
      body: JSON.stringify(formData),
    })
    .then(response => {
      if (!response.ok) {
        //console.log('网络响应错误,response:', response);
        response.text().then(body => {         
          const jsonResponse = JSON.parse(body); 
          const reason = jsonResponse.error.detail.invalidParam.reason; 
          setErrorMessage('网络响应错误，状态码:'+response.status+',原因:'+reason);
        });
        //setErrorMessage('网络响应错误，状态码:'+response.status);        
        throw new Error('网络响应不是 ok');S
      }
      return response.json(); // 解析 JSON 响应
    })
    .then(data => {
        if(data.result === "FAIL"){
            setErrorMessage(data.error.message);
        }else{
            //setIsAddModalVisible(false);
            //setErrorMessage('');
            setErrorMessage('操作成功,重新刷新页面后查看结果');
            setFormData({ // 清空 formData
                imsiRange: '',
                msisdnRange: '',
                ueNum: '',
                ki: '',
                opType: '',
                op: '',
                tpl: ''
            });
            console.log('连续开户成功:', data);
        }
    })
    .catch(error => {
      console.error('发生错误:', error);
    });
  };
  const handleDelContinueSubClick = () => {
    const url = `http://${ip}:${port}/coreNetwork/wxCm/v1/ue/cancelSub?imsiRange=${formdelData.imsiRange}&ueNum=${formdelData.ueNum}`;
    console.log('连续销户表单数据:', formdelData);
    if(formdelData.imsiRange==="" || formdelData.ueNum===""){      
        setErrorMessage('输入参数不能为空');
        return;  
    };
    fetch(url, {
      method: 'DELETE',
      headers: {
        'Content-Type': 'application/json',
      },
      
    })
    .then(response => {
      if (!response.ok) {
        response.text().then(body => {         
          const jsonResponse = JSON.parse(body); 
          const reason = jsonResponse.error.detail.invalidParam.reason; 
          setErrorMessage('网络响应错误，状态码:'+response.status+',原因:'+reason);
        });   
        throw new Error('网络响应不是 ok');
      }
      return response.json(); // 解析 JSON 响应
    })
    .then(data => {
        if(data.result === "FAIL"){
            setErrorMessage(data.error.message);
        }else{        
        //setErrorMessage('');
        //setIsDelModalVisible(false);
        setErrorMessage('操作成功,重新刷新页面后查看结果');
        setFormdelData({ // 清空 formData
            imsiRange: '',  
            ueNum: '',  
        });
        console.log('连续销户成功:', data);
       }
    })
    .catch(error => {      
      console.error('发生错误:', error);
    });
  };
  return (
    <div style={styles.container}>
      <button onClick={() => handleButtonClick('连续开户')} style={styles.button}>连续开户</button>
      <button onClick={() => handleButtonClick('连续销户')} style={styles.button}>连续销户</button>
      <button onClick={() => handleButtonClick('全量销户')} style={styles.button}>全量销户</button>
      <button onClick={() => handleButtonClick('静态IP修改')} style={styles.button}>静态IP修改</button>
      <button onClick={() =>{setErrorMessage('请确认模版已在动态管理模块上传服务器');handleButtonClick('导入')}} style={styles.button}>导入</button>
      <button onClick={() => handleButtonClick('导出')} style={styles.button}>导出</button>
      
      {isModalVisible && ( // 控制 全量销户Modal 显示
        <Modal visible={isModalVisible} onOutside={() => setIsModalVisible(false)}>
          <Wrapper>
            <TitleWrapper>
              全量销户
            </TitleWrapper>
            <ContentWrapper>
              危险操作，确定全量销户？！
              {errorMessage ? <div style={{ color: 'red', marginTop: '10px' }}>{errorMessage}</div>: null}  
            </ContentWrapper>
            <ButtonWrapper>
              <YesDelButton onClick={handleDelAllSubClick}>
                提交
              </YesDelButton>
              <NoDelButton onClick={() =>{
                setErrorMessage('');
                setIsModalVisible(false)
              }}>
                退出
              </NoDelButton>
            </ButtonWrapper>
          </Wrapper>
        </Modal>
      )}
    {isExportModalVisible && ( 
        <Modal visible={isExportModalVisible} onOutside={() => setIsExportModalVisible(false)}>
          <Wrapper>
            <TitleWrapper>
              导出用户
            </TitleWrapper>
            <ContentWrapper>
              确定导出用户？
              {errorMessage ? <div style={{ color: 'red', marginTop: '10px' }}>{errorMessage}</div>: null}  
            </ContentWrapper>
            <ButtonWrapper>
              <YesButton onClick={handleExportAllSubClick}>
                提交
              </YesButton>
              <NoButton onClick={() =>{
                setErrorMessage('');
                setIsExportModalVisible(false)
              }}>
                退出
              </NoButton>
            </ButtonWrapper>
          </Wrapper>
        </Modal>
      )}
     {isAddModalVisible && ( // 控制 连续Modal 显示
        <Modal visible={isAddModalVisible} onOutside={() => setIsAddModalVisible(false)}>
          <Wrapper>
            <TitleWrapper style={{ width: '400px'}}>
              连续开户
            </TitleWrapper>
            <ContentWrapper style={{ height: '280px', width: '400px'}}>                
                <label style={labelstyle}>
                    <span style={spanStyle}>
                        起始IMSI:</span>
                    <input type="text" name="imsiRange" style={inputStyle} value={formData.imsiRange} onChange={handleChange} required />
                </label>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                     MSISDN: </span>
                    <input type="text" name="msisdnRange" style={inputStyle} value={formData.msisdnRange} onChange={handleChange} required />
                </label>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                    用户数量:</span>
                    <input type="number" name="ueNum" style={inputStyle} value={formData.ueNum} onChange={handleChange} required min="0"/>
                </label>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                    K:      </span>
                    <input type="text" name="ki" style={inputStyle} value={formData.ki} onChange={handleChange} required />
                </label>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                    opType:</span>
                    <select name="opType" style={inputStyle} value={formData.opType} onChange={handleChange} required>
                    <option value="">请选择</option>
                    <option value="op">op</option>
                    <option value="opc">opc</option>
                    </select>
                </label>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                    OP/OPC:</span>
                    <input type="text" name="op" style={inputStyle} value={formData.op} onChange={handleChange} required />
                </label>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                    模板:</span>
                    <input type="text" name="tpl" style={inputStyle} value={formData.tpl} onChange={handleChange} required />
                </label>
                {errorMessage && <div style={{ color: 'red', marginTop: '10px' }}>{errorMessage}</div>}
            </ContentWrapper>
            <ButtonWrapper>
              <YesButton onClick={handleAddContinueSubClick}>
                提交
              </YesButton>
              <NoButton onClick={() =>{
                setErrorMessage('');  
                setIsAddModalVisible(false)
                }}>
                退出
              </NoButton>
            </ButtonWrapper>
          </Wrapper>
        </Modal>
      )}
     {isDelModalVisible && ( // 控制 连续Modal 显示
        <Modal visible={isDelModalVisible} onOutside={() => setIsDelModalVisible(false)}>
          <Wrapper>
            <TitleWrapper style={{ width: '400px'}}>
              连续销户
            </TitleWrapper>
            <ContentWrapper style={{ height: '280px', width: '400px'}}>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                        起始IMSI:</span>
                    <input type="text" name="imsiRange" style={inputStyle} value={formdelData.imsiRange} onChange={handledelChange} required />
                </label>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                    用户数量:</span>
                    <input type="number" name="ueNum" style={inputStyle} value={formdelData.ueNum} onChange={handledelChange} required min="0"/>
                </label>
                {errorMessage ? <div style={{ color: 'red', marginTop: '10px' }}>{errorMessage}</div>: null}                
            </ContentWrapper>
            <ButtonWrapper>
              <YesButton onClick={handleDelContinueSubClick}>
                提交
              </YesButton>
              <NoButton onClick={() => {
                setErrorMessage(''); 
                setIsDelModalVisible(false);
                }}>    
                退出
              </NoButton>
            </ButtonWrapper>
          </Wrapper>
        </Modal>
      )}
     {isImportModalVisible && ( // 控制 连续Modal 显示
        <Modal visible={isImportModalVisible} onOutside={() => setIsImportModalVisible(false)}>
          <Wrapper>
            <TitleWrapper style={{ width: '400px'}}>
              从文件导入
            </TitleWrapper>
            <ContentWrapper style={{ height: '280px', width: '400px'}}>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                        文件名称:</span>
                    <input type="text" name="fileName" style={inputStyle} value={formimportData.fileName} onChange={handleImportChange} required />
                </label> 
                {errorMessage ? <div style={{ color: 'red', marginTop: '10px' }}>{errorMessage}</div>: null}                
            </ContentWrapper>
            <ButtonWrapper>
              <YesButton onClick={handleImportSubClick}>
                提交
              </YesButton>
              <NoButton onClick={() => {
                setErrorMessage(''); 
                setIsImportModalVisible(false);
                }}>    
                退出
              </NoButton>
            </ButtonWrapper>
          </Wrapper>
        </Modal>
      )}
    {isStaticIPModalVisible && ( // 控制 连续Modal 显示
        <Modal visible={isStaticIPModalVisible} onOutside={() => setIsStaticIPModalVisible(false)}>
          <Wrapper>
            <TitleWrapper style={{ width: '400px'}}>
              用户IP地址连续修改
            </TitleWrapper>
            <ContentWrapper style={{ height: '280px', width: '400px'}}>
                <label style={labelstyle}>
                    <span style={spanStyle}>
                        IP地址段:</span>
                    <input type="text" name="ip" style={inputStyle} value={formstaticipData.ip} onChange={handleStaticIPChange} required />
                </label> 
                {errorMessage ? <div style={{ color: 'red', marginTop: '10px' }}>{errorMessage}</div>: null}                
            </ContentWrapper>
            <ButtonWrapper>
              <YesButton onClick={handleStaticIPSubClick}>
                提交
              </YesButton>
              <NoButton onClick={() => {
                setErrorMessage(''); 
                setIsStaticIPModalVisible(false);
                }}>    
                退出
              </NoButton>
            </ButtonWrapper>
          </Wrapper>
        </Modal>
      )}       
    </div>
  );
};
MoprButtons.propTypes = propTypes;
const styles = {
  container: {
    position: 'absolute',
    right: '20px', // 距离右边20px
    top: '100px',  // 距离顶部20px
    display: 'flex',
    gap: '10px',
    zIndex: 10, // 确保按钮在最上层
  },
  button: {
    padding: '10px 15px',
    cursor: 'pointer',
    border: 'none',
    borderRadius: '0px',
    backgroundColor: '#007bff',
    color: '#fff',
    boxShadow: '0 4px 10px rgba(0, 0, 0, 0.4)',
  },
};

export default MoprButtons;
