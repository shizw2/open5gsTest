import PropTypes, { string } from 'prop-types';
import { Component } from 'react';
import { connect } from 'react-redux';

import styled from 'styled-components';
import oc from 'open-color';
import { media } from 'helpers/style-utils';

import EditIcon from 'react-icons/lib/md/edit';
import DeleteIcon from 'react-icons/lib/md/delete';
import CloseIcon from 'react-icons/lib/md/close';

import PhoneIcon from 'react-icons/lib/md/phone';
import SecurityIcon from 'react-icons/lib/md/security';
import PdnIcon from 'react-icons/lib/md/cast';
import KeyboardControlIcon from 'react-icons/lib/md/keyboard-control';

import { Modal, Tooltip, Dimmed } from 'components';

import React, { useState } from 'react';

//import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';
//import 'react-tabs/style/react-tabs.css';
//import { Tabs, Tab } from '@material-ui/core';

import { ViewBSF } from './ViewNF/ViewBSF';
import { ViewNRF } from './ViewNF/ViewNRF';
import { ViewAMF } from './ViewNF/ViewAMF';
import { ViewAUSF } from './ViewNF/ViewAUSF';
import { ViewNSSF } from './ViewNF/ViewNSSF';
import { ViewPCF } from './ViewNF/ViewPCF';
import { ViewSCP } from './ViewNF/ViewSCP';
import { ViewSMF } from './ViewNF/ViewSMF';
import { ViewUDM } from './ViewNF/ViewUDM';
import { ViewUDR } from './ViewNF/ViewUDR';
import { ViewUPF } from './ViewNF/ViewUPF';
import { ViewEIR } from './ViewNF/ViewEIR';

const componentMap = {
  nrf: ViewNRF,
  bsf: ViewBSF,
  amf: ViewAMF,
  test: ViewAMF,
  ausf: ViewAUSF,
  nssf: ViewNSSF,
  pcf: ViewPCF,
  scp: ViewSCP,
  smf: ViewSMF,
  udm: ViewUDM,
  udr: ViewUDR,
  upf: ViewUPF,
  eir: ViewEIR,
  // 其他 nfconfig._id 对应的组件
};

const Wrapper = styled.div`
  display: flex;
  flex-direction: column;
  postion: relative;
  width: 900px;

  ${media.mobile`
    width: calc(100vw - 4rem);
  `}

  background: white;
  box-shadow: 0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23);
`

const Header = styled.div`
  position: relative;
  display: flex;

  background: ${oc.gray[1]};

  .title {
    padding: 1.5rem;
    color: ${oc.gray[8]};
    font-size: 1.5rem;
  }

  .actions {
    position: absolute;
    top: 0;
    right: 0;
    width: 6rem;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
`
const Button = styled.div`
  background-color: transparent;
  padding: 8px;
  margin-right: 0px;
  cursor: pointer;
  font-size: 16px;
  color: ${oc.gray[6]};
  &:hover {
    background-color: ${oc.indigo[3]};
  }
`

const CircleButton = styled.div`
  height: 2rem;
  width: 2rem;
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 1px;

  color: ${oc.gray[6]};

  border-radius: 1rem;
  font-size: 1.5rem;

  &:hover {
    color: ${oc.indigo[6]};
  }

  &.delete {
    &:hover {
      color: ${oc.pink[6]};
    }
  }
`
const Body = styled.div`
  display: block;
  margin: 0.5rem;

  height: 500px;
  ${media.mobile`
    height: calc(100vh - 16rem);
  `}

  overflow: scroll;
`

