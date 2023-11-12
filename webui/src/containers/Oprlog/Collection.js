import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';
import { MODEL, Ommlog } from 'modules/crud/ommlog';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';
import styled from 'styled-components';
import oc from 'open-color';
import { 
  Layout, 
  Oprlog,
  Spinner,   
  Blank,
  Dimmed,
  Confirm
} from 'components';

import Document from './Document';

const Tabaleheadf =styled.div`
.tabalehead-wrapper {
  position: relative;
}
`
const Tabalehead = styled.div`
  display: flex;
  flex-direction: row;
  flex:1;
  line-height: 2.5rem;
  margin : 1rem 4rem;
  background: ${oc.gray[2]};
  position: sticky;
  top: 0;
  left: 0;
  right: 0;
  z-index: 1;
  .opttime {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    width: 320px;
  }
  .opuser {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    width: 220px;
  }
  .optype {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    width: 120px;
  }
  .optcommand {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    width: 120px;
  }
  .optfm {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    width: 120px;
    margin : 0rem 0rem 0rem 1rem;
  }
`;
class Collection extends Component {
  state = {
    document: {
      action: '',
      visible: false,
      dimmed: false
    },
    confirm: {
      visible: false,
      _id: ''
    },
    view: {
      visible: false,
      disableOnClickOutside: false,
      _id: ''
    }
  };

  componentWillMount() {
    const { oprlogs, dispatch } = this.props

    if (oprlogs.needsFetch) {
      dispatch(oprlogs.fetch)
    }
  }

  componentWillReceiveProps(nextProps) {
    const { oprlogs, status } = nextProps
    const { dispatch } = this.props

    if (oprlogs.needsFetch) {
      dispatch(oprlogs.fetch)
    }

    if (status.error) {
      let title = 'Unknown Code';
      let message = 'Unknown Error';
      if (response.data && response.data.name && response.data.message) {
        title = response.data.name;
        message = response.data.message;
      } else {
        title = response.status;
        message = response.statusText;
      }
      dispatch(clearActionStatus(MODEL, 'delete'));
    }
  }

  documentHandler = {
    show: (action, payload) => {
      this.setState({
        document: {
          action,
          visible: true,
          dimmed: true,
          ...payload
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: true
        }
      })
    },
    hide: () => {
      this.setState({
        document: {
          action: '',
          visible: false,
          dimmed: false
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: false
        }
      })
    },
    actions: {
      create: () => {
        this.documentHandler.show('create');
      },
      update: (_id) => {
        this.documentHandler.show('update', { _id });
      }
    }
  }

  confirmHandler = {
    show: (_id) => {
      this.setState({
        confirm: {
          visible: true,
          _id,
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: true
        }
      })
    },
    hide: () => {
      this.setState({
        confirm: {
          ...this.state.confirm,
          visible: false
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: false
        }
      })
    }
    
  }

  viewHandler = {
    show: (_id) => {
      this.setState({
        view: {
          _id,
          visible: true,
          disableOnClickOutside: false
        }
      });
    },
    hide: () => {
      this.setState({
        view: {
          ...this.state.view,
          visible: false
        }
      })
    }
  }


  render() {
    const {
      documentHandler,
      viewHandler,
      confirmHandler
    } = this;

    const { 
      document
    } = this.state;

    const { 
      oprlogs,
      status
    } = this.props

    const {
      isLoading,
      data
    } = oprlogs;

const styles = {
  underline: {
    width: '100%',
    height: '2px',    
    backgroundColor: '#5C7CFA',
  },
  topm: {
    margin: '0px 30px 20px 10px',
  }
}
return (  
  <div>                   
    <Tabalehead>
      <div className="optfm">序号</div>
      <div className="opttime">操作时间</div>
      <div className="opuser">操作账号</div>
      <div className="optype">操作类型</div>          
      <div className="optfm">功能模块</div>               
    </Tabalehead>                       
  <Layout.Content style={styles.topm}>              
    <Oprlog.List         
      oprlogs={data}          
      onView={viewHandler.show}
    />
    {isLoading && <Spinner md />}      
    <Oprlog.View
      visible={this.state.view.visible}
      oprlog={data.filter(oprlog => 
      oprlog._id === this.state.view._id)[0]}
      disableOnClickOutside={this.state.view.disableOnClickOutside}
      onHide={viewHandler.hide}/>
    <Document 
      { ...document }
      onHide={documentHandler.hide} />
    <Dimmed visible={document.dimmed} />           
  </Layout.Content>
  </div>
)
}
}

Collection = connect( 
  (state) => ({ 
    oprlogs: select(Ommlog.fetchOmmlogs({}), state.crud),
    status: selectActionStatus(MODEL, state.crud, 'delete')
  })
)(Collection);

export default Collection;
