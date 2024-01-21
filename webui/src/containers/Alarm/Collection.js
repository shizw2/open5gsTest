import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

//import { MODEL, fetchAccounts, deleteAccount } from 'modules/crud/account';
//import { clearActionStatus } from 'modules/crud/actions';
//import { select, selectActionStatus } from 'modules/crud/selectors';
//import * as Notification from 'modules/notification/actions';

import { 
  Layout, 
  Alarm, 
  Spinner,    
  Blank,
  
  Confirm
} from 'components';

import Document from './Document';

class Collection extends Component {
  state = {
    document: {
      action: '',
      visible: false,
      dimmed: false
    },
    confirm: {
      visible: false,
      username: ''
    },
  }

  

  render() {
   
    return (
      <Layout.Content>     
        <Document 
          { ...document }          
          />        
      </Layout.Content>
    )
  }
}



export default Collection;
