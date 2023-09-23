import { Component } from 'react';
import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media } from 'helpers/style-utils';

import EditIcon from 'react-icons/lib/md/edit';
import DeleteIcon from 'react-icons/lib/md/delete';

import { Tooltip, Spinner } from 'components';

const Sizer = styled.div`
  display: inline-block;
  width: 33.3%;
  padding: 0.5rem;

  ${p => p.disabled && 'opacity: 0.5; cursor: not-allowed;'};

  ${media.desktop`
    width: 50%;
  `}

  ${media.tablet`
    width: 100%;
  `}
`;

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

const NFConfig = styled.div`
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
  .ambr {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 240px;
  }
  .name {
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
  nfconfig: PropTypes.shape({
    title: PropTypes.string
  }),
  onView: PropTypes.func,
  onEdit: PropTypes.func,
  onDelete: PropTypes.func
}

class Item extends Component {
  static propTypes = {
    nfconfig: PropTypes.shape({
      title: PropTypes.string
    }),
    onView: PropTypes.func,
    onEdit: PropTypes.func,
    onDelete: PropTypes.func
  }

  handleEdit = e => {    
    e.stopPropagation();

    const {
      nfconfig,
      onEdit,
    } = this.props;

    const {
      _id
    } = nfconfig;
    console.log("handleEdit:",_id)
    onEdit(_id)
  }

  handleDelete = e => {
    e.stopPropagation();

    const {
      nfconfig,
      onDelete
    } = this.props;

    const {
      _id
    } = nfconfig;

    onDelete(_id)
  }

  render() {
    const {
      handleEdit,
      handleDelete
    } = this;
    
    const {
      disabled,
      nfconfig,
      onView,
      onEdit,
      onDelete
    } = this.props;

    const {
      _id,
      title,
      slice,
      ambr
    } = nfconfig;

    return (
      <Sizer disabled={disabled}>
          <Card disabled={disabled} onClick={() => onView(_id)}>
            <NFConfig>
              <div className="title">{_id}</div>          
            </NFConfig>
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
      </Sizer>
    )
  }
}

export default Item;
