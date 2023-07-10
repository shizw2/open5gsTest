import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';
import { bindActionCreators, compose } from 'redux';

import * as sidebarActions from 'modules/sidebar';
import withWidth, { SMALL } from 'helpers/with-width';

import { Layout } from 'components';
import Notification from 'containers/Notification';
import * as Subscriber from 'containers/Subscriber';
import * as Profile from 'containers/Profile';
import * as Account from 'containers/Account';
import * as NFConfig from 'containers/NFConfig';

class App extends Component {
  static propTypes = {
    session: PropTypes.object.isRequired,
    view: PropTypes.string.isRequired,
    width: PropTypes.number.isRequired
  }
  
  componentWillMount() {
    const { 
      width,
      SidebarActions
    } = this.props;

    if (width !== SMALL) {
      SidebarActions.setVisibility(true);
    }
  }

  render() {
    const {
      view,
      session
    } = this.props;

    if (view === "subscriber") {
      document.body.style.backgroundColor = "#e9ecef";
    } else {
      document.body.style.backgroundColor = "white";
    }

    return (
      <Layout>

        <Layout.Container visible={view === "profile"}>
          <Profile.Collection/>
        </Layout.Container>
        <Layout.Container visible={view === "account"}>
          <Account.Collection session={session}/>
        </Layout.Container>
        <Layout.Container visible={view === "nfconfig"}>
          <NFConfig.Collection session={session}/>
        </Layout.Container>
        <Notification/>
      </Layout>
    )
  }
}

const enhance = compose(
  withWidth(),
  connect(
    (state) => ({
      view: state.sidebar.view
    }),
    (dispatch) => ({
      SidebarActions: bindActionCreators(sidebarActions, dispatch)
    })
  )
);

export default enhance(App);
