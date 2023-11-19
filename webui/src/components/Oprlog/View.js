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

const Oprlog = styled.div`
  
  line-height: 2.5rem;
  margin : 0 2rem;

  .opttime {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .opuser {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .optype {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .optcommand {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
  .optfm {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 100%;
  }
`;

const View = ({ visible, disableOnClickOutside, oprlog , onHide }) => {
  const _id = (oprlog || {})._id;
  const opttime = (oprlog || {}).opttime;
  const opuser =(oprlog || {}).opuser;
  const optype =(oprlog || {}).optype;
  const optfm =(oprlog || {}).optfm;
  const optcommand =(oprlog || {}).optcommand;
  return (
    <div>
      <Modal
        visible={visible}
        onOutside={onHide}
        disableOnClickOutside={disableOnClickOutside}>
        <Wrapper>
          <Header>
            <div className="title">操作详情</div>
            <div className="actions">             
              <Tooltip content='Close' width="60px">
                <CircleButton className="delete" onClick={onHide}><CloseIcon/></CircleButton>
              </Tooltip>
            </div>
          </Header>
          <Body>
            <Oprlog>
              <div className="opttime">操作时间：{opttime}</div>
              <div className="opuser">操作账号：{opuser}</div>
              <div className="optype">操作类型：{optype}</div>          
              <div className="optfm">功能模块：{optfm}</div> 
              <div className="optcommand">{optcommand}</div>
            </Oprlog>
            
          </Body>
        </Wrapper>
      </Modal>
      <Dimmed visible={visible}/>
    </div>
  )
}

export default View;
