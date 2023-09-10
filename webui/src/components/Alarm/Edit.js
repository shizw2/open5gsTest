import { Component } from 'react';
import PropTypes from 'prop-types';

import withWidth, { SMALL } from 'helpers/with-width';
import { Form } from 'components';


class Edit extends Component {
  static propTypes = {
    visible: PropTypes.bool, 
    action: PropTypes.string, 
    formData: PropTypes.object,
    isLoading: PropTypes.bool,
    validate: PropTypes.func, 
    onHide: PropTypes.func, 
    onSubmit: PropTypes.func,
    onError: PropTypes.func
  }

  constructor(props) {
    super(props);

    this.state = this.getStateFromProps(props);
  }

  componentWillReceiveProps(nextProps) {

    this.setState(this.getStateFromProps(nextProps));
  }

  getStateFromProps(props) {
    const { 
      session,
      action,
      width,
      formData,
    } = props;

    const {
      username,
      roles
    } = session.user;
    


    return state;
  }

  render() {
    const {
      visible,
      action,
      isLoading,
      validate,
      onHide,
      onSubmit,
      onError
    } = this.props;

    const {
      formData
    } = this.state;

    return (
      <Form 
        visible={visible}
        title={'Alarm'}
        width="480px"
        height="400px"
        formData={formData}
        isLoading={isLoading}
        validate={validate}
        onHide={onHide}
        onSubmit={onSubmit}
        onError={onError}/>
    )
  }
}

export default withWidth()(Edit);
