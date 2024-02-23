import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import NProgress from 'nprogress';

import { MODEL, fetchImeichecks, fetchImeicheck, createImeicheck, updateImeicheck } from 'modules/crud/imeicheck';
import { Ommlog } from 'modules/crud/ommlog';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';

import { Imeicheck } from 'components';

import traverse from 'traverse';

const formData = {
  "status": 0,
  "checkflag": false,  
}

class Document extends Component {
  constructor(props) {
    super(props);
    const { imeichecks, dispatch } = props

    if (imeichecks.needsFetch) {
      dispatch(imeichecks.fetch)
    }
  }
  static propTypes = {
    action: PropTypes.string,
    visible: PropTypes.bool, 
    onHide: PropTypes.func
  }

  state = {
    formData
  }

  componentDidMount() {
    const { imeicheck, dispatch } = this.props

    if (imeicheck.needsFetch) {
      dispatch(imeicheck.fetch)
    }
  }
 
  componentDidUpdate(prevProps) {
    const { imeicheck, status, dispatch, action, onHide } = this.props;

    if (imeicheck.needsFetch && imeicheck.needsFetch !== prevProps.imeicheck.needsFetch) {
      dispatch(imeicheck.fetch);
    }
/*
    if (imeicheck.data && imeicheck.data !== prevProps.imeicheck.data) {
     
      this.setState({ formData: imeicheck.data });
    } else if (!imeicheck.data && prevProps.imeicheck.data){
      this.setState({ formData });
    }

    if (status.response && status.response !== prevProps.status.response) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

      const message = action === 'create' ? "New imeicheck created" : "This imeicheck updated";

      dispatch(Notification.success({
        title: 'Imeicheck',
        message
      }));

      dispatch(clearActionStatus(MODEL, action));
      onHide();
    }

    if (status.error && status.error !== prevProps.status.error) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

      const response = (status.error.response || {});

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
*/
    if (status.response && prevProps.status.response !== status.response) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);
  
      const message = action === 'create' ? "New IEMI created" : `${status.id} IMEI updated`;
  
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
    const { imeichecks, action, status } = this.props;
    const { imei } = formData;

    if (action === 'create' && imeichecks && imeichecks.data &&
    imeichecks.data.filter(imeicheck => imeicheck.imei === imei).length > 0) {
      errors.imei.addError(`'${imei}' is duplicated`);
    }
    console.log("formData:",formData)
    if(formData.checkflag){
      const {bindimsi} = formData;      
      if(bindimsi){
        console.log("bindimsi:",bindimsi)
        if(bindimsi.length<1){
          errors.bindimsi.addError(`绑定的IMSI不能为空`);
        }else if(!bindimsi[0].imsi){
          errors.bindimsi.addError(`绑定的IMSI不能为空`);
        }

      }else{
        errors.bindimsi.addError(`绑定的IMSI不能为空`);
      }
    }
    if(formData.bindimsi){
      if(formData.bindimsi.length>1){
        const imsiSet = new Set(); // 使用 Set 存储已经出现过的 imsi 值

        formData.bindimsi.forEach((item, index) => {
          if (imsiSet.has(item.imsi)) {
            errors.bindimsi[index].imsi.addError(`'${item.imsi}' is duplicated`);
          } else {
            imsiSet.add(item.imsi);
          }
        });    
      }
   }

    return errors;
  }

  handleSubmit = (formData) => {
    const { dispatch, action } = this.props;  

    NProgress.configure({ 
      parent: '#nprogress-base-form',
      trickleSpeed: 5
    });
    NProgress.start();

    if (action === 'create') {
      dispatch(createImeicheck({}, formData));
      dispatch(Ommlog.createOmmlog(action,"黑白名单",{}, formData));
    } else if (action === 'update') {
      dispatch(updateImeicheck(formData.imei, {}, formData));
      dispatch(Ommlog.createOmmlog(action,"黑白名单",{}, formData));
    } else {
      throw new Error(`Action type '${action}' is invalid.`);
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
      visible,
      action,
      status,
      imeicheck,
      onHide
    } = this.props
    let editformData =imeicheck.data || {}; 
    if (action === 'create') {
      editformData = { ...formData, ...imeicheck.data }; 
    }
    return (
      <Imeicheck.Edit
        visible={visible} 
        action={action}
        formData={editformData}
        isLoading={imeicheck.isLoading && !status.pending}
        validate={validate}
        onHide={onHide}
        onSubmit={handleSubmit}
        onError={handleError} />
    )
  }
}

Document = connect(
  (state, props) => ({ 
    imeichecks: select(fetchImeichecks(), state.crud),
    imeicheck: select(fetchImeicheck(props.imei), state.crud),
    status: selectActionStatus(MODEL, state.crud, props.action)
  })
)(Document);

export default Document;
