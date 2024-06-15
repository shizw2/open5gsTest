import {
  fetchCollection,
  fetchDocument,
  createDocument,
  updateDocument,
  deleteDocument
} from './actions'

export const MODEL = 'imeichecks';
export const URL = '/Imeicheck';

export const fetchImeichecks = (params = {}) => {
  return fetchCollection(MODEL, URL, params, { idProperty: 'imei' });
}

export const fetchImeicheck = (imei, params = {}) => {
  return fetchDocument(MODEL, imei, `${URL}/${imei}`, params, { idProperty: 'imei' });
}

export const createImeicheck = (params = {}, data = {}) => {
  return createDocument(MODEL, URL, params, data, { idProperty: 'imei' });
}

export const updateImeicheck = (imei, params = {}, data = {}) => {
  return updateDocument(MODEL, imei, `${URL}/${imei}`, params, data, { idProperty: 'imei' });
}

export const deleteImeicheck = (imei, params = {}) => {
  return deleteDocument(MODEL, imei, `${URL}/${imei}`, params, { idProperty: 'imei' });
}
