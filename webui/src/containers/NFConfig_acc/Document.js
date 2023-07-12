import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import NProgress from 'nprogress';

import { MODEL, fetchNFConfigs, fetchNFConfig, createNFConfig, updateNFConfig } from 'modules/crud/nfconfig';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';

import { NFConfig } from 'components';

import traverse from 'traverse';
import crypto from 'crypto';

const formData = {
  "roles": [ "user" ],
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
    const { nfconfig, dispatch } = this.props

    if (nfconfig.needsFetch) {
      dispatch(nfconfig.fetch)
    }
  }

  componentWillReceiveProps(nextProps) {
    const { nfconfig, status } = nextProps
    const { dispatch, action, onHide } = this.props

    if (nfconfig.needsFetch) {
      dispatch(nfconfig.fetch)
    }

    if (nfconfig.data) {
      this.setState({ formData: nfconfig.data })
    } else {
      this.setState({ formData });
    }

    if (status.response) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

      const message = action === 'create' ? "New nfconfig created" : `${status.id} nfconfig updated`;

      dispatch(Notification.success({
        title: 'NFConfig',
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
    const { nfconfigs, action, status } = this.props;
    const { username, password1, password2 } = formData;

    if (action === 'create' && nfconfigs && nfconfigs.data &&
      nfconfigs.data.filter(nfconfig => nfconfig.username === username).length > 0) {
      errors.username.addError(`'${username}' is duplicated`);
    }

    if (action === 'create') {
       if (password1 === undefined) {
          errors.password1.addError(`is required`);
       }
       if (password2 === undefined) {
          errors.password2.addError(`is required`);
       }
    }

    if (password1 != password2) {
      if (Object.keys(errors.password1.__errors).length == 0)
        errors.password1.addError(`is not matched`);
      if (Object.keys(errors.password2.__errors).length == 0)
        errors.password2.addError(`is not matched`);
    }

    return errors;
  }

  generatePassword = function(password, cb) {
    crypto.randomBytes(32, function(randomBytesErr, buf) {
      if (randomBytesErr) {
        return cb(randomBytesErr);
      }
      var salt = buf.toString('hex');

      crypto.pbkdf2(password, salt, 25000, 512, 'sha256', function(pbkdf2Err, hashRaw) {
        if (pbkdf2Err) {
          return cb(pbkdf2Err);
        }
        var hash = new Buffer(hashRaw, 'binary').toString('hex');

        cb(null, salt, hash);
      });
    });
  };

  submit = (formData) => {
    const { dispatch, action } = this.props;

    NProgress.configure({ 
      parent: '#nprogress-base-form',
      trickleSpeed: 5
    });
    NProgress.start();

    if (action === 'create') {
      dispatch(createNFConfig({}, formData));
    } else if (action === 'update') {
      dispatch(updateNFConfig(formData._id, {}, formData));
    } else {
      throw new Error(`Action type '${action}' is invalid.`);
    }
  }

  handleSubmit = (formData) => {
    if (formData.password1 === undefined) {
      this.submit(formData);
    } else {
      this.generatePassword(formData.password1, (err, salt, hash) => {
        if (err) throw err

        formData = Object.assign(formData, {
          salt,
          hash,
        })

        this.submit(formData);
      })
    }
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
      session,
      visible,
      action,
      status,
      nfconfig,
      onHide
    } = this.props

    return (
      <NFConfig.Edit
        session={session}
        visible={visible} 
        action={action}
        formData={this.state.formData}
        isLoading={nfconfig.isLoading && !status.pending}
        validate={validate}
        onHide={onHide}
        onSubmit={handleSubmit}
        onError={handleError} />
    )
  }
}

Document = connect(
  (state, props) => ({ 
    nfconfigs: select(fetchNFConfigs(), state.crud),
    nfconfig: select(fetchNFConfig(props._id), state.crud),
    status: selectActionStatus(MODEL, state.crud, props.action)
  })
)(Document);

export default Document;
