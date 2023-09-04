import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import NProgress from 'nprogress';

import { Alarm } from 'components';

import traverse from 'traverse';
import crypto from 'crypto';
const os = require('os');
const ip = os.hostname();
//const ip = window.location.hostname
const formData = {
  "roles": [ "user" ],
}

class Document extends Component {
  constructor(props) {
    super(props)
    this.state = {
      selectedUserOption: '1h',
      selectedRegOption: '1h',
      selectedGtpOption: '1h',
      loading: true,
      loadingReg: true,
      loadingGtp: true
      
    }

  }
  handleIframeLoad = () => {
    this.setState({ loading: false });
  }
  handleIframeLoadReg = () => {
    this.setState({ loadingReg: false });
  }
  handleIframeLoadGtp = () => {
    this.setState({ loadingGtp: false });
  }
  componentDidMount() {
    // 在组件挂载后，通过设置 loading 状态的方式触发 iframe 加载完成的效果
    setTimeout(() => {
      this.setState({ loading: false });
    }, 100); // 这里使用了一个延时来模拟加载时间，你也可以根据实际情况进行调整
    setTimeout(() => {
      this.setState({ loadingReg: false });
    }, 600); 
    setTimeout(() => {
      this.setState({ loadingGtp: false });
    }, 1200);   
  }
  handleOptionChange = (event) => {
    this.setState({ selectedUserOption: event.target.value });
  }
  handleRegOptionChange = (event) => {
    this.setState({ selectedRegOption: event.target.value });
  }
  handleGtpOptionChange = (event) => {
    this.setState({ selectedGtpOption: event.target.value });
  }
  static propTypes = {
    action: PropTypes.string,
    visible: PropTypes.bool, 
    onHide: PropTypes.func
  }

