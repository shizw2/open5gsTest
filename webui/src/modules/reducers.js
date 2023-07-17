import { combineReducers } from 'redux';

import crud from './crud/reducers';
import sidebar from './sidebar';
import notifications from './notification/reducers';
/*combineReducers 函数用于将多个 reducer 合并成一个根 reducer*/
export default combineReducers({
  crud,
  sidebar,
  notifications
});
