import { Component } from 'react';


import { 
  Layout, 
  Alarm, 
  Spinner,    
  Blank,
  
  Confirm
} from 'components';

import DynoprButtons from './Dynopr';

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
        <DynoprButtons 
          { ...document }          
          />        
      </Layout.Content>
    )
  }
}



export default Collection;
