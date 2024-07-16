import { Component } from 'react';
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

import NProgress from 'nprogress';

import { MODEL, fetchNFConfigs, fetchNFConfig, createNFConfig, updateNFConfig } from 'modules/crud/nfconfig';
import { clearActionStatus } from 'modules/crud/actions';
import { select, selectActionStatus } from 'modules/crud/selectors';
import * as Notification from 'modules/notification/actions';

import { NFConfig } from 'components';
import { Ommlog} from 'modules/crud/ommlog';
import traverse from 'traverse';

const formData = {
  "security": {
    k: "465B5CE8 B199B49F AA5F0A2E E238A6BC",
    amf: "8000",
    op_value: "E8ED289D EBA952E4 283B54E8 8E6183CA",
  },
  "ambr": {
    "downlink": {
      "value": 1,
      "unit": 3
    },
    "uplink": {
      "value": 1,
      "unit": 3
    }
  },
  "slice": [{
    "sst": 1,
    "default_indicator": true,
    "session": [{
        "name": "internet",
        "type": 3,
        "ambr": {
          "downlink": {
            "value": 1,
            "unit": 3
          },
          "uplink": {
            "value": 1,
            "unit": 3
          }
        },
        "qos": {
          "index": 9,
          "arp": {
            "priority_level": 8,
            "pre_emption_capability": 1,
            "pre_emption_vulnerability": 1
          }
        },
    }]
  }]
}

class Document extends Component {
  constructor(props) {
    super(props);
    const { nfconfig, dispatch,profiles } = props

    if (nfconfig.needsFetch) {
      dispatch(nfconfig.fetch)
    }
    if (profiles.needsFetch) {
      dispatch(profiles.fetch);
    }
  }
  static propTypes = {
    action: PropTypes.string,
    visible: PropTypes.bool, 
    onHide: PropTypes.func
  }

