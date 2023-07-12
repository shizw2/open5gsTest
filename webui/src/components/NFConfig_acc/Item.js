import { Component } from 'react';
import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media  } from 'helpers/style-utils';

import EditIcon from 'react-icons/lib/md/edit';
import DeleteIcon from 'react-icons/lib/md/delete';

import { Tooltip, Spinner } from 'components';

const Card = styled.div`
  position: relative;
  display: flex;
  padding: 0.5rem;

  cursor: pointer;

  ${p => p.disabled && 'opacity: 0.5; cursor: not-allowed; pointer-events: none;'};

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

const NFConfig = styled.div`
  display: flex;
  flex-direction: row;
  flex:1;
  line-height: 2.5rem;
  margin : 0 2rem;

  .username {
    font-size: 1.25rem;
    color: ${oc.gray[8]};
    width: 320px;
  }
  .role {
    font-size: 1.1rem;
    color: ${oc.gray[6]};
    width: 240px;
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
    username: PropTypes.string
  }),
  onEdit: PropTypes.func,
  onDelete: PropTypes.func
}

class Item extends Component {
  static propTypes = {
    nfconfig: PropTypes.shape({
      username: PropTypes.string
    }),
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

    onEdit(_id)
  }

  handleDelete = e => {
    e.stopPropagation();

    const {
      nfconfig,
      onDelete
    } = this.props;

    const {
      username
    } = nfconfig;

    onDelete(username)
  }

  render() {
    const {
      handleEdit,
      handleDelete
    } = this;
    
    const {
      session,
      disabled,
      spinner,
      nfconfig,
      onEdit,
      onDelete
    } = this.props;

    return (
      <Card disabled={disabled} onClick={handleEdit}>
        <NFConfig>
          <div className='username'>{nfconfig.username}</div>
          <div className='role'>{nfconfig.roles[0]}</div>
        </NFConfig>
        {session.user.username !== nfconfig.username &&
          <div className="actions">
            <Tooltip content='Delete' width="60px">
              <CircleButton className="delete" onClick={handleDelete}><DeleteIcon/></CircleButton>
            </Tooltip>
          </div>}
        {spinner && <SpinnerWrapper><Spinner sm/></SpinnerWrapper>}
      </Card>
    )
  }
}

export default Item;
