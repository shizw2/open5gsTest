import {
  fetchCollection,
  fetchDocument,
  createDocument,
  updateDocument,
  deleteDocument
} from './actions'

export const MODEL = 'nfconfigs';
export const URL = '/NFConfig';

export const fetchNFConfigs = (params = {}) => {
  return fetchCollection(MODEL, URL, params);
}

export const fetchNFConfig = (_id, params = {}) => {
  return fetchDocument(MODEL, _id, `${URL}/${_id}`, params);
}

export const createNFConfig = (params = {}, data = {}) => {
  return createDocument(MODEL, URL, params, data);
}

export const updateNFConfig = (_id, params = {}, data = {}) => {
  return updateDocument(MODEL, _id, `${URL}/${_id}`, params, data);
}

export const deleteNFConfig = (_id, params = {}) => {
  return deleteDocument(MODEL, _id, `${URL}/${_id}`, params);
}
