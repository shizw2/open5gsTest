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
      loadingGtp: true,
      selectedNavItem: '接入指标'
      
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
  handleNavItemClick(item) {
    this.setState({ selectedNavItem: item });
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
    const styles = {
      verticalLine: {
        width: '1px',
        height: '20px',
        backgroundColor: '#5C7CFA',
        margin: '0 1px',
        display: 'inline-block',
      },
    
      underline: {
        width: '100%',
        height: '2px',
        backgroundColor: '#5C7CFA',
      }
    }
    return (
      <div>
        <div style={{ display: 'flex', justifyContent: 'flex-end' }}>
          <nav >
            {/* 导航链接 */}
            <button 
            className={`nav-link ${this.state.selectedNavItem === '接入指标' ? 'selected' : ''}`}
            style={{
              border: 'none',
              padding: '10px',
              backgroundColor: this.state.selectedNavItem === '接入指标' ? '#5C7CFA' : 'lightgray',
              color: 'white'
            }}
            onClick={() => this.handleNavItemClick('接入指标')}
          >
            接入指标
          </button>
          
          <button 
            className={`nav-link ${this.state.selectedNavItem === '数据转发' ? 'selected' : ''}`}
            style={{
              border: 'none',
              padding: '10px',
              backgroundColor: this.state.selectedNavItem === '数据转发' ? '#5C7CFA' : 'lightgray',
              color: 'white'
            }}
            onClick={() => this.handleNavItemClick('数据转发')}
          >          
            数据转发
          </button>
          
          <button 
            className={`nav-link ${this.state.selectedNavItem === '物理资源' ? 'selected' : ''}`}
            style={{
              border: 'none',
              padding: '10px',
              backgroundColor: this.state.selectedNavItem === '物理资源' ? '#5C7CFA' : 'lightgray',
              color: 'white'
            }}
            onClick={() => this.handleNavItemClick('物理资源')}
          >
            物理资源
          </button>
          </nav>
        </div>
        <div style={styles.underline}></div>
        {/* ====实时在线统计===*/}
        {this.state.selectedNavItem === '接入指标' && (<div>
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
                      marginTop: '1px',
                      marginBottom: '0px',
                      marginLeft: '0px',
                      marginRight: '0px',
                      boxShadow: '0px 3px 6px rgba(0, 0, 0, 0.16)'
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
                  style={{ width: '12.8%', border: '10px', height: '300px',marginTop: '1px' }}
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
          </div>)}
      {/* =====用户面统计===*/}
      {this.state.selectedNavItem === '数据转发' && (<div>
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
        </div>)}
         {/* =====用户面统计 end===*/}
         {/* =====资源监控 begin===*/}
         {this.state.selectedNavItem === '物理资源' && (<div>
          <div style={{ display: 'flex', flexDirection: 'row', flexWrap: 'wrap'}}>
              <div style={{ display: 'flex', flexDirection: 'column',width: '10%' }}>
                <iframe
                  id="myFrame1"
                  title="resg"        
                  //srcDoc =====运行时间=== 
                  src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=15" width="450" height="200" frameborder="0"&kiosk" `}
                  style={{ width: '100%', border: '10px', height: '100px' }}
                  sandbox="allow-same-origin allow-scripts allow-forms"
                  scrolling="auto"            
                ></iframe>
                <iframe
                  id="myFrame2"
                  title="resg"        
                  //srcDoc=====核数=== 
                  src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=14" width="450" height="200" frameborder="0"&kiosk" `}
                  style={{ width: '100%', border: '10px', height: '100px' }}
                  sandbox="allow-same-origin allow-scripts allow-forms"
                  scrolling="auto"            
                ></iframe>
              </div>

              <div style={{ display: 'flex', flexDirection: 'column',width: '10%' }}>
                <iframe
                  id="myFrame3"
                  title="resg"        
                  //srcDoc=====总内存=== 
                  src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=75" width="450" height="200" frameborder="0"&kiosk" `}
                  style={{ width: '100%', border: '10px', height: '100px'  }}
                  sandbox="allow-same-origin allow-scripts allow-forms"
                  scrolling="auto"            
                ></iframe>
                <iframe
                  id="myFrame4"
                  title="resg"        
                  //srcDoc=====IO WAIT=== 
                  src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=20" width="450" height="200" frameborder="0"&kiosk" `}
                  style={{ width: '100%', border: '10px', height: '100px' }}
                  sandbox="allow-same-origin allow-scripts allow-forms"
                  scrolling="auto"            
                ></iframe>
              </div>

              <iframe
                id="myFrame5"
                title="resg"        
                //srcDoc=====使用率=== 
                src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=177" width="450" height="200" frameborder="0"&kiosk" `}
                style={{ width: '13%', border: '10px', height: '200px' }}
                sandbox="allow-same-origin allow-scripts allow-forms"
                scrolling="auto"            
              ></iframe>

              <iframe
                id="myFrame6"
                title="resg"        
                //srcDoc=====硬盘=== 
                src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=181" width="250" height="200" frameborder="0"&kiosk" `}
                style={{ width: '67%', border: '10px', height: '200px' }}
                sandbox="allow-same-origin allow-scripts allow-forms"
                scrolling="auto"            
              ></iframe>
              <iframe
                id="myFrame7"
                title="resg"        
                //srcDoc=====CPU使用率=== 
                src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=7" width="450" height="200" frameborder="0"&kiosk" `}
                style={{ width: '50%', border: '10px', height: '300px' }}
                sandbox="allow-same-origin allow-scripts allow-forms"
                scrolling="auto"            
              ></iframe>
              <iframe
                id="myFrame8"
                title="resg"        
                //srcDoc=====内存信息=== 
                src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=156" width="450" height="200" frameborder="0"&kiosk" `}
                style={{ width: '50%', border: '10px', height: '300px' }}
                sandbox="allow-same-origin allow-scripts allow-forms"
                scrolling="auto"            
              ></iframe>
              <iframe
                id="myFrame9"
                title="resg"        
                //srcDoc=====每小时流量ens33=== 
                src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=183" width="450" height="200" frameborder="0"&kiosk" `}
                style={{ width: '50%', border: '10px', height: '300px' }}
                sandbox="allow-same-origin allow-scripts allow-forms"
                scrolling="auto"            
              ></iframe>
              <iframe
                id="myFrame10"
                title="resg"        
                //srcDoc=====每秒网络带宽使用=== 
                src={`http://${ip}:3002/d-solo/9CWBzd1f0bik001/linux?orgId=1&refresh=15s&theme=light&panelId=157" width="450" height="200" frameborder="0"&kiosk" `}
                style={{ width: '50%', border: '10px', height: '300px' }}
                sandbox="allow-same-origin allow-scripts allow-forms"
                scrolling="auto"            
              ></iframe>
            </div>
          </div>)} 
         {/* =====资源监控 end===*/}
      </div>
    )
  }
}



export default Document;
