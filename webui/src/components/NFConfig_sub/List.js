import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media, transitions } from 'helpers/style-utils';
import CSSTransitionGroup from 'react-transition-group/CSSTransitionGroup';

import { Layout, Blank } from 'components';
import Item from './Item';

const Wrapper = styled.div`
  display: block;
  margin: 1rem 0.5rem;
  
  ${media.mobile`
    margin: 0.5rem 0.25rem;
  `}

  .nfconfig-enter {
    animation: ${transitions.stretchOut} .3s ease-in;
    animation-fill-mode: forwards;
  }   
    
  .nfconfig-leave { 
    animation: ${transitions.shrinkIn} .15s ease-in;
    animation-fill-mode: forwards;
  }   
`

const propTypes = {
  nfconfigs: PropTypes.arrayOf(PropTypes.object),
  onView: PropTypes.func,
  onEdit: PropTypes.func,
  onDelete: PropTypes.func,
  search: PropTypes.string
}

const List = ({ nfconfigs, deletedImsi, onView, onEdit, onDelete, search }) => {
  const nfconfigList = nfconfigs
    .filter(s => s.imsi.indexOf(search) !== -1)
    .sort( 
      (a,b) => {
        if(a.imsi > b.imsi) return 1;
        if (a.imsi < b.imsi) return -1;
        return 0;
      }
    )
    .map(nfconfig =>
      <Item 
        key={nfconfig.imsi}
        nfconfig={nfconfig}
        disabled={deletedImsi === nfconfig.imsi}
        onView={onView}
        onEdit={onEdit}
        onDelete={onDelete} />
    );

  return (
    <Wrapper>
      <CSSTransitionGroup
        transitionName="nfconfig"
        transitionEnterTimeout={300}
        transitionLeaveTimeout={150}>
        {nfconfigList}
      </CSSTransitionGroup>
    </Wrapper>
  )
}

List.propTypes = propTypes;

export default List;