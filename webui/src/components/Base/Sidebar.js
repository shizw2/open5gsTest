import PropTypes from 'prop-types';

import styled from 'styled-components';
import { media, transitions } from 'helpers/style-utils';
import oc from 'open-color';

import SubscriberIcon from 'react-icons/lib/md/person-add';
import Subscriber2Icon from 'react-icons/lib/md/person';
import ProfileIcon from 'react-icons/lib/md/content-copy';
import AccountIcon from 'react-icons/lib/md/vpn-key';
import ConfigIcon from 'react-icons/lib/md/settings';
import AlertIcon from 'react-icons/lib/go/alert';
import PerformanceIcon from 'react-icons/lib/go/graph';
import OprlogIcon from 'react-icons/lib/go/steps';
import ImeicheckIcon from 'react-icons/lib/go/check';
import properties from '../../../properties'; // 导入配置文件

const Menu = styled.div`
  display: block;
  width: ${p => p.visible ? p.width : '0' };
  transition: width .2s ease-in-out;
  overflow: hidden;

  position: relative;
  z-index: 1;

  ${media.mobile`
    position: absolute;
    top: 4rem;
    left: 0;
    width: ${p => p.visible ? '100%' : '0'};
    height: ${p => p.visible ? '100%' : '0'};
    transition: height .2s ease-in-out;
  `}

  background-color: ${oc.indigo[5]};
  box-shadow: 3px 3px 6px rgba(0,0,0,0.1), 3px 3px 6px rgba(0,0,0,0.2);
`;
const FilteredMenu = styled(Menu).withConfig({ shouldForwardProp: prop => prop !== 'visible' })``;
const StyledItem = styled.div`
  display: flex;
  align-items: center;
  padding : 1rem;

  transition: all .3s;

  cursor: pointer;
  color: white;
  background: ${p => p.active ? oc.indigo[7] : oc.indigo[5]};

  border-left: ${p => p.active ? `12px solid ${oc.indigo[6]}` :
    `12px solid ${oc.indigo[4]}`};

  &:hover {
    background: ${p => p.active ? oc.indigo[7] : oc.indigo[6]};
  }
`;
const FilteredStyledItem = styled(StyledItem).withConfig({ shouldForwardProp: prop => prop !== 'active' })``;
const Icon = styled.div`
  display: inline-flex;
  padding-left: 1rem;
  font-size: 1.5rem;
`;

const Title = styled.div`
  padding-left: 2rem;
  font-size: 1.2rem;
`;

const Item = ({ children, selected, name, onSelect }) => (
  <FilteredStyledItem
    onClick={() => onSelect(name)}
    active={name===selected}>
    {children}
  </FilteredStyledItem>
)

const propTypes = {
  isOpen: PropTypes.bool,
  width: PropTypes.string,
  selectedView: PropTypes.string,
  onSelectView: PropTypes.func
}

const defaultProps = {
  width: "16rem",
}

const SubMenu = styled.div` 
  padding-left: 2rem; /* 添加缩进效果 */
`;


const Sidebar = ({ isOpen, width, selectedView, onSelectView }) => (
  <FilteredMenu visible={isOpen} width={width}>
    <Item name="nfconfig" selected={selectedView} onSelect={onSelectView}>
      <Icon><ConfigIcon/></Icon>
      <Title>{properties.base_nfconfig}</Title>
    </Item>
    <Item name="operator" selected={selectedView} onSelect={onSelectView}>
      <Icon><SubscriberIcon/></Icon>
      <Title>{properties.base_operation}</Title>
    </Item>
    {(selectedView === "operator" || selectedView === "subscriber" || selectedView === "profile")&& (
      <SubMenu>
        <Item name="subscriber" selected={selectedView} onSelect={onSelectView}>
          <Icon><Subscriber2Icon/></Icon>
          <Title>{properties.base_subscriber}</Title>
        </Item>
        <Item name="profile" selected={selectedView} onSelect={onSelectView}>
          <Icon><ProfileIcon/></Icon>
          <Title>{properties.base_profile}</Title>
        </Item>
      </SubMenu>
    )}
    <Item name="alert" selected={selectedView} onSelect={onSelectView}>
      <Icon><AlertIcon/></Icon>
      <Title>{properties.base_alert}</Title>
    </Item>
    <Item name="performance" selected={selectedView} onSelect={onSelectView}>
      <Icon><PerformanceIcon/></Icon>
      <Title>{properties.base_performance}</Title>
    </Item>
    <Item name="oprlog" selected={selectedView} onSelect={onSelectView}>
      <Icon><OprlogIcon/></Icon>
      <Title>{properties.base_oprlog}</Title>
    </Item>
    <Item name="imeicheck" selected={selectedView} onSelect={onSelectView}>
      <Icon><ImeicheckIcon/></Icon>
      <Title>{properties.base_imeicheck}</Title>
    </Item>
    <Item name="account" selected={selectedView} onSelect={onSelectView}>
      <Icon><AccountIcon/></Icon>
      <Title>{properties.base_account}</Title>
    </Item>
  </FilteredMenu>
)

Sidebar.propTypes = propTypes;
Sidebar.defaultProps = defaultProps;

export default Sidebar;