  state = {
    formData
  }
/*
  componentWillMount() {
    const { nfconfig, dispatch } = this.props

    if (nfconfig.needsFetch) {
      dispatch(nfconfig.fetch)
    }
  }
*/
  componentDidMount() {
    const { nfconfig, dispatch } = this.props

    if (nfconfig.needsFetch) {
      dispatch(nfconfig.fetch)
    }
  }
  /*
  componentWillReceiveProps(nextProps) {
    const { nfconfig, status } = nextProps
    const { dispatch, action, onHide } = this.props

    if (nfconfig.needsFetch) {
      dispatch(nfconfig.fetch)
    }

    if (nfconfig.data) {
      // Mongoose library has a problem for 64bit-long type
      //
      //   FETCH : the library returns 'Number' type for 64bit-long type
      //   CREATE/UPDATE : the library returns 'String' type for 64bit-long type
      //
      // In this case, I cannot avoid json-schema validation function
      // So, I've changed the type from 'String' to 'Number' if the key name is 'downlink' and 'uplink'
      // 
      //    The followings are changed from 'String' to 'Number' after DB CREATE or UPDATE
      //     - ambr.downlink, ambr.uplink, qos.mbr.downlink, qos.mbr.uplink, qos.gbr.downlink, qos.gbr.uplink
      // 
      //traverse(nfconfig.data).forEach(function(x) {
      //  if (this.key == 'downlink') this.update(Number(x));
      //  if (this.key == 'uplink') this.update(Number(x));
      //})

      //if (nfconfig.data.security) {
       // if (nfconfig.data.security.opc) {
      //    nfconfig.data.security.op_type = 0;
      //    nfconfig.data.security.op_value = nfconfig.data.security.opc;
      //  } else {
      //    nfconfig.data.security.op_type = 1;
       //   nfconfig.data.security.op_value = nfconfig.data.security.op;
      //  }
      //}
      this.setState({ formData: nfconfig.data })
    } else {
      this.setState({ formData });
    }

    if (status.response) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

//      const message = action === 'create' ? "New nfconfig created" : `${status.id} nfconfig updated`;
      const message = action === 'create' ? "New nfconfig created" : `This nfconfig updated`;

      dispatch(Notification.success({
        title: 'NFConfig',
        message
      }));

      dispatch(clearActionStatus(MODEL, action));
      onHide();
    } 

    if (status.error) {
      NProgress.configure({ 
        parent: 'body',
        trickleSpeed: 5
      });
      NProgress.done(true);

      const response = ((status || {}).error || {}).response || {};

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
          label: 'Dismiss',
          callback: () => onHide()
        }
      }));
      dispatch(clearActionStatus(MODEL, action));
    }
  }
*/
componentDidUpdate(prevProps) {
  const { nfconfig, status } = this.props;
  const { dispatch, action, onHide } = this.props;

  if (nfconfig.needsFetch && nfconfig.needsFetch !== prevProps.nfconfig.needsFetch) {
    dispatch(nfconfig.fetch);
  }
/*
  if (nfconfig.data && nfconfig.data !== prevProps.nfconfig.data) {     
    this.setState({ formData: nfconfig.data });
  } else if (!nfconfig.data && prevProps.nfconfig.data) {
    this.setState({ formData });
  }
*/
  if (status.response && status.response !== prevProps.status.response) {
    NProgress.configure({
      parent: 'body',
      trickleSpeed: 5
    });
    NProgress.done(true);

    // const message = action === 'create' ? "New nfconfig created" : `${status.id} nfconfig updated`;
    //const message = action === 'create' ? "New nfconfig created" : `This nfconfig updated`;

    let message;
    let title = (
      <h style={{ fontSize: "18", textAlign: 'center' }}>
        配置变更
      </h>
    );

    if (status.id ==='upf')
    {      
      //message = action === 'create' ? "New nfconfig created" : `This nfconfig updated`;
      message = (
        <p style={{ fontSize: "16", color: 'blue', textAlign: 'left' }}>
          修改SBI、IPPool、PFCP、GTP、NRF、DPDK、Metrics配置，需重启5GC
        </p>
      );
    }
    else if (status.id ==='smf')
    {      
      //message = action === 'create' ? "New nfconfig created" : `This nfconfig updated`;
      message = (
        <p style={{ fontSize: "16", color: 'blue', textAlign: 'left' }}>
          修改SBI、IPPool、PFCP、GTP、NRF、Metrics、Diameter配置，需重启5GC
        </p>
      );
    }
    else if (status.id ==='pcf')
    {
      message = (
        <p style={{ fontSize: "16", color: 'blue', textAlign: 'left' }}>
          修改SBI、NRF、Metrics、Diameter配置，需重启5GC
        </p>
      );
    }
    else if (status.id ==='amf')
    {
      message = (
        <p style={{ fontSize: "16", color: 'blue', textAlign: 'left' }}>
          修改SBI、NGAP、NRF、Metrics配置，需重启5GC
        </p>
      );
    }
    else
    {
      message = (
        <p style={{ fontSize: "16", color: 'blue', textAlign: 'left' }}>
          修改SBI、NRF、Metrics配置，需重启5GC
        </p>
      );
    }
    
    dispatch(Notification.success({
      title,
      message,
      autoDismiss: 5,
    }));

    dispatch(clearActionStatus(MODEL, action));
    onHide();
  }

  if (status.error && status.error !== prevProps.status.error) {
    NProgress.configure({
      parent: 'body',
      trickleSpeed: 5
    });
    NProgress.done(true);

    const response = ((status || {}).error || {}).response || {};

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
        label: 'Dismiss',
        callback: () => onHide()
      }
    }));
    dispatch(clearActionStatus(MODEL, action));
  }
  }
  validate = (formData, errors) => {
    const { nfconfigs, action, status } = this.props;

    if (formData._nf && formData[formData._nf].sbi && formData[formData._nf].sbi.server)
    {
      for (let i = 0; i < formData[formData._nf].sbi.server.length; i++)
      {
        let address = formData[formData._nf].sbi.server[i].address;
        //if ( !(ipToNumberV4(address) || ipToNumberV6(address)) )
        if ( !(isV4Format(address) || isV6Format(address)) )
        {
          if (formData[formData._nf].sbi.server[i].port)
          {
            errors[formData._nf].sbi.server[i].port.addError(`Non-IP`);
          }
        }
      }
    }

    if (formData.amf && formData.amf.security && formData.amf.security.integrity_order)
    {
      let NIAs = formData.amf.security.integrity_order.map(integrity_order => {
        return integrity_order;
      });

      let duplicates = {};
      for (let i = 0; i < NIAs.length; i++) {
        if (duplicates.hasOwnProperty(NIAs[i])) {
          duplicates[NIAs[i]].push(i);
        } else if (NIAs.lastIndexOf(NIAs[i]) !== i) {
          duplicates[NIAs[i]] = [i];
        }
      }
      for (let key in duplicates) {
        duplicates[key].forEach(index => 
          errors.amf.security.integrity_order[index].addError(`${key} is duplicated`));
      }
    }

    if (formData.amf && formData.amf.security && formData.amf.security.ciphering_order)
    {
      let NEAs = formData.amf.security.ciphering_order.map(ciphering_order => {
        return ciphering_order;
      });

      let duplicates = {};
      for (let i = 0; i < NEAs.length; i++) {
        if (duplicates.hasOwnProperty(NEAs[i])) {
          duplicates[NEAs[i]].push(i);
        } else if (NEAs.lastIndexOf(NEAs[i]) !== i) {
          duplicates[NEAs[i]] = [i];
        }
      }
      for (let key in duplicates) {
        duplicates[key].forEach(index => 
          errors.amf.security.ciphering_order[index].addError(`${key} is duplicated`));
      }
    }

    if (formData.udm && formData.udm.hnet)
    {
      let ids = formData.udm.hnet.map(hnet => {
        return hnet.id;
      });

      let duplicates = {};
      for (let i = 0; i < ids.length; i++) {
        if (duplicates.hasOwnProperty(ids[i])) {
          duplicates[ids[i]].push(i);
        } else if (ids.lastIndexOf(ids[i]) !== i) {
          duplicates[ids[i]] = [i];
        }
      }
      for (let key in duplicates) {
        duplicates[key].forEach(index => 
          errors.udm.hnet[index].id.addError(`${key} is duplicated`));
      }
    }

    if ( (formData.smf && formData.smf.session)
         || (formData.upf && formData.upf.session) )
    {
      let ippools;

      if ( formData.smf && formData.smf.session )
      {
        ippools = formData.smf.session.map(session => {
          return session.subnet;
        });
      }
      else if ( formData.upf && formData.upf.session )
      {
        ippools = formData.upf.session.map(session => {
          return session.subnet;
        });
      }
      
      let overlaps = {};

      for (let i = 0; i < ippools.length; i++) {
        const ippool = ippools[i];

        // 解析并计算当前子网的范围
        const [address, subnetMask] = ippool.split('/');
        const isIPv6 = address.includes(':');
        let subnetMaskBits;
        let startIP;
        let endIP;

        if (isIPv6)
        {
          subnetMaskBits = 128 - parseInt(subnetMask);
          startIP = ipToNumberV6(address);
          endIP = startIP + BigInt(Math.pow(2, subnetMaskBits)) - BigInt(1);
        }
        else  //IPv4
        {
          subnetMaskBits = 32 - parseInt(subnetMask);
          startIP = ipToNumber(address);
          endIP = startIP + Math.pow(2, subnetMaskBits) - 1;
        }

        // 检查当前子网是否与其他子网有重叠
        for (let j = i+1; j < ippools.length; j++) {
          if (i === j) continue; // 跳过自身

          const otherIPPool = ippools[j];
          const [otherAddress, otherSubnetMask] = otherIPPool.split('/');
          const isOtherIPv6 = otherAddress.includes(':');

          let otherSubnetMaskBits;
          let otherStartIP;
          let otherEndIP;

          if (isOtherIPv6)
          {
            otherSubnetMaskBits = 128 - parseInt(otherSubnetMask);
            otherStartIP = ipToNumberV6(otherAddress);
            otherEndIP = otherStartIP + BigInt(Math.pow(2, otherSubnetMaskBits)) - BigInt(1);
          }
          else  //IPv4
          {
            otherSubnetMaskBits = 32 - parseInt(otherSubnetMask);
            otherStartIP = ipToNumber(otherAddress);
            otherEndIP = otherStartIP + Math.pow(2, otherSubnetMaskBits) - 1;
          }

          if ( isIPv6 !== isOtherIPv6 ) continue;

          if (startIP <= otherEndIP && otherStartIP <= endIP)
          //if ( (isIPv6 ? startIP : BigInt(startIP)) <= (isOtherIPv6 ? otherEndIP: BigInt(otherEndIP)) 
          //    && (isOtherIPv6 ? otherStartIP : BigInt(otherStartIP)) <= (isIPv6 ? endIP : BigInt(endIP)) )
          {
            // 存在重叠，记录重叠的 addr 值和索引
            if (!overlaps[ippool]) {
              overlaps[ippool] = [i];
            }
            overlaps[ippool].push(j);
          }
        }
      }

      if (formData.smf && formData.smf.session)
      {
        for (let key in overlaps) {
          overlaps[key].forEach(index => 
            errors.smf.session[index].subnet.addError(`${key} is overlapped`));
        }
      }
      else if (formData.upf && formData.upf.session)
      {
        for (let key in overlaps) {
          overlaps[key].forEach(index => 
            errors.upf.session[index].subnet.addError(`${key} is overlapped`));
        }
      }
    }
    
    if ( (formData.upf && formData.upf.pfcp && formData.upf.gtpu)
         || (formData.smf && formData.smf.pfcp && formData.smf.gtpu && formData.smf['p-cscf']) )
    {
      let confNf;
      let confUnits = [
        'pfcp',
        'gtpu',
      ];

      if (formData.upf && formData.upf.pfcp && formData.upf.gtpu)
      {
        confNf = 'upf';
      }
      else if (formData.smf && formData.smf.pfcp && formData.smf['gtpu'])
      {
        confNf = 'smf';
      }

      for (let k=0; k<confUnits.length; k++)
      {

        let confUnit = confUnits[k];

        let addrs = formData[confNf][confUnit].server.map(item => {
          return item.address;
        });

        let duplicates = {};

        for (let i = 0; i < addrs.length; i++) {
          const addr1 = addrs[i];

          for (let j = i+1; j < addrs.length; j++) {
            const addr2 = addrs[j];
            
            if (addr1 === addr2)
            {
              // 存在重叠，记录重叠的 addr 值和索引
              if (!duplicates[addr1]) {
                duplicates[addr1] = [i];
              }
              duplicates[addr1].push(j);
            }
          }
        }

        for (let key in duplicates) {
          duplicates[key].forEach(index => 
            errors[confNf][confUnit].server[index].address.addError(`${key} is duplicated`));
        }
      }
    }


/*
    if (formData.msisdn) {
      const { msisdn } = formData;
      if (msisdn && msisdn.length > 1 && msisdn[0] === msisdn[1])
        errors.msisdn.addError(`'${msisdn[1]}' is duplicated`);
    }

    if (formData.slice) {
      let s_nssais = formData.slice.map(slice => {
        return JSON.stringify({ sst: slice.sst, sd: slice.sd })
      });
      let duplicates = {};
      for (let i = 0; i < s_nssais.length; i++) {
        if (duplicates.hasOwnProperty(s_nssais[i])) {
          duplicates[s_nssais[i]].push(i);
        } else if (s_nssais.lastIndexOf(s_nssais[i]) !== i) {
          duplicates[s_nssais[i]] = [i];
        }
      }
      for (let key in duplicates) {
        duplicates[key].forEach(index =>
          errors.slice[index].sst.addError(`${key} is duplicated`));
      }
    }

    for (let i = 0; i < formData.slice.length; i++) {
      let names = formData.slice[i].session.map(session => {
        return session.name
      });
      let duplicates = {};
      for (let j = 0; j < names.length; j++) {
        if (duplicates.hasOwnProperty(names[j])) {
          duplicates[names[j]].push(j);
        } else if (names.lastIndexOf(names[j]) !== j) {
          duplicates[names[j]] = [j];
        }
      }
      for (let key in duplicates) {
        duplicates[key].forEach(index => 
          errors.slice[i].session[index].name.addError(`'${key}' is duplicated`));
      }
    }

    if (!formData.slice.some(slice => slice.default_indicator == true)) {
      for (let i = 0; i < formData.slice.length; i++) {
        errors.slice[i].default_indicator.addError(
            `At least 1 Default S-NSSAI is required`);
      }
    }
*/
    return errors;
  }

  handleSubmit = (formData) => {
    const { dispatch, action } = this.props;

    /*if (formData.security) {
      if (formData.security.op_type === 1) {
        formData.security.op = formData.security.op_value;
        formData.security.opc = null;
      } else {
        formData.security.op = null;
        formData.security.opc = formData.security.op_value;
      }
    }*/

    NProgress.configure({ 
      parent: '#nprogress-base-form',
      trickleSpeed: 5
    });
    NProgress.start();

    if (action === 'create') {
      dispatch(createNFConfig({}, formData));
      dispatch(Ommlog.createOmmlog(action,"配置管理",{}, formData));
    } else if (action === 'update') {
      dispatch(updateNFConfig(formData._id, {}, formData));
      dispatch(Ommlog.createOmmlog(action,"配置管理",{}, formData));
    } else {
      throw new Error(`Action type '${action}' is invalid.`);
    }
  }

  handleError = errors => {
    const { dispatch } = this.props;
    errors.map(error =>
      dispatch(Notification.error({
        title: 'Validation Error',
        message: error.stack
      }))
    )
  }

  render() {
    const {
      validate,
      handleSubmit,
      handleError
    } = this;

    const { 
      visible,
      action,
      status,
      nfconfig,
      onHide
    } = this.props
    let editformData = nfconfig.data || {}; 
    return (
      <NFConfig.Edit
        visible={visible} 
        action={action}
        formData={editformData}
        //formData={this.state.formData}
        isLoading={nfconfig.isLoading && !status.pending}
        validate={validate}
        onHide={onHide}
        onSubmit={handleSubmit}
        onError={handleError} />
    )
  }
}

