import axios from 'axios';
import { all, takeEvery, put, call, take, fork } from 'redux-saga/effects';
import { CRUD } from './actions';
import Session from 'modules/auth/session';

/*baseURL 是基础 URL，它是请求的根路径或端点的前缀。如果没有指定 baseURL 参数，默认值为 '/api/db'。在实际请求中，将使用 baseURL 作为请求的起始路径。
url 是特定请求的相对 URL，它是基于 baseURL 构建的。它表示请求的具体路径和查询参数。
通过将 baseURL 和 url 组合在一起，可以构建出完整的请求 URL。*/
const crudApi = (method, url, csrf, authToken, { params, data } = {}, baseURL = '/api/db') => {
  let headers = { 'X-CSRF-TOKEN': csrf }
  if (authToken) {
    headers['Authorization'] = "Bearer " + authToken
  }
  return axios({
    baseURL,
    headers: headers,
    method,
    url,
    params,
    data
  });
}

function* crudEntity(action) {
  const { method, url, params, data } = action.payload;
  const { success, failure } = action.meta;
  const meta = {
    ...action.meta,
    fetchedAt: Date.now()
  }

  try {
    const sessionData = new Session();
    const csrf = ((sessionData || {}).session || {}).csrfToken;
    const authToken = ((sessionData || {}).session || {}).authToken;
    // 根据 url 判断使用哪个基础 URL
    //const baseURL = url === '/NFConfig' ? '/api/yaml' : '/api/db';
    const baseURL = url.startsWith('/NFConfig') ? '/api/yaml' : '/api/db';
    const response = yield call(crudApi, method, url, csrf, authToken, { params, data }, baseURL);
    yield put({ meta, type: success, payload: response })
  } catch (error) {
    yield put({ meta, type: failure, payload: error, error: true })
  }
}

function* watchFetch() {
  while(true) {
    const action = yield take(CRUD.FETCH);
    yield fork(crudEntity, action);
  } 
}

function* watchFetchOne() {
  while(true) {
    const action = yield take(CRUD.FETCH_ONE);
    yield fork(crudEntity, action);
  } 
}

function* watchCreate() {
  while(true) {
    const action = yield take(CRUD.CREATE);
    yield fork(crudEntity, action);
  } 
}

function* watchUpdate() {
  while(true) {
    const action = yield take(CRUD.UPDATE);
    yield fork(crudEntity, action);
  } 
}

function* watchDelete() {
  while(true) {
    const action = yield take(CRUD.DELETE);
    yield fork(crudEntity, action);
  } 
}

/*导出的默认 Generator 函数是应用程序的根 Saga，它将所有的 Watcher 函数组合在一起
使用 all 和 fork 将所有的 Watcher 函数并行地执行*/
export default function* () {
  yield all([
    fork(watchFetch),
    fork(watchFetchOne),
    fork(watchCreate),
    fork(watchUpdate),
    fork(watchDelete)
  ])
}
