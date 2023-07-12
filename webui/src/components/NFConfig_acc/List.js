import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media } from 'helpers/style-utils';

import { Layout, Blank } from 'components';
import Item from './Item';

const Wrapper = styled.div`
  display: block;
  margin: 2rem;
  
  ${media.mobile`
    margin: 0.5rem 0.25rem;
  `}
`

const propTypes = {
  nfconfigs: PropTypes.arrayOf(PropTypes.object),
  onEdit: PropTypes.func,
  onDelete: PropTypes.func,
}

const List = ({ nfconfigs, deletedId, onEdit, onDelete, session }) => {
  const {
    username,
    roles
  } = session.user;

  const nfconfigList = nfconfigs
    .map(nfconfig =>
      <Item 
        key={nfconfig.username}
        session={session}
        nfconfig={nfconfig}
        disabled={deletedId === nfconfig.username || (roles.indexOf('admin') === -1 && nfconfig.username !== username)}
        spinner={deletedId === nfconfig.username}
        onEdit={onEdit}
        onDelete={onDelete} />
    );

  return (
    <Wrapper>
      {nfconfigList}
    </Wrapper>
  )
}

List.propTypes = propTypes;

export default List;
