import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import NProgress from 'nprogress';

//import { MODEL, fetchProfiles, fetchProfile, createProfile, updateProfile } from 'modules/crud/profile';

import { MODEL, Ommlog } from 'modules/crud/ommlog';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';


import traverse from 'traverse';

const formData = {
  "opuser":[ "user" ],
  "optcommand":[ "user" ], 
}

class Document extends Component {
  static propTypes = {
    action: PropTypes.string,
    visible: PropTypes.bool, 
    onHide: PropTypes.func
  }

  state = {
    formData
  }

  componentWillMount() {
    const { oprlog, dispatch } = this.props

    if (oprlog.needsFetch) {
      dispatch(oprlog.fetch)
    }
  }

  componentWillReceiveProps(nextProps) {
    const { oprlog, status } = nextProps
    const { dispatch, action, onHide } = this.props

    if (oprlog.needsFetch) {
      dispatch(oprlog.fetch)
    }
  

    if (status.response) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

//      const message = action === 'create' ? "New profile created" : `${status.id} profile updated`;
      const message = action === 'create' ? "New profile created" : `This profile updated`;

      dispatch(Notification.success({
        title: 'oprlog',
        message
      }));

      dispatch(clearActionStatus(MODEL, action));
      onHide();
    } 

    if (status.error) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

      const response = ((status || {}).error || {}).response || {};

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
          label: 'Dismiss',
          callback: () => onHide()
        }
      }));
      dispatch(clearActionStatus(MODEL, action));
    }
  }

  validate = (formData, errors) => {
    const { oprlogs, action, status } = this.props;

    return errors;
  }

  handleSubmit = (formData) => {
    const { dispatch, action } = this.props;

    NProgress.configure({ 
      parent: '#nprogress-base-form',
      trickleSpeed: 5
    });
    NProgress.start();

  }

  handleError = errors => {
    const { dispatch } = this.props;
    errors.map(error =>
      dispatch(Notification.error({
        title: 'Validation Error',
        message: error.stack
      }))
    )
  }

  render() {
    const {
      validate,
      handleSubmit,
      handleError
    } = this;

    const { 
      visible,
      action,
      status,
      oprlog,
      onHide
    } = this.props

    return (
      <div />
    )
  }
}

Document = connect(
  (state, props) => ({ 
    oprlogs: select(Ommlog.fetchOmmlogs({}), state.crud),
    oprlog: select(Ommlog.fetchOmmlog(props._id), state.crud),
    status: selectActionStatus(MODEL, state.crud, props.action)
  })  
)(Document);

export default Document;
