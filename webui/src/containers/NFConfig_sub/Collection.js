import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import { MODEL, fetchNFConfigs, deleteNFConfig } from 'modules/crud/nfconfig';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';

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
  state = {
    search: '',
    document: {
      action: '',
      visible: false,
      dimmed: false
    },
    confirm: {
      visible: false,
      imsi: ''
    },
    view: {
      visible: false,
      disableOnClickOutside: false,
      imsi: ''
    }
  };

  componentWillMount() {
    const { nfconfigs, dispatch } = this.props

    if (nfconfigs.needsFetch) {
      dispatch(nfconfigs.fetch)
    }
  }

  componentWillReceiveProps(nextProps) {
    const { nfconfigs, status } = nextProps
    const { dispatch } = this.props

    if (nfconfigs.needsFetch) {
      dispatch(nfconfigs.fetch)
    }

    if (status.response) {
      dispatch(Notification.success({
        title: 'NFConfig',
        message: `${status.id} has been deleted`
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
  }

  handleSearchChange = (e) => {
    this.setState({
      search: e.target.value
    });
  }

  handleSearchClear = (e) => {
    this.setState({
      search: ''
    });
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
      update: (imsi) => {
        this.documentHandler.show('update', { imsi });
      }
    }
  }

  confirmHandler = {
    show: (imsi) => {
      this.setState({
        confirm: {
          visible: true,
          imsi,
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

          dispatch(deleteNFConfig(this.state.confirm.imsi));
        }
      }
    }
  }

  viewHandler = {
    show: (imsi) => {
      this.setState({
        view: {
          imsi,
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
      handleSearchChange,
      handleSearchClear,
      documentHandler,
      viewHandler,
      confirmHandler
    } = this;

    const { 
      search,
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
        {Object.keys(data).length > 0 && <NFConfig.Search 
          onChange={handleSearchChange}
          value={search}
          onClear={handleSearchClear} />}
        <NFConfig.List
          nfconfigs={data}
          deletedImsi={status.id}
          onView={viewHandler.show}
          onEdit={documentHandler.actions.update}
          onDelete={confirmHandler.show}
          search={search}
        />
        {isLoading && <Spinner md />}
        <Blank
          visible={!isLoading && !(Object.keys(data).length > 0)}
          title="ADD A SUBSCRIBER"
          body="You have no nfconfigs... yet!"
          onTitle={documentHandler.actions.create}
          />
        <FloatingButton onClick={documentHandler.actions.create}/>
        <NFConfig.View
          visible={this.state.view.visible}
          nfconfig={data.filter(nfconfig => 
            nfconfig.imsi === this.state.view.imsi)[0]}
          disableOnClickOutside={this.state.view.disableOnClickOutside}
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