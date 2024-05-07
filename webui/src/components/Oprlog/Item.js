import React, { Component } from 'react';
import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media } from 'helpers/style-utils';

import EditIcon from 'react-icons/lib/md/edit';
import DeleteIcon from 'react-icons/lib/md/delete';

import { Tooltip, Spinner } from 'components';

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
const Scroll = styled.div`
  &.fixed-background {
    /* 添加滚动时需要的样式 */
    background-color: ${oc.gray[1]};   
  }
`;
const Oprlog = styled.div`
  display: flex;
  flex-direction: row;
  flex:1;
  line-height: 2.5rem;
  margin : 0 4.1rem 0rem 2rem;

  .opttime {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 320px;
  }
  .opuser {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 220px;
  }
  .optype {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 220px;
  }
  .optcommand {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 120px;
  }
  .optfm {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 120px;
  }
  .optorder {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 120px;    
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

const propTypes = {
  oprlog: PropTypes.shape({
    oprtime: PropTypes.string
  }),
  onView: PropTypes.func
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
    oprlog: PropTypes.shape({
      oprtime: PropTypes.string
    }),
    onView: PropTypes.func
  }

  

  render() {
    const {
      
    } = this;
    
    const {
      disabled,
      oprlog,
      onView,
      index
    } = this.props;

    const {
      _id,
      opuser,
      optype,
      optcommand,
      optfm,
      opttime
    } = oprlog;
    const { isHovered } = this.state;
    return (
      <Card disabled={disabled} onClick={() => onView(_id)}>
        <Scroll // 使用 Scroll 组件来包装 Oprlog 组件
          ref={(element) => (this.itemRef = element)}
          className={`scroll ${isHovered ? 'fixed-background' : ''}`}
          onMouseEnter={() => this.setState({ isHovered: true })}
          onMouseLeave={() => this.setState({ isHovered: false })}
        >
        <Oprlog>
          <div className="optorder">{index}</div>
          <div className="opttime">{opttime}</div>
          <div className="opuser">{opuser}</div>          
          <div className="optype" style={{ color: optype.includes("警示") ? "red" : "" }}>{optype}</div>           
          <div className="optfm">{optfm}</div>           
        </Oprlog>        
        </Scroll>       
        {disabled && <SpinnerWrapper><Spinner sm/></SpinnerWrapper>}
      </Card>      
    )
  }
}

export default Item;
