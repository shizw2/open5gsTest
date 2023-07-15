import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media } from 'helpers/style-utils';

import EditIcon from 'react-icons/lib/md/edit';
import DeleteIcon from 'react-icons/lib/md/delete';
import CloseIcon from 'react-icons/lib/md/close';

import PhoneIcon from 'react-icons/lib/md/phone';
import SecurityIcon from 'react-icons/lib/md/security';
import PdnIcon from 'react-icons/lib/md/cast';
import KeyboardControlIcon from 'react-icons/lib/md/keyboard-control';

import ViewNRF from './ViewNRF';
import ViewBSF from './ViewBSF';

import { Modal, Tooltip, Dimmed } from 'components';


const View = ({ visible, disableOnClickOutside, nfconfig, onEdit, onDelete, onHide }) => {
  const _id = (nfconfig || {})._id;
  const title = (nfconfig || {}).title;
  console.log('title '+title)
  console.log('_id '+_id)
  // 根据title选择要渲染的View组件
  const renderViewComponent = () => {
    if (_id === 'nfconfig1' || _id === 'nrf') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    } else if (_id === 'nfconfig2' || _id === 'bsf') {
      return <ViewBSF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if (_id === 'nfconfig1' || _id === 'upf') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if ( _id === 'amf') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if (_id === 'ausf') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if (_id === 'nssf') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if (_id === 'pcf') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    } if (_id === 'scp') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if (_id === 'smf') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if (_id === 'udm') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    }if (_id === 'udr') {
      return <ViewNRF visible={visible} disableOnClickOutside={disableOnClickOutside} nfconfig={nfconfig} onEdit={onEdit} onDelete={onDelete} onHide={onHide} />;
    } else {
      return null; // 如果找不到匹配的title，则返回空
    }
  };

  return (
    <div>
      {/* 渲染对应的View组件 */}
      {renderViewComponent()}

      {/* 其他代码 */}
    </div>
  );
  
}

export default View;
