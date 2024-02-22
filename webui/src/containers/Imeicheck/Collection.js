import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import { MODEL, fetchImeichecks, deleteImeicheck } from 'modules/crud/imeicheck';
import { Ommlog} from 'modules/crud/ommlog';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';
import styled from 'styled-components';
import oc from 'open-color';
import FreshIcon from 'react-icons/lib/md/refresh';
import { Tooltip } from 'components';
import { getStatusName, getReasonName } from 'components/Imeicheck/getName';
import { 
  Layout, 
  Imeicheck, 
  Spinner, 
  FloatingButton, 
  Blank,
  Dimmed,
  Confirm
} from 'components';

import Document from './Document';
const Tabalehead = styled.div`
  display: flex;
  flex-direction: row;
  flex: 0 0 auto;
  line-height: 2rem;
  margin : 1rem 2rem;
  background: ${oc.gray[2]};
  position: sticky;
  top: 0;
  left: 0;
  right: 0;
  z-index: 1;
  .order {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 110px;
    min-width: 110px;
    margin : 0rem 0rem 0rem 2rem;    
  }
  .imei {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    width: 220px;
    min-width: 220px;
    background-color: ${oc.gray[2]};
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .status {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 160px;
    min-width: 160px;
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .reason {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 160px;
    min-width: 160px;
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .checkflag {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 160px;
    min-width: 160px;
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .imsi {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 300px;
    min-width: 300px;    
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .refresh-button-container{
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};     
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: right;
  }
`;
const CircleButton = styled.div`
  height: 2rem;
  width: 2rem;
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 1px;

  color: ${oc.gray[6]};

  border-radius: 1rem;
  font-size: 1.5rem;

  &:hover {
    color: ${oc.blue[6]};
  }

  &.refresh {
    &:hover {
      color: ${oc.blue[6]};
    }
  }
`;
class Collection extends Component {
  constructor(props) {
    super(props);
 
  this.state = {
    document: {
      action: '',
      visible: false,
      dimmed: false
    },
    confirm: {
      visible: false,
      imei: ''
    },
    view: {
      visible: false,
      disableOnClickOutside: false,
      imei: ''
    },
    originalData: this.props.imeichecks.data, 
    data: this.props.imeichecks.data,
    searchimei: "",
    searchstatus: "",
    searchreason: "",
    searchcheckflag: "",
    searchimsi: "",
    isimeiDropdownOpen: false,
    isstatusDropdownOpen: false,
    isreasonDropdownOpen: false,
    ischeckflagDropdownOpen: false,
    isimsiDropdownOpen: false,
    };
    const { imeichecks, dispatch } = props

    if (imeichecks.needsFetch) {
      dispatch(imeichecks.fetch)
    }
  }
  componentDidMount() {
    const { imeichecks, dispatch } = this.props

    if (imeichecks.needsFetch) {
      dispatch(imeichecks.fetch)
    }
  }