function ipToNumber(ipAddress) {
  const parts = ipAddress.split('.');
  return (parts[0] << 24) + (parts[1] << 16) + (parts[2] << 8) + parseInt(parts[3]);
}

function isV4Format(ipAddress){
  return /^(\d{1,3}\.){3}\d{1,3}$/.test(ipAddress);
}

function isV6Format(ipAddress){
  return /^(?:[A-F0-9]{1,4}:){7}[A-F0-9]{1,4}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,7}:(?:[A-F0-9]{1,4}:){1,7}[A-F0-9]{1,4}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,6}:[A-F0-9]{1,4}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,5}(?::[A-F0-9]{1,4}){1,2}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,4}(?::[A-F0-9]{1,4}){1,3}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,3}(?::[A-F0-9]{1,4}){1,4}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,2}(?::[A-F0-9]{1,4}){1,5}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,1}(?::[A-F0-9]{1,4}){1,6}$/i.test(ipAddress) ||
  /^(?:[A-F0-9]{1,4}:){1,7}::$/i.test(ipAddress) ||
  /^:(?::[A-F0-9]{1,4}){1,7}$/i.test(ipAddress) ||
  /^::$/i.test(ipAddress);
}

// 辅助函数：将 IPv4 地址转换为 32 位整数
function ipToNumberV4(ipAddress) {
  if (!ipAddress)
  {
    return 0;
  }

  const parts = ipAddress.split('.');
  let number = 0;
  if ( parts.length > 0 )
  {
    for (let i = 0; i < parts.length; i++) {
      number += parseInt(parts[i]) << (24 - (8 * i));
   }
  }
  return number;
}

