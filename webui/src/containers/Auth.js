import React,{ Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';
import { bindActionCreators } from 'redux';
import Router from 'next/router';
import NProgress from 'nprogress';

import Session from 'modules/auth/session';
import { Login } from 'components';

class Auth extends Component {
  constructor(props) {
    super(props);

    this.inputRef = React.createRef();
    this.state = {
    error: null,
    form: {
      username: '',
      password: ''
    },
    session: {}
  };
  }
  static async getInitialProps({req}) {
    const session = new Session({req})
    return {session: await session.getSession(true)}
  }

  async componentDidMount() {
    const session = new Session()
    this.setState({ 
      session: await session.getSession(true)
    });

    if (this.inputRef.current) {
      this.inputRef.current.focus();
  }
  }
/*
  setInnerRef = (comp) => {
    this.input = comp;
  }
*/
  handleChange = (e) => {
    this.setState({ 
      form: {
        ...this.state.form,
        [e.target.name]: e.target.value 
      }
    });
  }

  handleKeyPress = (e) => {
    if (e.charCode === 13) {
        this.handleSubmit();
    }
  }

  handleSubmit = (e) => {
    const { 
      username,
      password
    } = this.state.form;

    const {
      loginRequest
    } = this.props;

    NProgress.configure({ 
      parent: '#nprogress-base-login',
      trickleSpeed: 5
    });
    NProgress.start();

    const session = new Session()
    session.signin(username, password)
      .then(() => {
        NProgress.configure({ 
          parent: 'body',
          trickleSpeed: 5
        });
        NProgress.done();

        Router.push('/');
      })
      .catch(err => {
        NProgress.configure({ 
          parent: 'body',
          trickleSpeed: 5
        });
        NProgress.done();

        this.setState({
          error: { 
            message: err.message
          }
        });

        if (this.inputRef && this.inputRef.current) {
          this.inputRef.current.focus(); // 确保 inputRef 存在且不为空
        }
      })
  }

  handleErrorReset = (e) => {
    this.setState({
      error: null
    });
  }

  render() {
    const { 
      form,
      error
    } = this.state;
/*
    const { 
      setInnerRef,
      handleChange,
      handleSubmit,
      handleKeyPress,
      handleErrorReset
    } = this;

    return (
      <Login
        form={form}
        error={error}
        innerRef={setInnerRef}
        onChange={handleChange}
        onSubmit={handleSubmit}
        onKeyPress={handleKeyPress}
        onErrorReset={handleErrorReset}
      />
    );
    */
    return (
      <Login
        form={form}
        error={error}
        innerRef={this.inputRef}
        onChange={this.handleChange}
        onSubmit={this.handleSubmit}
        onKeyPress={this.handleKeyPress}
        onErrorReset={this.handleErrorReset}
      />
    );
  }
}

export default Auth;