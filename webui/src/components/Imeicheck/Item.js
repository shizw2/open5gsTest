import { Component } from 'react';
import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media } from 'helpers/style-utils';

import EditIcon from 'react-icons/lib/md/edit';
import DeleteIcon from 'react-icons/lib/md/delete';

import { Tooltip, Spinner } from 'components';
import { getStatusName, getReasonName } from './getName';
const Card = styled.div`
  position: relative;
  display: flex;
  padding : 0.2rem 0.5rem 0.2rem 0.5rem; 

  cursor: pointer;

  ${p => p.disabled && 'opacity: 0.5; cursor: not-allowed; pointer-events: none;'}

  .actions {
    position: absolute;
    top: 0;
    right: 0;
    width: 8rem;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;

    opacity: 0;
  }

  &:hover {
    background: ${oc.gray[1]};

    .actions {
      ${p => p.disabled ? 'opacity: 0;' : 'opacity: 1;'};
    }
  }
`;

const CircleButton = styled.div`
  height: 2rem;
  width: 2rem;
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 1px 4px;

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

const Imeicheck = styled.div`
  display: flex;
  flex-direction: row;
  flex:1;
  line-height: 2.5rem;
  margin : 0 2rem;
  .title {
    font-size: 1.25rem;
    color: ${oc.gray[8]};
    width: 320px;
  }
  .index {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 110px;
  }
  .imei {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 220px;
  }
  .status {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 160px;
  }
  .reason {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 160px;
  }
  .checkflag {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 160px;
  }
  .imsi {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 300px;
  }
`;

const SpinnerWrapper = styled.div`
  position: absolute;
  top: 0;
  right: 0;
  width: 4rem;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
`
const Scroll = styled.div`
  &.fixed-background {
    /* 添加滚动时需要的样式 */
    background-color: ${oc.gray[1]};   
  }
`;
const propTypes = {
  imeicheck: PropTypes.shape({
    title: PropTypes.string
  }),
  onView: PropTypes.func,
  onEdit: PropTypes.func,
  onDelete: PropTypes.func
}

class Item extends Component {
  constructor(props) {
    super(props);
    this.itemRef = null;
    this.state = {
      isHovered: false
    };
  }

  componentDidMount() {
    window.addEventListener('scroll', this.handleScroll); // 添加滚动事件监听器
  }

  componentWillUnmount() {
    window.removeEventListener('scroll', this.handleScroll); // 移除滚动事件监听器
  }
  
  handleScroll = () => {
    const item = this.itemRef;
    if (item && item.getBoundingClientRect().top < window.innerHeight) {
      this.setState({ isHovered: true });
    } else {
      this.setState({ isHovered: false });
    }
  }
  static propTypes = {
    imeicheck: PropTypes.shape({
      title: PropTypes.string
    }),
    onView: PropTypes.func,
    onEdit: PropTypes.func,
    onDelete: PropTypes.func
  }

  handleEdit = e => {
    e.stopPropagation();

    const {
      imeicheck,
      onEdit,
    } = this.props;

    const {
      imei
    } = imeicheck;

    onEdit(imei)
  }

  handleDelete = e => {
    e.stopPropagation();

    const {
      imeicheck,
      onDelete
    } = this.props;

    const {
      imei
    } = imeicheck;
    onDelete(imei)
  }

  render() {
    const {
      handleEdit,
      handleDelete,

    } = this;
    const { isHovered } = this.state;
    const {
      disabled,
      imeicheck,
      onView,
      onEdit,
      onDelete,
      index
    } = this.props;

    const {
      imei,      
      status,
      reason,
      checkflag,
      bindimsi
    } = imeicheck;
    const imsiList = bindimsi.map(item => item.imsi);
    const joinedImsi = imsiList.join("-");
    return (
      <Card disabled={disabled} onClick={() => onView(imei)}>
        <Scroll // 使用 Scroll 组件来包装 Oprlog 组件
          ref={(element) => (this.itemRef = element)}
          className={`scroll ${isHovered ? 'fixed-background' : ''}`}
          onMouseEnter={() => this.setState({ isHovered: true })}
          onMouseLeave={() => this.setState({ isHovered: false })}
        >
          <Imeicheck>
            <div className="index">{index}</div>          
            <div className="imei">{imei}</div>
            <div className="status">{getStatusName(status)}</div>
            <div className="reason">{getReasonName(reason)}</div>
            <div className="checkflag">{checkflag?"是":"否"}</div>
            <div className="imsi">{joinedImsi}</div>
          </Imeicheck>
        </Scroll>   
        <div className="actions">
          <Tooltip content='Edit' width="60px">
            <CircleButton onClick={handleEdit}><EditIcon/></CircleButton>
          </Tooltip>
          <Tooltip content='Delete' width="60px">
            <CircleButton className="delete" onClick={handleDelete}><DeleteIcon/></CircleButton>
          </Tooltip>
        </div>
        {disabled && <SpinnerWrapper><Spinner sm/></SpinnerWrapper>}
      </Card>
    )
  }
}

export default Item;
