import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import { MODEL, fetchNFConfigs, deleteNFConfig } from 'modules/crud/nfconfig';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';
import ArchitectureDiagram from './ArchitectureDiagram';

import { 
  Layout, 
  NFConfig, 
  Spinner, 
  FloatingButton, 
  Blank,
  Dimmed,
  Confirm
} from 'components';

import Document from './Document';

class Collection extends Component {
  constructor(props) {
    super(props)
    const { nfconfigs, dispatch } = props

    if (nfconfigs.needsFetch) {
      dispatch(nfconfigs.fetch)
    }
  }
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
    },
    selectedNF: null,
    rectCoordinates: null,
  };
  
  handleSelectedNFChange = (selectedNF) => {
    console.log("handleSelectedNFChange"+selectedNF)
    this.setState({ selectedNF });
  };

  handleRectCoordinatesChange = (rectCoordinates) => {
    this.setState({ rectCoordinates });
  };

  handleViewHandlerShow = (_id) => {
    this.viewHandler.show(_id);
  };
/*
  componentWillMount() {
    const { nfconfigs, dispatch } = this.props

    if (nfconfigs.needsFetch) {
      dispatch(nfconfigs.fetch)
    }
  }
  */
  componentDidMount() {
    const { nfconfigs, dispatch } = this.props

    if (nfconfigs.needsFetch) {
      dispatch(nfconfigs.fetch)
    }
  }
  /*
  componentWillReceiveProps(nextProps) {
    const { nfconfigs, status } = nextProps
    const { dispatch } = this.props

    if (nfconfigs.needsFetch) {
      dispatch(nfconfigs.fetch)
    }

    if (status.response) {
      dispatch(Notification.success({
        title: 'NFConfig',
//        message: `${status.id} has been deleted`
        message: `This nfconfig has been deleted`
      }));
      dispatch(clearActionStatus(MODEL, 'delete'));
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

      dispatch(Notification.error({
        title,
        message,
        autoDismiss: 0,
        action: {
          label: 'Dismiss'
        }
      }));
      dispatch(clearActionStatus(MODEL, 'delete'));
    }
  }*/
  componentDidUpdate(prevProps) {
    const { nfconfigs, status } = this.props;
    const { dispatch } = this.props;

    if (nfconfigs.needsFetch && nfconfigs.needsFetch !== prevProps.nfconfigs.needsFetch) {
      dispatch(nfconfigs.fetch);
    }

    if (status.response && status.response !== prevProps.status.response) {
      dispatch(Notification.success({
        title: 'NFConfig',
        message: `This nfconfig has been deleted`
      }));
      dispatch(clearActionStatus(MODEL, 'delete'));
    } 

    if (status.error && status.error !== prevProps.status.error) {
      let title = 'Unknown Code';
      let message = 'Unknown Error';
      if (response.data && response.data.name && response.data.message) {
        title = response.data.name;
        message = response.data.message;
      } else {
        title = response.status;
        message = response.statusText;
      }

      dispatch(Notification.error({
        title,
        message,
        autoDismiss: 0,
        action: {
          label: 'Dismiss'
        }
      }));
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
    },
    actions : {
      delete: () => {
        const { dispatch } = this.props

        if (this.state.confirm.visible === true) {
          this.confirmHandler.hide();
          this.documentHandler.hide();
          this.viewHandler.hide();

          dispatch(deleteNFConfig(this.state.confirm._id));
        }
      }
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
      nfconfigs,
      status
    } = this.props

    const {
      isLoading,
      data
    } = nfconfigs;

    return (
      <Layout.Content>
        <ArchitectureDiagram
          onSelectedNFChange={this.handleSelectedNFChange}
          onRectCoordinatesChange={this.handleRectCoordinatesChange}
          onViewHandlerShow={this.handleViewHandlerShow}
        />
        <NFConfig.List
          nfconfigs={data}
          deletedId={status.id}
          onView={viewHandler.show}
          onEdit={documentHandler.actions.update}
          onDelete={confirmHandler.show}
        />
        {isLoading && <Spinner md />}
        <Blank
          visible={!isLoading && !(Object.keys(data).length > 0)}
          title="ADD A NFCONFIG"
          body="You have no nfconfigs... yet!"
          onTitle={documentHandler.actions.create}
          />
        {/*<FloatingButton onClick={documentHandler.actions.create}/>*/}
        <NFConfig.View
          visible={this.state.view.visible}
          disableOnClickOutside={this.state.view.disableOnClickOutside}
          nfconfigs={data.filter(nfconfig => 
            (nfconfig._nf === this.state.view._id))}
          onEdit={documentHandler.actions.update}
          onDelete={confirmHandler.show}
          onHide={viewHandler.hide}/>
        <Document 
          { ...document }
          onEdit={documentHandler.actions.update}
          onDelete={confirmHandler.show}
          onHide={documentHandler.hide} />
        <Dimmed visible={document.dimmed} />
        <Confirm
          visible={this.state.confirm.visible}
          message="Delete this nfconfig?"
          onOutside={confirmHandler.hide}
          buttons={[
            { text: "CANCEL", action: confirmHandler.hide, info:true },
            { text: "DELETE", action: confirmHandler.actions.delete, danger:true }
          ]}/>
		  

      </Layout.Content>
    )
  }
}

Collection = connect(
  (state) => ({ 
    nfconfigs: select(fetchNFConfigs(), state.crud),
    status: selectActionStatus(MODEL, state.crud, 'delete')
  })
)(Collection);

export default Collection;