// 辅助函数：将 IPv6 地址转换为整数
function ipToHexV6(ipAddress) {
  if (!ipAddress)
  {
    return 0;
  }

  const parts = ipAddress.split(':');
  let number = '';

  for (let i = 0; i < parts.length; i++) {
    const segment = parts[i] || '0000'; // 处理连续零位段
    const zerosToAdd = 4 - segment.length;
    const paddedSegment = '0'.repeat(zerosToAdd) + segment;
    number += paddedSegment;
  }

  return number;
}

// 辅助函数：将 IPv6 地址转换为整数
function ipToNumberV6(ipAddress) {
  if (!ipAddress)
  {
    return 0;
  }

  const parts = ipAddress.split(':');
  let number = '';
  if ( parts.length > 0 )
  {
    for (let i = 0; i < parts.length; i++) {
      const segment = parts[i] || '0000'; // 处理连续零位段
      const zerosToAdd = 4 - segment.length;
      const paddedSegment = '0'.repeat(zerosToAdd) + segment;
      number += paddedSegment;
    }
  }
  return BigInt(`0x${number}`);
}

// 判断地址池是否重叠
function checkAddressOverlap(pool1, pool2) {
  const [address1, subnet1] = pool1.split('/');
  const [address2, subnet2] = pool2.split('/');

  const startIP1 = ipToNumber(address1);
  const endIP1 = startIP1 + Math.pow(2, 32 - parseInt(subnet1)) - 1;

  const startIP2 = ipToNumber(address2);
  const endIP2 = startIP2 + Math.pow(2, 32 - parseInt(subnet2)) - 1;

  if (startIP1 <= endIP2 && startIP2 <= endIP1) {
    return true; // 有重叠
  }

  return false; // 无重叠
}

Document = connect(
  (state, props) => ({ 
    profiles: select(Ommlog.fetchOmmlogs({}), state.crud),
    nfconfigs: select(fetchNFConfigs(), state.crud),
    nfconfig: select(fetchNFConfig(props._id), state.crud),
    status: selectActionStatus(MODEL, state.crud, props.action)
  })
)(Document);

export default Document;
