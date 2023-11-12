import { Component } from 'react';
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
  padding : 0.5rem;  
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
    margin: 0px 0px 0px 20px;
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

const Oprlog = styled.div`
  display: flex;
  flex-direction: row;
  flex:1;
  line-height: 2.5rem;
  margin : 0 2rem;

  .opttime {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 320px;
  }
  .opuser {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 240px;
  }
  .optype {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 120px;
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

    return (
      <Card disabled={disabled} onClick={() => onView(_id)}>
        <Oprlog>
          <div className="optfm">{index}</div>
          <div className="opttime">{opttime}</div>
          <div className="opuser">{opuser}</div>
          <div className="optype">{optype}</div>          
          <div className="optfm">{optfm}</div>           
        </Oprlog>        
        {disabled && <SpinnerWrapper><Spinner sm/></SpinnerWrapper>}
      </Card>      
    )
  }
}

export default Item;
