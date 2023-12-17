
import { Component } from 'react'
import PropTypes from 'prop-types';
import {
  fetchCollection,
  fetchDocument,
  createDocument,  
  deleteDocument
} from './actions'

export const MODEL = 'ommlogs';
export const URL = '/Ommlog';
/*
const logformData = {
  "opuser": "admin",
  "optype": "add",
  "optcommand": "详细内容：" ,
  "optfm": "其他",
  "opttime": ""
}

export const fetchOmmlogs = (params = {}) => {
  return fetchCollection(MODEL, URL, params);
}

export const fetchOmmlog = (_id, params = {}) => {
  return fetchDocument(MODEL, _id, `${URL}/${_id}`, params);
}

export const createOmmlog = (action,fm,params = {}, data = {},info) => { 
  logformData.optcommand="";  
  logformData.optfm=fm;
  logformData.optcommand +="客户端IP：";
  logformData.optcommand +=username;
  logformData.optcommand +=" 操作对象：";
  const currentDate = new Date();
  const currentDateString = currentDate.toLocaleString();
  logformData.opttime=currentDateString;
  if(action==='create')
  {
    logformData.optype="新增";
    logformData.optcommand+=JSON.stringify(data);
  }
  else if(action==='update')
  {
    logformData.optype="修改";
    logformData.optcommand+=JSON.stringify(data);
  }
  else if(action==='delete'){
    logformData.optype="删除";
    logformData.optcommand+=info;
    
  }  
  return createDocument(MODEL, URL, params, logformData);
}

export const deleteOmmlog = (_id, params = {}) => {
  return deleteDocument(MODEL, _id, `${URL}/${_id}`, params);
}
*/
let globalUsername = "";
class Ommlog extends Component{  
  constructor(props) {
    super(props);
    this.state = this.getStateFromProps(props);
  }


  getStateFromProps(props) {
    const { 
      session,
    } = props;

    const {
      username,
      roles
    } = session.user;    
    globalUsername = username;    
  }
  render() {
    return (
      <div>
        {/* 其他代码... */}
      </div>
    );
  }
  static createOmmlog(action, fm, params = {}, data = {}, info) {
    const logformData = {
      optcommand:"",
      optype: "新增",
      opuser: globalUsername, 
      optfm:fm,     
      optcommand:"操作对象："
    };    
    const currentDate = new Date();
    const year = currentDate.getFullYear();
    const month = String(currentDate.getMonth() + 1).padStart(2, '0');
    const day = String(currentDate.getDate()).padStart(2, '0');
    const hours = String(currentDate.getHours()).padStart(2, '0');
    const minutes = String(currentDate.getMinutes()).padStart(2, '0');
    const seconds = String(currentDate.getSeconds()).padStart(2, '0');
    const currentDateString = `${year}/${month}/${day} ${hours}:${minutes}:${seconds}`;
    
    logformData.opttime = currentDateString;
   
    if (action === "create") {
      logformData.optype = "新增";
        if (Object.keys(data).length > 0){        
        logformData.optcommand += JSON.stringify(data);
      }else{
        logformData.optcommand += info;        
      }      
    } else if (action === "update") {
      logformData.optype = "修改";
      if(Object.keys(data).length > 0){
        logformData.optcommand += JSON.stringify(data);
    }} else if (action === "delete") {
      logformData.optype = "删除";
      logformData.optcommand += info;
    } else if(action === "login") {
      logformData.optype = "登入";
        if (Object.keys(data).length > 0){        
        logformData.optcommand += JSON.stringify(data);
      }else{
        logformData.optcommand += info;        
      }      
    }else if(action === "logout") {
      logformData.optype = "登出";
        if (Object.keys(data).length > 0){        
        logformData.optcommand += JSON.stringify(data);
      }else{
        logformData.optcommand += info;        
    } 
    }
    console.log("createDocument:",MODEL, URL, params, logformData);  
    return createDocument(MODEL, URL, params, logformData);
  }
  static fetchOmmlog(_id, params = {}) {
    return fetchDocument(MODEL, _id, `${URL}/${_id}`, params);
  }
  static fetchOmmlogs(params = {}){
    console.log("params:",params);
    return fetchCollection(MODEL, URL, params);
  }
}
export { Ommlog , globalUsername };