  componentDidUpdate(prevProps) {
    const { imeichecks, status, dispatch } = this.props;
    if (imeichecks !== prevProps.imeichecks) {
      this.setState({
        originalData: imeichecks.data,
        data: imeichecks.data
      });
    }
    if (imeichecks.needsFetch&& imeichecks.needsFetch !== prevProps.imeichecks.needsFetch) {
      dispatch(imeichecks.fetch);
    }

    if (status.response && status.response !== prevProps.status.response) {
      dispatch(Notification.success({
        title: '黑白名单',
        message: 'This imei has been deleted'
      }));
      dispatch(clearActionStatus(MODEL, 'delete'));
    }

    if (status.error && status.error !== prevProps.status.error) {
      const response = status.error.response || {};
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
          label: 'Dismiss'
        }
      }));
      dispatch(clearActionStatus(MODEL, 'delete'));
    }
  }
  documentHandler = {
    show: (action, payload) => {
      this.setState({
        document: {
          action,
          visible: true,
          dimmed: true,
          ...payload
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: true
        }
      })
    },
    hide: () => {
      this.setState({
        document: {
          action: '',
          visible: false,
          dimmed: false
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: false
        }
      })
    },
    actions: {
      create: () => {
        this.documentHandler.show('create');
      },
      update: (imei) => {
        this.documentHandler.show('update', { imei });
      }
    }
  }

  confirmHandler = {
    show: (imei) => {
      this.setState({
        confirm: {
          visible: true,
          imei,
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: true
        }
      })
    },
    hide: () => {
      this.setState({
        confirm: {
          ...this.state.confirm,
          visible: false
        },
        view: {
          ...this.state.view,
          disableOnClickOutside: false
        }
      })
    },
    actions : {
      delete: () => {
        const { dispatch } = this.props

        if (this.state.confirm.visible === true) {
          this.confirmHandler.hide();
          this.documentHandler.hide();
          this.viewHandler.hide();          
          dispatch(deleteImeicheck(this.state.confirm.imei));
          dispatch(Ommlog.createOmmlog('delete',"黑白名单",{},{},"被删除IMEI- "+this.state.confirm.imei));
        }
      }
    }
  }

  viewHandler = {
    show: (imei) => {
      this.setState({
        view: {
          imei,
          visible: true,
          disableOnClickOutside: false
        }
      });
    },
    hide: () => {
      this.setState({
        view: {
          ...this.state.view,
          visible: false
        }
      })
    }
  }

