import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import NotificationSystem from 'react-notification-system';

import * as actions from 'modules/notification/actions';
/*
class Notification extends Component {

  componentWillReceiveProps(nextProps) {

    const { dispatch } = this.props;
    const { notifications } = nextProps;

    const notificationIds = notifications.map(notification => notification.uid);
    const systemNotifications = this.system.state.notifications || [];

    if (notifications.length > 0) {
      systemNotifications.forEach(notification => {
        if (notificationIds.indexOf(notification.uid) < 0) {
          this.system.removeNotification(notification.uid);
        }
      });

      notifications.forEach(notification => {
        this.system.addNotification({
          ...notification,
          onRemove: () => {
            dispatch(actions.hide(notification.uid));
            notification.onRemove && notification.onRemove();
          }
        });
      });
    }

    if ((this.props.notifications !== notifications) && notifications.length === 0) {
      this.system.clearNotifications();
    }
  }

  shouldComponentUpdate(nextProps) {
    return this.props !== nextProps;
  }

  render() {
    const { notifications, ...rest} = this.props;

    return <NotificationSystem ref={ref => this.system = ref} { ...rest } />
  }
}

Notification = connect(
  (state) => ({
    notifications: state.notifications
  })
)(Notification);

export default Notification;
*/
class Notification extends Component {
  static propTypes = {
    notifications: PropTypes.array.isRequired,
    dispatch: PropTypes.func.isRequired,
  };

  componentDidMount() {
    this.updateNotifications();
  }

  componentDidUpdate(prevProps) {
    if (prevProps.notifications !== this.props.notifications) {
      this.updateNotifications();
    }
  }

  updateNotifications() {
    const { notifications, dispatch } = this.props;
    const notificationIds = notifications.map(notification => notification.uid);
    const systemNotifications = this.system.state.notifications || [];

    if (notifications.length > 0) {
      systemNotifications.forEach(notification => {
        if (notificationIds.indexOf(notification.uid) < 0) {
          this.system.removeNotification(notification.uid);
        }
      });

      notifications.forEach(notification => {
        this.system.addNotification({
          ...notification,
          onRemove: () => {
            dispatch(actions.hide(notification.uid));
            notification.onRemove && notification.onRemove();
          },
        });
      });
    }

    if (notifications.length === 0) {
      this.system.clearNotifications();
    }
  }

  render() {
    const { notifications, ...rest } = this.props;

    return <NotificationSystem ref={ref => this.system = ref} {...rest} />;
  }
}

const mapStateToProps = (state) => ({
  notifications: state.notifications,
});

export default connect(mapStateToProps)(Notification);
