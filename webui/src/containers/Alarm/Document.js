import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';
import NProgress from 'nprogress';

import { Alarm } from 'components';
import traverse from 'traverse';
import crypto from 'crypto';

const formData = {
  "roles": [ "user" ],
}

//const html = '<!DOCTYPE html><html><head><meta charset="utf-8"><title>当前告警</title></head><body><iframe src="http://192.168.6.200:3002/d/eea-9_sik/prometheus-alerts?orgId=1&refresh=5s&theme=light&kiosk" width="100%" height="1000" frameborder="0"></iframe></body></html>'
//const ip = window.location.hostname
const os = require('os')
const ip = os.hostname()

const html2=`http://${ip}:3002/d-solo/eea-9_sik/prometheus-alerts?orgId=1&refresh=5s&panelId=442&theme=light&kiosk`
const html1=`http://${ip}:3002/d-solo/QIA3UR57z/mynode?orgId=1&refresh=1m&theme=light&panelId=2&kiosk`

class Document extends Component {
  constructor(props) {
    super(props)
    this.state = {
      selectedOption: '90d',
      togglePosition: 'left'
    }

  }
  handleOptionChange = (event) => {
    this.setState({ selectedOption: event.target.value });
  }
  handleToggleChange = () => {
    const { togglePosition } = this.state;
    const newTogglePosition = togglePosition === 'left' ? 'right' : 'left';
    this.setState({ togglePosition: newTogglePosition });
     // 获取父窗口中的 iframe 元素
    const iframe = window.parent.document.getElementById('myIframe');

    // 根据开关位置设置 iframe 中的 src
    if (newTogglePosition === 'left') {
      iframe.src = html2
    } else {
      iframe.src = html1
    }
  }
  static propTypes = {
    action: PropTypes.string,
    visible: PropTypes.bool, 
    onHide: PropTypes.func
  }

  state = {
    formData
  }

  

  render(){    
    const { selectedOption,togglePosition} = this.state
    const fromValue = `now-${selectedOption.toLowerCase()}`
    return (     
    <div>
    {/*实时告警统计*/ }   
     <div style={{ display: 'flex', justifyContent: 'space-between' }}> 
      <div style={{ position: 'relative',width: '60%', height: '350px' }}>
        <div
          style={{
            position: 'absolute',
            top: '10px',
            right: '10px',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'left',
            width: '40px',
            height: '20px',
            borderRadius: '10px',
            background: '#3D71D9',
            cursor: 'pointer',
            zIndex: 9999 // 确保开关位于 iframe 上层
          }}
        onClick={this.handleToggleChange}        
        >
        <div
          style={{
            width: '16px',
            height: '16px',
            borderRadius: '50%',
            background: '#fff',
            transform: `translateX(${togglePosition === 'left' ? '0px' : '20px'})`,
            transition: '.2s ease'
          }}
        ></div>
       </div>
        <iframe
          id="myIframe"
          title="实时告警"
          src={html2}
          style={{ width: '100%', border: '0px', height: '100%', marginLeft: 'auto', marginRight: 'auto' }}
          sandbox="allow-same-origin allow-scripts allow-forms"
          scrolling="auto"          
        ></iframe>
      </div>
        <iframe
        title="告警柱状图"       
        src={`http://${ip}:3002/d-solo/eea-9_sik/prometheus-alerts?orgId=1&refresh=5s&panelId=443&theme=light&kiosk"`}     
        style={{ width: '38%', border: '0px', height: '350px' , float:"right"}}
        andbox="allow-same-origin allow-scripts  allow-forms"
        scrolling="auto"        
        ></iframe>       
        </div>
        {/*历史告警统计*/ }        
        <div style={{ display: 'flex', justifyContent: 'space-between' }}>
          <div style={{ position: 'relative', width: '100%' }}>
            <iframe
              title="重启以来的历史告警" 
              src={`http://${ip}:3002/d-solo/eea-9_sik/prometheus-alerts?orgId=1&refresh=5s&panelId=414&from=${fromValue}&to=now&theme=light&kiosk" `}              
              style={{
                width: '100%',
                border: '10px',
                height: '300px',
                fontWeight: 'bold',
                marginLeft: '0'
              }}
              sandbox="allow-same-origin allow-scripts  allow-forms"
              scrolling="auto"
            ></iframe>
            <div style={{ position: 'absolute', top: '10px', right: '10px'}}>
              <form id="myForm" style={{ border: '1px solid #ccc', borderRadius: '5px', padding: '3px' }}>
              <select
                  value={selectedOption}
                  onChange={this.handleOptionChange}
                  style={{ 
                    border: 'none', 
                    outline: 'none', 
                    background: 'transparent', 
                    fontFamily: 'Inter, Helvetica, Arial, sans-serif', 
                    fontSize: '0.5rem', 
                    fontWeight: 500 ,
                    color: '#24292E',
                    paddingTop: '4px',
                    paddingBottom: '4px' 
                    }}>
                  <option value="1d">1天</option>
                  <option value="7d">一周</option>
                  <option value="30d">一个月</option>
                  <option value="90d">三个月</option>
                  <option value="180d">半年</option>
                </select>
              </form>
            </div>
          </div>
        </div>
       </div>
        
    )
  }
}



export default Document;
