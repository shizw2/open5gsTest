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
  accounts: PropTypes.arrayOf(PropTypes.object),
  onEdit: PropTypes.func,
  onDelete: PropTypes.func,
}

const List = ({ accounts, deletedId, onEdit, onDelete, session }) => {
  const {
    username,
    roles
  } = session.user;

  const accountList = accounts
    .map(account =>
      <Item 
        key={account.username}
        session={session}
        account={account}
        disabled={deletedId === account.username || (roles.indexOf('admin') === -1 && account.username !== username)}
        spinner={deletedId === account.username}
        onEdit={onEdit}
        onDelete={onDelete} />
    );

  return (
    /*
    <Wrapper>
      {accountList}
    </Wrapper>
    */
    <iframe
    title="resg"
    //srcDoc={html}      
    src={"http://192.168.6.200:3002/d/eea-9_sik/prometheus-alerts?orgId=1&refresh=5s&theme=light&viewPanel=442"}
    style={{ width: '100%', border: '0px', height: '1000px' }}
    andbox="allow-same-origin allow-scripts allow-popups allow-forms"
    scrolling="auto"
    ></iframe>
  )
}

List.propTypes = propTypes;

export default List;
