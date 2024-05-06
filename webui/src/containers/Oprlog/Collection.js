import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';
import { MODEL, Ommlog } from 'modules/crud/ommlog';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';
import styled from 'styled-components';
import oc from 'open-color';
import FreshIcon from 'react-icons/lib/md/refresh';
import Dropdown from 'react-icons/lib/md/arrow-drop-down-circle';
import { Tooltip } from 'components';
import { 
  Layout, 
  Oprlog,
  Spinner,   
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
  margin : 1rem 7rem 0rem 3.1rem;
  background: ${oc.gray[2]};
  position: sticky;
  top: 0;
  left: 0;
  right: 0;
  z-index: 1;
  
  .opttime {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    width: 320px;
    min-width: 320px;
    background-color: ${oc.gray[2]};
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .opuser {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 220px;
    min-width: 220px;
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .optype {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 220px;
    min-width: 220px;
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .optcommand {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 120px;
    min-width: 120px;
  }
  .optfm {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 120px;
    min-width: 120px;    
    &:hover {
      color: ${oc.blue[6]};
      cursor: pointer; 
    }
  }
  .optorder {
    font-size: 1.25rem;
    color: ${oc.gray[6]};
    background-color: ${oc.gray[2]};
    width: 120px;
    min-width: 120px;
    margin : 0rem 0rem 0rem 2rem;    
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
      originalData: this.props.oprlogs.data, 
      data: this.props.oprlogs.data,
      searchOpruser: '',
      searchOprtype: '',
      searchOprfm: '',
      endTime: '',
    document: {
      action: '',
      visible: false,
      dimmed: false
    },
    confirm: {
      visible: false,
      _id: ''
    },
    view: {
      visible: false,
      disableOnClickOutside: false,
      _id: ''
      },
      isFmDropdownOpen: false,
      shouldFmCloseOnBlur: false,
      isoptypeDropdownOpen: false,
      isopusrDropdownOpen: false,
      fmSelectedOption: "",
      isDatePickerOpen: false,
  };
  const { oprlogs, dispatch } = props;

  if (oprlogs.needsFetch) {
    dispatch(oprlogs.fetch);
  }
  }
/*
  componentWillMount() {
    const { oprlogs, dispatch } = this.props    
    if (oprlogs.needsFetch) {
      dispatch(oprlogs.fetch)
    }
  }
 */
  componentDidMount() {
    const { oprlogs, dispatch } = this.props;
 
    if (oprlogs.needsFetch) {
      dispatch(oprlogs.fetch);
    }
  }
  componentDidUpdate(prevProps) {
    const { oprlogs, status, dispatch } = this.props;

    if (oprlogs !== prevProps.oprlogs) {
      this.setState({
        originalData: oprlogs.data,
        data: oprlogs.data
      });
    }

    if (oprlogs.needsFetch && oprlogs.needsFetch !== prevProps.oprlogs.needsFetch) {
      dispatch(oprlogs.fetch);
    }

    if (status.error && status.error !== prevProps.status.error) {
      let title = 'Unknown Code';
      let message = 'Unknown Error';
      if (status.response?.data?.name && status.response?.data?.message) {
        title = status.response.data.name;
        message = status.response.data.message;
      } else {
        title = status.response?.status ?? 'Unknown Status';
        message = status.response?.statusText ?? 'Unknown Status Text';
      }
      dispatch(clearActionStatus(MODEL, 'delete'));
    }
  }
  /*
  componentWillReceiveProps(nextProps) {
    const { oprlogs, status } = nextProps
    const { dispatch } = this.props
    this.setState({
      originalData: oprlogs.data,
      data: oprlogs.data
    });
    if (oprlogs.needsFetch) {
      dispatch(oprlogs.fetch)
    }
    if (status.error) {
      let title = 'Unknown Code';
      let message = 'Unknown Error';
      if (response.data && response.data.name && response.data.message) {
        title = response.data.name;
        message = response.data.message;
      } else {
        title = response.status;
        message = response.statusText;
      }
      dispatch(clearActionStatus(MODEL, 'delete'));
    }
  }
*/
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
      update: (_id) => {
        this.documentHandler.show('update', { _id });
      }
    }
  }

  confirmHandler = {
    show: (_id) => {
      this.setState({
        confirm: {
          visible: true,
          _id,
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
    }
    
  }

  viewHandler = {
    show: (_id) => {
      this.setState({
        view: {
          _id,
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
  handleDatachange=(opty,optt,opu,opfm)=>{
    let dateRangeStart;
    switch (optt) {
      case "一天":
        dateRangeStart = new Date();
        dateRangeStart.setDate(dateRangeStart.getDate() - 1);
        break;
      case "一周":
        dateRangeStart = new Date();
        dateRangeStart.setDate(dateRangeStart.getDate() - 7);
        break;
      case "一个月":
        dateRangeStart = new Date();
        dateRangeStart.setMonth(dateRangeStart.getMonth() - 1);
        break;
      default:
        dateRangeStart = new Date(1970);
        break;
    }
    this.setState(prevState => ({
      ...prevState,   
      data: prevState.originalData.filter(item => {      
         return (item.opuser[0].includes(opu)&&
         item.optfm[0].includes(opfm)&&
         new Date(item.opttime[0])>=dateRangeStart&&
         item.optype[0].includes(opty));       
     })          
    }));
  }
  openDatePicker = () => {
    this.setState({ isDatePickerOpen: true });
  }
  closeDatePicker = () => {
    this.setState({ isDatePickerOpen: false });
  }
  handleTimeClick = () => {
    this.openDatePicker();    
  }
  handletimepeOptionChange = (e) => {
    const time = e.target.value;
    const currentDate = new Date();
    this.setState({ 
      endTime: time, 
      isDatePickerOpen: false 
    });
    this.handleDatachange(this.state.searchOprtype,time,this.state.searchOpruser,this.state.fmSelectedOption)   
    }
  openoprtypeDropdown = () => {
    this.setState({ isoptypeDropdownOpen: true });
  }  
  closeoprtypeDropdown = () => {
    this.setState({ isoptypeDropdownOpen: false });
        }
  handleOprtypeClick = () => {
    this.openoprtypeDropdown();
        }
  handleoprtypeOptionChange = (e) => {
    const selectedOption = e.target.value;
    this.setState({ 
      searchOprtype: selectedOption, 
      isoptypeDropdownOpen: false 
    });
    this.handleDatachange(selectedOption,this.state.endTime,this.state.searchOpruser,this.state.fmSelectedOption)    
  }
  openFmDropdown = () => {
    this.setState({ isFmDropdownOpen: true });
  }  
  closeFmDropdown = () => {
    this.setState({ isFmDropdownOpen: false });
  }
  handleOprfmClick = () => {    
    this.openFmDropdown()      
  }

  handlefmOptionChange = (e) => {    
    const selectedOption = e.target.value;
    this.setState({ 
      fmSelectedOption: selectedOption, 
      isFmDropdownOpen: false,
      shouldFmCloseOnBlur: true 
    }); 
    this.handleDatachange(this.state.searchOprtype,this.state.endTime,this.state.searchOpruser,selectedOption)
    };
  openusrDropdown = () => {
    this.setState({ isopusrDropdownOpen: true });
  }  
  closeusrDropdown = () => {
    this.setState({ isopusrDropdownOpen: false });
  }
  handleOpruserClick = () => {
    this.openusrDropdown();  
      }
  handleInputChange = (e) => {
    this.setState({ searchOpruser: e.target.value });
  };
  handleInputKeyUp = (e) => {
    if (e.keyCode === 13) { // 检查是否按下了回车键
      const searchOpruser = e.target.value;
      this.setState({ 
        isopusrDropdownOpen: false 
      });
      this.handleDatachange(this.state.searchOprtype,this.state.endTime,searchOpruser,this.state.fmSelectedOption)
      }
  };
  handleRefreshClick=()=>{    
    this.setState({      
      searchOpruser: '',
      searchOprtype: '',
      searchOprfm: '',      
      endTime: '',
      isFmDropdownOpen: false,
      shouldFmCloseOnBlur: false,
      isoptypeDropdownOpen: false,
      isopusrDropdownOpen: false,
      fmSelectedOption: "",
      isDatePickerOpen: false,
    })
    this.handleDatachange('','','','')
  }
  handleSaveFileClick=()=>{  
    const header = [ '操作时间', '操作账号', '操作类型', '功能模块','操作对象']; // 表头
    const csvData = `${header.join(',')}\n${this.state.data.map(item => {
      return Object.values(item).slice(1, 6).map(value => `"${String(value).replace(/"/g, '""')}"`).join(',');
    }).join('\n')}`;
    // 创建一个新的Blob对象，将CSV数据作为其内容
    const blob = new Blob([csvData], { type: 'text/csv' });
    const downloadLink = document.createElement('a');
    const timestamp = new Date().getTime(); // 获取当前时间的时间戳
    const date = new Date(timestamp);
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0'); // 将月份补零至两位数
    const day = String(date.getDate()).padStart(2, '0'); // 将日期补零至两位数
    
    const filename = `log_${year}${month}${day}.csv`; // 文件名以年月日格式
  
    downloadLink.href = URL.createObjectURL(blob);
    downloadLink.download = filename;
    downloadLink.click();
  }
  render() {
    const {
      documentHandler,
      viewHandler,
      confirmHandler
    } = this;

    const { 
      document,
      data
    } = this.state;

    const { 
      oprlogs,
      status
    } = this.props
    
    const {
      isLoading
    } = oprlogs

    const styles = {
      underline: {
        width: '100%',
        height: '2px',    
        backgroundColor: '#5C7CFA',
      },
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
  return (  
    <div>                   
     <Tabalehead>
          <div className="optorder">序号</div>
          <div className="opttime"onClick={this.handleTimeClick}>
          操作时间
          <div ></div>
          {this.state.isDatePickerOpen ? (
            <select style={{ ...styles.select, ...styles.selectval }}
            value={this.state.endTime} 
            onChange={this.handletimepeOptionChange} 
            >
            <option value=""></option>
            <option value="一天">一天</option>
            <option value="一周">一周</option>
            <option value="一个月">一个月</option>              
          </select>        
          ):(            
            <span style={styles.selectval}>{this.state.endTime}</span>
          )}
          </div>
          <div className="opuser"onClick={this.handleOpruserClick}>
            操作账号
            <div ></div>
            {this.state.isopusrDropdownOpen ? ( 
            <input style={{ ...styles.input, ...styles.selectval }}
              type="text"
              value={this.state.searchOpruser}
              onChange={this.handleInputChange}
              onKeyUp={this.handleInputKeyUp}  
            />
            ):(              
              <span style={styles.selectval}>{this.state.searchOpruser}</span>
            )}
            </div>
          <div className="optype"onClick={this.handleOprtypeClick}>
            操作类型
            <div ></div>
            {this.state.isoptypeDropdownOpen ? (  
            <select style={{ ...styles.select, ...styles.selectval }}
              value={this.state.searchOprtype} 
              onChange={this.handleoprtypeOptionChange} 
            >
              <option value=""></option>
              <option value="新增">新增</option>
              <option value="修改">修改</option>
              <option value="删除">删除</option>
              <option value="登入">登入</option>
              <option value="登出">登出</option>
              <option value="警示">警示</option>               
            </select>
          ):(            
            <span style={styles.selectval}>{this.state.searchOprtype}</span>
          )}
          </div>          
          <div className="optfm"onClick={this.handleOprfmClick}>
            功能模块
          <div ></div>
            {this.state.isFmDropdownOpen ? (  
            <select style={{ ...styles.select, ...styles.selectval }}
              value={this.state.fmSelectedOption} 
              onChange={this.handlefmOptionChange}              
            >
              <option value=""></option>
              <option value="配置管理">配置管理</option>
              <option value="用户管理">用户管理</option>
              <option value="签约模板">签约模板</option>
              <option value="账号管理">账号管理</option>
              <option value="黑白名单">黑白名单</option>
            </select>
          ):(
            <span style={styles.selectval}>{this.state.fmSelectedOption}</span>
          )}
          </div>
          <div className="refresh-button-container">
            <Tooltip content='刷新' width="60px" bottom={`${true}`}>
            <CircleButton className='refresh' onClick={this.handleRefreshClick}>
            <FreshIcon/>
            </CircleButton>
            </Tooltip>
          </div>
          <div className="refresh-button-container">
            <Tooltip content='导出' width="60px" bottom={`${true}`}>         
            <CircleButton className='refresh' onClick={this.handleSaveFileClick}>
              <Dropdown/>            
              </CircleButton> 
            </Tooltip>          
          </div>               
    </Tabalehead>                       
    <Layout.Content style={styles.topm}>              
      <Oprlog.List         
        oprlogs={data}          
              onView={viewHandler.show}            
      />
      <Oprlog.View
        visible={this.state.view.visible}
        oprlog={data.filter(oprlog => 
        oprlog._id === this.state.view._id)[0]}
        disableOnClickOutside={this.state.view.disableOnClickOutside}
        onHide={viewHandler.hide}/>
      <Document 
        { ...document }
        onHide={documentHandler.hide} />
      <Dimmed visible={document.dimmed} />           
    </Layout.Content>
  </div>
)
}
}

Collection = connect( 
  (state) => ({ 
    oprlogs: select(Ommlog.fetchOmmlogs({}), state.crud),
    status: selectActionStatus(MODEL, state.crud, 'delete')
  })
)(Collection);

export default Collection;
