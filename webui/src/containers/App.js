import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';
import { bindActionCreators, compose } from 'redux';

import * as sidebarActions from 'modules/sidebar';
import withWidth, { SMALL } from 'helpers/with-width';

import { Layout } from 'components';
import Notification from 'containers/Notification';
import * as Subscriber from 'containers/Subscriber';
import * as Profile from 'containers/Profile';
import * as Account from 'containers/Account';
import * as Alarm from 'containers/Alarm';
import * as Performance from 'containers/Performance';
import * as Oprlog from 'containers/Oprlog';
import * as NFConfig from 'containers/NFConfig';
import * as Imeicheck from 'containers/Imeicheck';
import * as Dynmanage from 'containers/Dynmanage';
import { Ommlog } from 'modules/crud/ommlog';
import React from 'react';
import Logout from 'components/Base/Logout';
import Session from 'modules/auth/session';
import properties from '../../properties' // 导入配置文件

class App extends Component {
  static propTypes = {
    session: PropTypes.object.isRequired,
    view: PropTypes.string.isRequired,
    width: PropTypes.number.isRequired
  }
  timeoutId = null;

  handleLogout = async(createOmmlog) => {
    // 执行退出登录操作
    // ...
      createOmmlog('logout', '账号管理', {}, {}, "安全登出"); 
      const session = new Session()
      await session.signout()  
  
      // @FIXME next/router not working reliably so using window.location
      window.location = '/'

  };
  resetTimer = () => {
    clearTimeout(this.timeoutId);
    this.startTimer();
  };

  startTimer = () => {
    const { createOmmlog } = this.props; 
    this.timeoutId = setTimeout(() => {
      // 在定时器触发时执行handleLogout函数      
      if(properties.monitor===0)this.handleLogout(createOmmlog);//如果需要长期监控则不执行退出。
    }, 300000); // 设置5分钟的定时器，超过5分钟未操作则执行退出登录操作
  };
  componentDidMount() {
    const { 
      width,
      SidebarActions
    } = this.props;

    if (width !== SMALL) {
      SidebarActions.setVisibility(true);
    }

    window.addEventListener('mousemove', this.resetTimer);
    window.addEventListener('keydown', this.resetTimer);
    this.startTimer();
  }
  componentWillUnmount() {
    clearTimeout(this.timeoutId);
    window.removeEventListener('mousemove', this.resetTimer);
    window.removeEventListener('keydown', this.resetTimer);
  }
  render() {
    const {
      view,
      session
    } = this.props;

    if (view === "subscriber") {
      document.body.style.backgroundColor = "#e9ecef";
    } else {
      document.body.style.backgroundColor = "white";
    }

    return (
      <div>
      <Layout>
        <Layout.Container visible={view === "subscriber"}>
          <Subscriber.Collection/>
        </Layout.Container>
        <Layout.Container visible={view === "profile"}>
          <Profile.Collection/>
        </Layout.Container>
        <Layout.Container visible={view === "account"}>
          <Account.Collection session={session}/>
        </Layout.Container>
        <Layout.Container visible={view === "nfconfig"}>
          <NFConfig.Collection/>
        </Layout.Container>       
        <Layout.Container visible={view === "alert"}>
          <Alarm.Collection/>        
        </Layout.Container>
        <Layout.Container visible={view === "performance"}> 
          <Performance.Collection/>       
        </Layout.Container>
        <Layout.Container visible={view === "oprlog"}> 
          <Oprlog.Collection/>       
        </Layout.Container>
        <Layout.Container visible={view === "imeicheck"}> 
          <Imeicheck.Collection/>       
        </Layout.Container>
        <Layout.Container visible={view === "dynmanage"}> 
          <Dynmanage.Collection/>       
        </Layout.Container>
        <Notification/>
        <Ommlog session={session}/>
      </Layout>
      <Logout onLogout={this.handleLogout} />
      </div>
    )
  }
}

const enhance = compose(
  withWidth(),
  connect(
    (state) => ({
      view: state.sidebar.view
    }),
    (dispatch) => ({
      SidebarActions: bindActionCreators(sidebarActions, dispatch),
      createOmmlog: (type, category, data1, data2, username) =>
    dispatch(Ommlog.createOmmlog(type, category, data1, data2, username))
    })
    
  )
);

export default enhance(App);
