import PropTypes from 'prop-types';

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
    width: 8rem;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
`;

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
  
`


const ViewBSF = ({ visible, disableOnClickOutside, nfconfig, onEdit, onDelete, onHide }) => {
  const _id = (nfconfig || {})._id;
  const title = (nfconfig || {}).title;
  const msisdn_list = ((nfconfig || {}).msisdn || []);
  const imeisv = (nfconfig || {}).imeisv;


  return (
    <div>
      <Modal
        visible={visible}
        onOutside={onHide}
        disableOnClickOutside={disableOnClickOutside}>
        <Wrapper>
          <Header>
            <div className="title">{title}</div>
            <div className="actions">
              <Tooltip content='Edit' width="60px">
                <CircleButton onClick={() => onEdit(_id)}><EditIcon/></CircleButton>
              </Tooltip>
              <Tooltip content='Delete' width="60px">
                <CircleButton className="delete" onClick={() => onDelete(_id)}><DeleteIcon/></CircleButton>
              </Tooltip>
              <Tooltip content='Close' width="60px">
                <CircleButton className="delete" onClick={onHide}><CloseIcon/></CircleButton>
              </Tooltip>
            </div>
          </Header>
          <Body>
            <NFConfig>
              <div className="header">
                NFConfig Configuration
              </div>
              <div className="sectionbody">
                <div className="sectioncolumn">
                  {(msisdn_list.length !== 0 || (imeisv && imeisv.length !== 0)) &&
                    <div className="body">
                      <div className="left">
                        <PhoneIcon/>
                      </div>
                      <div className="right">
                        {msisdn_list.map((msisdn, index) =>
                          <div key={index} className="data">
                            {msisdn}
                            <span style={{color:oc.gray[5]}}><KeyboardControlIcon/>MSISDN</span>
                          </div>
                        )}
                        {imeisv && imeisv.length !== 0 &&
                          <div className="data">
                            {imeisv}
                            <span style={{color:oc.gray[5]}}><KeyboardControlIcon/>IMEISV</span>
                          </div>
                        }
                      </div>
                    </div>
                  }
                  {/* 添加logger属性的渲染 */}
                  {nfconfig && nfconfig.logger && (
                    <div className="logger-section">
                      <div className="logger-header">logger:</div>
                      <div className="logger-body">
                        <div className="eight-spaces">
                          file: {nfconfig.logger.file}
                        </div>
                        <div className="eight-spaces">
                          level: {nfconfig.logger.level}
                        </div>
                      </div>
                    </div>
                  )}
                  
                  {/* 添加 nrf 属性的渲染 */}
                  {nfconfig && nfconfig.nrf && (
                    <div className="nrf-section">
                      <div className="nrf-header">nrf:</div>
                      <div className="nrf-body">
                  
                        {/* 添加 sbi 属性的渲染 */}
                        {nfconfig.nrf.sbi && (
                          <div className="sbi-section">
                            <div className="sbi-header">&nbsp;&nbsp;&nbsp;&nbsp;sbi:</div>
                            <div className="sbi-body">
                  
                              {/* 添加 addr 属性的渲染 */}
                              {nfconfig.nrf.sbi.addr && nfconfig.nrf.sbi.addr.map((item, index) => (
                                <div className="addr-data" key={index}>
                                  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;addr: {item}
                                </div>
                              ))}
                  
                              {/* 添加 port 属性的渲染 */}
                              {nfconfig.nrf.sbi.port && (
                                <div className="port-data">
                                  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;port: {nfconfig.nrf.sbi.port}
                                </div>
                              )}
                  
                            </div>
                          </div>
                        )}
                  
                      </div>
                    </div>
                  )}
              
                </div>                
              </div>
            </NFConfig>            
          </Body>
        </Wrapper>
      </Modal>
      <Dimmed visible={visible}/>
    </div>
  )
}

export default ViewBSF;