  state = {
    formData,
    emptyText:"暂无数据"
  }

  
  render() {
    
    const { selectedUserOption} = this.state
    const fromValue = `now-${selectedUserOption.toLowerCase()}`
    const { selectedRegOption} = this.state
    const fromRegValue = `now-${selectedRegOption.toLowerCase()}`
    const { selectedGtpOption} = this.state
    const fromGtpValue = `now-${selectedGtpOption.toLowerCase()}`
    const { loading } = this.state;
    const { loadingReg } = this.state;
    const { loadingGtp } = this.state;
    return (
      <div>
        {/* ====实时在线统计===*/}
        <div style={{ display: 'flex', justifyContent: 'space-between' }}>
          <div style={{ position: 'relative', width: '87%' }}>          
            {loading ? (
              <div>正在加载 ...请等待...</div> // 加载中的提示信息          
              ):(<iframe
                title="实时在线用户统计"  
                //====实时在线用户统计===           
                src={`http://${ip}:3002/d-solo/f8997824-d2b3-4caa-aa3a-6e7e408ce4a2/guage?orgId=1&refresh=5s&frameborder="0"&panelId=1&theme=light&from=${fromValue}&to=now&kiosk" `}
                style={{
                  width: '100%',
                  border: '10px',
                  height: '300px',
                  fontWeight: 'bold',
                  marginLeft: '0'
                  }}
                sandbox="allow-same-origin allow-scripts  allow-forms"
                scrolling="auto"
                onLoad={this.handleIframeLoad}
                ></iframe>)            
            }            
            <div style={{ position: 'absolute', top: '10px', right: '10px' }}>
              <form id="myForm" style={{ border: '1px solid #ccc', borderRadius: '5px', padding: '3px' }}>
              <select
                  value={selectedUserOption}
                  onChange={this.handleOptionChange}
                  style={{ border: 'none', outline: 'none', background: 'transparent', fontFamily: 'sans-serif', fontSize: '0.8rem', fontWeight: 500 }}>
                  <option value="1h">1小时</option>h
                  <option value="3h">3小时</option>
                  <option value="6h">6小时</option>
                  <option value="12h">12小时</option>
                  <option value="24h">24小时</option>
                </select>
              </form>
            </div>
          </div>
            {loading ? (
              <div>     </div> // 加载中的提示信息 空，第一条已经加载提示         
              ):(<iframe
              title="实时在线基站统计"   
              //====实时在线基站统计===          
              src={`http://${ip}:3002/d-solo/f8997824-d2b3-4caa-aa3a-6e7e408ce4a2/guage?orgId=1&refresh=5s&panelId=2&theme=light&kiosk" `}
              style={{ width: '13%', border: '10px', height: '300px' }}
              sandbox="allow-same-origin allow-scripts allow-forms"
              scrolling="auto"
              onLoad={this.handleIframeLoad}
            ></iframe>)
            }
        </div>
        {/* ====注册统计===*/}
        <div style={{ display: 'flex', justifyContent: 'space-between' }}>
          <div style={{ position: 'relative', width: '61%' }}>
            {loadingReg ? (
              <div>     </div> // 加载中的提示信息   空，第一条已经加载提示       
              ):(<iframe
              title="每分钟注册请求数" 
              //====每分钟注册请求数统计===     
              src={`http://${ip}:3002/d-solo/a3150b99-8d60-4909-88ef-8eed009ae06d/guage-copy-copy?orgId=1&refresh=5s&panelId=13&theme=light&from=${fromRegValue}&to=now&kiosk" `}     
              style={{ width: '100%', border: '10px', height: '300px' }}
              andbox="allow-same-origin allow-scripts  allow-forms"
              scrolling="auto"
              onLoad={this.handleIframeLoadReg}
              ></iframe>)
            }
            <div style={{ position: 'absolute', top: '10px', right: '10px' }}>
              <form id="myForm" style={{ border: '1px solid #ccc', borderRadius: '5px', padding: '3px' }}>
              <select
                  value={selectedRegOption}
                  onChange={this.handleRegOptionChange}
                  style={{ border: 'none', outline: 'none', background: 'transparent', fontFamily: 'sans-serif', fontSize: '0.8rem', fontWeight: 500 }}>
                  <option value="1h">1小时</option>h
                  <option value="3h">3小时</option>
                  <option value="6h">6小时</option>
                  <option value="12h">12小时</option>
                  <option value="24h">24小时</option>
                </select>
              </form>
            </div>
          </div>
          {loadingReg ? (
          <div>      </div> // 加载中的提示信息 空，第一条已经加载提示         
            ):(<iframe
            title="resg"        
            //====初始注册请求数===     
            src={`http://${ip}:3002/d-solo/a3150b99-8d60-4909-88ef-8eed009ae06d/guage-copy-copy?orgId=1&refresh=5s&var-instance1=All&theme=light&panelId=11&from=${fromRegValue}&to=now&kiosk" `}     
            style={{ width: '13%', border: '10px', height: '300px' }}
            andbox="allow-same-origin allow-scripts  allow-forms"
            scrolling="auto"
            onLoad={this.handleIframeLoadReg}
            ></iframe>)
          }
          {loadingReg ? (
            <div>     </div> // 加载中的提示信息空，第一条已经加载提示          
            ):(<iframe
            title="resg"        
            //====移动性注册请求数===  
            src={`http://${ip}:3002/d-solo/a3150b99-8d60-4909-88ef-8eed009ae06d/guage-copy-copy?orgId=1&refresh=5s&var-instance1=All&theme=light&panelId=18&from=${fromRegValue}&to=now&kiosk" `}     
            style={{ width: '13%', border: '10px', height: '300px' }}
            andbox="allow-same-origin allow-scripts  allow-forms"
            scrolling="auto"
            onLoad={this.handleIframeLoadReg}
            ></iframe>)
          }
          {loadingReg ? (
            <div>      </div> // 加载中的提示信息 空，第一条已经加载提示         
            ):(<iframe
            title="resg"        
            //====周期注册请求数===  
            src={`http://${ip}:3002/d-solo/a3150b99-8d60-4909-88ef-8eed009ae06d/guage-copy-copy?orgId=1&refresh=5s&var-instance1=All&theme=light&panelId=15&from=${fromRegValue}&to=now&kiosk" `}     
            style={{ width: '13%', border: '10px', height: '300px' }}
            andbox="allow-same-origin allow-scripts allow-forms"
            scrolling="auto"
            onLoad={this.handleIframeLoadReg}
            ></iframe>)
          }        
      </div>
      {/* =====用户面统计===*/}
      <div style={{ display: 'flex', justifyContent: 'space-between' }}>
        <div style={{ position: 'relative', width: '74%' }}>
          {loadingGtp ? (
            <div>     </div> // 加载中的提示信息 空，第一条已经加载提示         
            ):(<iframe
                title="resg"        
                //====N3 Gtp转发===  
                src={`http://${ip}:3002/d-solo/a3150b99-8d60-4909-88ef-8eed009ae06d/guage-copy-copy?orgId=1&refresh=5s&var-instance1=All&theme=light&panelId=19&from=${fromGtpValue}&to=now&kiosk" `}     
                style={{ width: '100%', border: '10px', height: '300px' }}
                andbox="allow-same-origin allow-scripts  allow-forms"
                scrolling="auto"
                onLoad={this.handleIframeLoadGtp}
              ></iframe>)
          }
           <div style={{ position: 'absolute', top: '10px', right: '10px' }}>
              <form id="myForm" style={{ border: '1px solid #ccc', borderRadius: '5px', padding: '3px' }}>
              <select
                  value={selectedGtpOption}
                  onChange={this.handleGtpOptionChange}
                  style={{ border: 'none', outline: 'none', background: 'transparent', fontFamily: 'sans-serif', fontSize: '0.8rem', fontWeight: 500 }}>
                  <option value="1h">1小时</option>h
                  <option value="3h">3小时</option>
                  <option value="6h">6小时</option>
                  <option value="12h">12小时</option>
                  <option value="24h">24小时</option>
                </select>
              </form>
            </div>
        </div>
        {loadingGtp ? (
          <div>     </div> // 加载中的提示信息   空，第一条已经加载提示       
            ):(<iframe
            title="resg"        
            //====N3 Gtp发送数据包=== 
            src={`http://${ip}:3002/d-solo/a3150b99-8d60-4909-88ef-8eed009ae06d/guage-copy-copy?orgId=1&refresh=5s&var-instance1=All&theme=light&panelId=14&from=${fromGtpValue}&to=now&kiosk" `}     
            style={{ width: '13%', border: '10px', height: '300px' }}
            andbox="allow-same-origin allow-scripts  allow-forms"
            scrolling="auto"
            onLoad={this.handleIframeLoadGtp}
          ></iframe>)
        }
        {loadingGtp ? (
            <div>     </div> // 加载中的提示信息,空，第一条已经加载提示          
            ):(<iframe
            title="resg"        
            //srcDoc=====N3 Gtp接收数据包=== 
            src={`http://${ip}:3002/d-solo/a3150b99-8d60-4909-88ef-8eed009ae06d/guage-copy-copy?orgId=1&refresh=5s&var-instance1=All&theme=light&panelId=21&from=${fromGtpValue}&to=now&kiosk" `}     
            style={{ width: '13%', border: '10px', height: '300px' }}
            andbox="allow-same-origin allow-scripts  allow-forms"
            scrolling="auto"
            onLoad={this.handleIframeLoadGtp}
          ></iframe>)
        }
        </div>
      </div>
    )
  }
}



export default Document;