//处理数据变化
  handleDatachange=(imei,status,reason,checkflag,imsi)=>{
    //console.log("input:",imei,status,reason,checkflag,imsi)
    let  flag=""
    if(checkflag==="是")flag=true
    else if(checkflag==="否")flag=false
    //console.log("flag:",flag)

    this.setState(prevState => ({
      ...prevState,   
      data: prevState.originalData.filter(item => { 
        //console.log("item:", item.status);     
         return (item.imei.includes(imei)&&
         getStatusName(item.status).includes(status)&&
         getReasonName(item.reason).includes(reason)&&
         (item.checkflag===flag||flag==="")&&
         item.bindimsi.map(item => item.imsi).join("-").includes(imsi)
         );       
     })          
    }));
    //console.log("this.state.data",this.state.data)
  }
  //处理imei变化
  openimeiDropdown = () => {
    this.setState({ isimeiDropdownOpen: true });
  }  
  handleimeiClick = () => {
    this.openimeiDropdown();  
  }
  handleInputChange = (e) => {
    this.setState({ searchimei: e.target.value });
  };
  handleInputKeyUp = (e) => {
    if (e.keyCode === 13) { // 检查是否按下了回车键
      const searchimei = e.target.value;
      this.setState({ 
        isimeiDropdownOpen: false 
      });
      this.handleDatachange(searchimei,
        this.state.searchstatus,
        this.state.searchreason,
        this.state.searchcheckflag,
        this.state.searchimsi)
      }
      
  };
  //处理状态变化
  openstatusDropdown = () => {
    this.setState({ isstatusDropdownOpen: true });
  }  
  handlestatusClick = () => {
    this.openstatusDropdown();
  }
  handlestatusOptionChange = (e) => {
    const selectedOption = e.target.value;
   // console.log("handlestatusOptionChange",selectedOption)
    this.setState({ 
      searchstatus: selectedOption, 
      isstatusDropdownOpen: false 
    });
    this.handleDatachange(this.state.searchimei,
      selectedOption,
      this.state.searchreason,
      this.state.searchcheckflag,
      this.state.searchimsi)    
  }
  //处理原因变化
  openreasonDropdown = () => {
    this.setState({ isreasonDropdownOpen: true });
  }
  handlereasonClick = () => {    
    this.openreasonDropdown()      
  }

  handlereasonOptionChange = (e) => {    
    const selectedOption = e.target.value;
    //("handlereasonOptionChange",selectedOption)
    this.setState({ 
      searchreason: selectedOption, 
      isreasonDropdownOpen: false,   
    }); 
    this.handleDatachange(this.state.searchimei,
      this.state.searchstatus,
      selectedOption,
      this.state.searchcheckflag,
      this.state.searchimsi)
    };
  //处理checkflag变化
  opencheckflagDropdown = () => {
    this.setState({ ischeckflagDropdownOpen: true });
  }
  handlecheckflagClick = () => {    
    this.opencheckflagDropdown()      
  }

  handlecheckflagOptionChange = (e) => {    
    const selectedOption = e.target.value;
    this.setState({ 
      searchcheckflag: selectedOption, 
      ischeckflagDropdownOpen: false,   
    }); 
    this.handleDatachange(this.state.searchimei,
      this.state.searchstatus,
      this.state.searchreason,
      selectedOption,
      this.state.searchimsi)
    };
  //处理imsi变化
  openimsiDropdown = () => {
    this.setState({ isimsiDropdownOpen: true });
  }
  handleimsiClick = () => {
    this.openimsiDropdown();  
      }
  handleimsiInputChange = (e) => {
    this.setState({ searchimsi: e.target.value });
  };
  handleimsiInputKeyUp = (e) => {
    if (e.keyCode === 13) { // 检查是否按下了回车键
      const searchimsi = e.target.value;
      this.setState({ 
        isimsiDropdownOpen: false 
      });
      this.handleDatachange(this.state.searchimei,
        this.state.searchstatus,
        this.state.searchreason,
        this.state.searchcheckflag,
        searchimsi)
      }
  };
  handleRefreshClick=()=>{    
    this.setState({      
      searchimei: "",
      searchstatus: "",
      searchreason: "",
      searchcheckflag: "",
      searchimsi: "",
      isimeiDropdownOpen: false,
      isstatusDropdownOpen: false,
      isreasonDropdownOpen: false,
      ischeckflagDropdownOpen: false,
      isimsiDropdownOpen: false,
    })
    this.handleDatachange("","","","","")
  }

  render() {
    const {
      documentHandler,
      viewHandler,
      confirmHandler
    } = this;

    const { 
      document,
      data,
      originalData
    } = this.state;

    const { 
      imeichecks,
      status
    } = this.props

    const {
      isLoading
    } = imeichecks;
    const styles = {      
      body: {
        backgroundColor: '#5C7CFA',
      },
      topm: {
        margin: '0px 30px 20px 10px',
      },
      select:{ 
        border: 'none', 
        outline: 'none',
        width: '90px',
        background: 'transparent', 
        fontFamily: 'Inter, Helvetica, Arial, sans-serif', 
        fontSize: '1.1rem', 
        fontWeight: 500 ,
        color: '#868E96',
        paddingTop: '4px',
        paddingBottom: '4px' 
        },
      input:{ 
        width: '160px',
        height: '30px',          
        background: 'transparent',         
        border: '1px solid #CCC'         
      },
      selectval:{
        fontSize: '1rem',
        color: '#5C7CFA',
        cursor: 'pointer'
      }
    }
    //("data:",data)
    return (
      <div>
        <Tabalehead>
          <div className="order">序号</div>
          <div className="imei"onClick={this.handleimeiClick}>
          IMEI
          <div ></div>
          {this.state.isimeiDropdownOpen ? ( 
            <input style={{ ...styles.input, ...styles.selectval }}
              type="text"
              value={this.state.searchimei}
              onChange={this.handleInputChange}
              onKeyUp={this.handleInputKeyUp}  
            />
            ):(              
              <span style={styles.selectval}>{this.state.searchimei}</span>
            )}
          </div>
 
          <div className="status"onClick={this.handlestatusClick}>
            状态
            <div ></div>
            {this.state.isstatusDropdownOpen ? (  
            <select style={{ ...styles.select, ...styles.selectval }}
              value={this.state.searchstatus} 
              onChange={this.handlestatusOptionChange} 
            >
              <option value=""></option>
              <option value="白名单">白名单</option>
              <option value="黑名单">黑名单</option>
              <option value="灰名单">灰名单</option>           
            </select>
            ):(            
              <span style={styles.selectval}>{this.state.searchstatus}</span>
            )}
          </div>          
          <div className="reason"onClick={this.handlereasonClick}>
            原因
            <div ></div>
            {this.state.isreasonDropdownOpen ? (  
            <select style={{ ...styles.select, ...styles.selectval }}
              value={this.state.searchreason} 
              onChange={this.handlereasonOptionChange}              
            >
              <option value=""></option>
              <option value="丢失">丢失</option>
              <option value="被偷">被偷</option>
              <option value="被抢">被抢</option>
              <option value="非法厂商">非法厂商</option>
              <option value="未知原因">未知原因</option>
            </select>
            ):(
              <span style={styles.selectval}>{this.state.searchreason}</span>
            )}
          </div>
          <div className="checkflag"onClick={this.handlecheckflagClick}>
            是否绑定IMSI
            <div ></div>
            {this.state.ischeckflagDropdownOpen ? (  
            <select style={{ ...styles.select, ...styles.selectval }}
              value={this.state.searchcheckflag} 
              onChange={this.handlecheckflagOptionChange}              
            >
              <option value=""> </option>
              <option value="是">是</option>
              <option value="否">否</option>
            </select>
            ):(
              <span style={styles.selectval}>{this.state.heckflag}</span>
            )}
          </div>
          <div className="imsi"onClick={this.handleimsiClick}>
          IMSI
          <div ></div>
          {this.state.isimsiDropdownOpen ? ( 
            <input style={{ ...styles.input, ...styles.selectval }}
              type="text"
              value={this.state.searchimsi}
              onChange={this.handleimsiInputChange}
              onKeyUp={this.handleimsiInputKeyUp}  
            />
            ):(              
              <span style={styles.selectval}>{this.state.searchimsi}</span>
            )}
          </div>
          <div className="refresh-button-container">
            <Tooltip content='刷新' width="60px" bottom={`${true}`}>
            <CircleButton className='refresh' onClick={this.handleRefreshClick}>
            <FreshIcon/>
            </CircleButton>
            </Tooltip>
          </div>              
        </Tabalehead>
        <Layout.Content>
          <Imeicheck.List
            imeichecks={data}
            deletedId={status.id}
            onView={viewHandler.show}
            onEdit={documentHandler.actions.update}
            onDelete={confirmHandler.show}
          />
          {isLoading && <Spinner md />}
          <Blank
            visible={!isLoading && !(Object.keys(originalData).length > 0)}
            title="添加一黑白名单"
            body="你还没有黑白名单配置!"
            onTitle={documentHandler.actions.create}
            />
          <FloatingButton onClick={documentHandler.actions.create}/> 
          <Imeicheck.View
            visible={this.state.view.visible}
            imeicheck={data.filter(imeicheck => 
              imeicheck.imei === this.state.view.imei)[0]}
            disableOnClickOutside={this.state.view.disableOnClickOutside}
            onEdit={documentHandler.actions.update}
            onDelete={confirmHandler.show}
            onHide={viewHandler.hide}/>     
          <Document 
            { ...document }
            onEdit={documentHandler.actions.update}
            onDelete={confirmHandler.show}
            onHide={documentHandler.hide} />
          <Dimmed visible={document.dimmed} />
          <Confirm
            visible={this.state.confirm.visible}
            message="删除此黑白名单配置?"
            onOutside={confirmHandler.hide}
            buttons={[
              { text: "CANCEL", action: confirmHandler.hide, info:`${true}` },
              { text: "DELETE", action: confirmHandler.actions.delete, danger:`${true}` }
            ]}/>
        </Layout.Content>
      </div>
    )
  }
}

Collection = connect(
  (state) => ({ 
    imeichecks: select(fetchImeichecks(), state.crud),
    status: selectActionStatus(MODEL, state.crud, 'delete')
  })
)(Collection);

export default Collection;
