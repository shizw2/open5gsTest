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

const Imeickeck = styled.div`
  display: flex;
  flex-direction: column;
  margin: 0, auto;
  color: ${oc.gray[9]};

  .header {
    margin: 12px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .imei-label {
    font-weight: bold;    
  }
  .imei {
    margin: 10px 0px 0px 85px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .status {
    margin: 10px 0px 0px 85px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .reason {
    margin: 10px 0px 0px 85px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .checkflag {
    margin: 10px 0px 0px 15px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .bindimsi {
    margin: 10px 0px 0px 30px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .imsi {
    margin: 10px 0px 0px 100px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .msisdn {
    margin: 10px 0px 0px 85px;
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  }`


const View = ({ visible, disableOnClickOutside, imeicheck, onEdit, onDelete, onHide }) => {
  const _id = (imeicheck || {})._id;
  const imei = (imeicheck || {}).imei;
  const status = (imeicheck || {}).status;
  const reason = (imeicheck || {}).reason;
  const checkflag = (imeicheck || {}).checkflag;  
  const bindimsi_list = ((imeicheck || {}).bindimsi || []);  
  const renderBindimsi_list = bindimsi_list.map((item, index) => (
    <div className="imsi" key={index}>      
      <span style={{ width: '30%', display: 'inline-block' }}>IMSI: {item.imsi}</span>      
      <span >MSISDN: {item.msisdn}</span>
    </div>
  ));
  const  statusname=["白名单","黑名单","灰名单"] 
  const  reasonname=["","丢失","被偷","被抢","非法厂商","未知原因"] 
  return (
    <div>
      <Modal
        visible={visible}
        onOutside={onHide}
        disableOnClickOutside={disableOnClickOutside}>
        <Wrapper>
          <Header>
            <div className="title">{imei}</div>
            <div className="actions">
              <Tooltip content='Edit' width="60px">
                <CircleButton onClick={() => onEdit(imei)}><EditIcon/></CircleButton>
              </Tooltip>
              <Tooltip content='Delete' width="60px">
                <CircleButton className="delete" onClick={() => onDelete(imei)}><DeleteIcon/></CircleButton>
              </Tooltip>
              <Tooltip content='Close' width="60px">
                <CircleButton className="delete" onClick={onHide}><CloseIcon/></CircleButton>
              </Tooltip>
            </div>
          </Header>
          <Body>
            <Imeickeck>               
              <div className="imei">
                  <span className="imei-label">IMEI:  </span>
                  <span>{imei}</span>
              </div>
              <div className="status">
                  <span className="imei-label">状态:  </span>
                  <span>{statusname[`${status}`]}</span>
              </div>
              <div className="reason">
                  <span className="imei-label">原因:  </span>
                  <span>{reasonname[`${reason}`]}</span>
              </div>              
              <div className="checkflag">
                  <span className="imei-label">是否绑定IMSI:  </span>
                  <span>{checkflag?"是":"否"}</span>
              </div>   
              <div className="bindimsi">
                  <span className="imei-label">绑定的IMSI:  </span>
                  <span>{renderBindimsi_list}</span>
              </div>              
            </Imeickeck>           
          </Body>
        </Wrapper>
      </Modal>
      <Dimmed visible={visible}/>
    </div>
  )
}

export default View;