const NFConfig = styled.div`
  display: flex;
  flex-direction: column;
  margin: 0, auto;
  color: ${oc.gray[9]};

  .tabsMenu {
    position: relative;
    display: flex;
    justify-content: flex-start;
    //margin-top: 60px;
    background-color: ${oc.gray[3]};
    padding-rig: 0px; 
  }

  .header {
    margin: 12px;
    font-size: 16px;
  }
  .sectionbody {
    display: flex;
  }
  .sectioncolumn {
    flex: 1;
  }
  .body {
    display: flex;
    flex-direction: row;
    flex:1;
    margin: 6px;

    .left {
      width: 80px;
      text-align: center;
      font-size: 18px;
      color: ${oc.gray[6]};
    }

    .right {
      display: flex;
      flex-direction: column;
      flex:1;

      .data {
        flex:1;
        font-size: 12px;
        margin: 4px;
      }
    }
  }
  .four-spaces {
    padding-left: 4px;
  }
  
  .eight-spaces {
    padding-left: 8px;
  }
  
  .twelve-spaces {
    padding-left: 12px;
  }
  
  .twenty-spaces {
    padding-left: 20px;
  }
  .forty-spaces {
    padding-left: 40px;
  }
  .sixty-spaces {
    padding-left: 60px;
  }
  .eighty-spaces {
    padding-left: 80px;
  }
  
`
class View extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      selectedIns: 0,
      nf: '',
    };
    this.handleButtonClick = this.handleButtonClick.bind(this);
  }

  handleButtonClick(index, nfType) {
    this.setState({ selectedIns: index });
    this.setState({ nf: nfType });
  }

  render() {
    
    const { visible, disableOnClickOutside, nfconfigs, onEdit, onDelete, onHide } = this.props;

    const nfInsNum = (nfconfigs || {}).length;
    const nfconfig1 = (nfconfigs || {})[0];
    const _nf = (nfconfig1 || {})._nf;
    const nfname = String(_nf).toUpperCase();

    if (this.state.nf != _nf)
    {
      this.state.selectedIns = 0;
      this.state.nf = _nf;
    }
    
    const nfconfig = (nfconfigs || {})[this.state.selectedIns];
    const _id = (nfconfig || {})._id;

    //nfname = nfname.replace(/\d/g, "");

    return (
      <div>
        <Modal
          visible={visible}
          onOutside={onHide}
          disableOnClickOutside={disableOnClickOutside}>
          <Wrapper>
            <Header>
              <div className="title">{nfname} Configuration</div>
              <div className="actions">
                {nfconfig && (
                  <Tooltip content='Edit' width="60px">
                    <CircleButton className="edit" onClick={() => onEdit(_id)}>
                      <EditIcon/>
                    </CircleButton>
                  </Tooltip>
                )}
                {/*
                <Tooltip content='Delete' width="60px">
                  <CircleButton className="delete" onClick={() => onDelete(_id)}><DeleteIcon/></CircleButton>
                </Tooltip>
                */}
                <Tooltip content='Close' width="60px">
                  <CircleButton className="close" onClick={onHide}><CloseIcon/></CircleButton>
                </Tooltip>
              </div>
            </Header>
            {nfInsNum === 1 ? (
            <Body>
              <NFConfig>
                {nfInsNum === 1 && nfconfig && componentMap[_nf] && React.createElement(componentMap[_nf], { nfconfig })}
              </NFConfig>
            </Body>
            ) : (
            <Body style={{ overflow: 'hidden' }}>
              <NFConfig>
                {nfInsNum > 1 && (
                <div className="tabsMenu" style={{ position: 'fixed' }}>
                  <nav >
                    {/* 导航链接 */}
                    <button 
                      className={`nav-link ${this.state.selectedIns === 0  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 0  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(0, _nf)} 
                    >
                      {nfname} 1
                    </button>
                    <button 
                      className={`nav-link ${this.state.selectedIns === 1  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 1  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(1, _nf)} 
                    >
                      {nfname} 2
                    </button>
                    {nfInsNum > 2 && (
                    <button 
                      className={`nav-link ${this.state.selectedIns === 2  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 2  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(2, _nf)} 
                    >
                      {nfname} 3
                    </button>
                    )}
                    {nfInsNum > 3 && (
                    <button 
                      className={`nav-link ${this.state.selectedIns === 3  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 3  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(3, _nf)} 
                    >
                      {nfname} 4
                    </button>
                    )}
                    {nfInsNum > 4 && (
                    <button 
                      className={`nav-link ${this.state.selectedIns === 4  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 4  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(4, _nf)} 
                    >
                      {nfname} 5
                    </button>
                    )}
                    {nfInsNum > 5 && (
                    <button 
                      className={`nav-link ${this.state.selectedIns === 5  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 5  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(5, _nf)} 
                    >
                      {nfname} 6
                    </button>
                    )}
                    {nfInsNum > 6 && (
                    <button 
                      className={`nav-link ${this.state.selectedIns === 6  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 6  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(6, _nf)} 
                    >
                      {nfname} 7
                    </button>
                    )}
                    {nfInsNum > 7 && (
                    <button 
                      className={`nav-link ${this.state.selectedIns === 7  ? 'selected' : ''}`}
                      style={{
                        border: 'none',
                        padding: '10px',
                        backgroundColor: this.state.selectedIns === 7  ? '#5C7CFA' : 'lightgray',
                        color: 'white'
                      }}
                      onClick={() => this.handleButtonClick(7, _nf)} 
                    >
                      {nfname} 8
                    </button>
                    )}
                  </nav>
                  {/*
                    <Button className={activeTab === 0 ? 'selected' : ''} >{nfname} 1</Button>
                    <Button className={activeTab === 1 ? 'selected' : ''} >{nfname} 2</Button>
                    <Button className={activeTab === 2 ? 'selected' : ''} >{nfname} 3</Button>
                  */}
                </div>
                )}
                {/* 根据不同的情况渲染不同的组件 */}
                {nfInsNum > 1 && (
                <div className="tab-content" style={{ marginTop: '50px', height: 'calc(100vh - 280px)', overflow: 'auto' }}>
                  {nfconfig && componentMap[_nf] && React.createElement(componentMap[_nf], { nfconfig })}
                </div>
                )}
              </NFConfig>            
            </Body>
            )}
          </Wrapper>
        </Modal>
        <Dimmed visible={visible}/>
      </div>
    )
  }
}

export default View;
