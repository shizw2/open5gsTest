import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import NProgress from 'nprogress';

import { MODEL, fetchAccounts, fetchAccount, createAccount, updateAccount } from 'modules/crud/account';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';

import { Account } from 'components';
import { Ommlog} from 'modules/crud/ommlog';
import traverse from 'traverse';
import crypto from 'crypto';

const formData = {
  "roles": [ "user" ],
}

class Document extends Component {
  constructor(props) {
    super(props);
    const { accounts, dispatch,profiles } = props;

    if (accounts.needsFetch) {
      dispatch(accounts.fetch);
    }
    /*
    if (profiles.needsFetch) {
      dispatch(profiles.fetch);
    }
  */
  }
  static propTypes = {
    action: PropTypes.string,
    visible: PropTypes.bool, 
    onHide: PropTypes.func
  }

  state = {
    formData
  }
 /* 
 componentWillMount() {
    const { account, dispatch } = this.props

    if (account.needsFetch) {
      dispatch(account.fetch)
    }
  }
*/
async componentDidMount() {
  const { account, dispatch } = this.props;

  if (account.needsFetch) {
    await dispatch(account.fetch);
  }

  if (account.data) {
    this.setState({ formData: account.data });
  }
}
/*
  componentWillReceiveProps(nextProps) {
    const { account, status } = nextProps
    const { dispatch, action, onHide } = this.props

    if (account.needsFetch) {
      dispatch(account.fetch)
    }

    if (account.data) {
      this.setState({ formData: account.data })
    } else {
      this.setState({ formData });
    }

    if (status.response) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

      const message = action === 'create' ? "New account created" : `${status.id} account updated`;

      dispatch(Notification.success({
        title: 'Account',
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

*/
componentDidUpdate(prevProps) {
  const { accounts, account, status, dispatch, action, onHide } = this.props;
/*
  if (accounts.needsFetch && prevProps.accounts !== accounts) {
    dispatch(accounts.fetch);
  }
*/
/*
if (account.data && !isEqual(account.data, prevProps.account.data)) {
  this.setState({ formData: account.data }, () => {
    console.log("this.state.formData==", this.state.formData);
    // 在回调函数中执行其他需要使用最新状态值的操作
  });
}
console.log("this.state.formData",this.state.formData)
*/
/*
  // 确保在账户数据更新后更新表单数据 
  console.log("eeeee",account.data,prevProps.account.data)
  if (account !== prevProps.account) {
    if (account.data) {
      this.setState({ formData: account.data });
    } else {
      // 处理没有数据的情况
    }
  }
  console.log("this.state.formData",this.state.formData)*/
  if (status.response && prevProps.status.response !== status.response) {
    NProgress.configure({ 
      parent: 'body',
      trickleSpeed: 5
    });
    NProgress.done(true);

    const message = action === 'create' ? "New account created" : `${status.id} account updated`;

    dispatch(Notification.success({
      title: 'Account',
      message
    }));

    dispatch(clearActionStatus(MODEL, action));
    onHide();
  } 

  if (status.error && prevProps.status.error !== status.error) {
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
    const { accounts, action, status } = this.props;
    const { username, password1, password2 } = formData;

    if (action === 'create' && accounts && accounts.data &&
      accounts.data.filter(account => account.username === username).length > 0) {
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
      dispatch(createAccount({}, formData));
      dispatch(Ommlog.createOmmlog(action,"账号管理",{}, formData.username));
    } else if (action === 'update') {
      dispatch(updateAccount(formData.username, {}, formData));
      dispatch(Ommlog.createOmmlog(action,"账号管理",{}, formData.username));
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
      account,
      onHide
    } = this.props    
    let editformData = account.data || {}; 
    if (action === 'create') {
      editformData = { ...formData, ...account.data }; // 将 account.data 的值合并到 formData 中
    }
    return (
      <Account.Edit
        session={session}
        visible={visible} 
        action={action}
        formData={editformData}
        //formData={this.state.formData}
        isLoading={account.isLoading && !status.pending}
        validate={validate}
        onHide={onHide}
        onSubmit={handleSubmit}
        onError={handleError} />
    )
  }
}

Document = connect(
  (state, props) => ({ 
    accounts: select(fetchAccounts(), state.crud),
    account: select(fetchAccount(props.username), state.crud),
    status: selectActionStatus(MODEL, state.crud, props.action)
  })
)(Document);

export default Document;
