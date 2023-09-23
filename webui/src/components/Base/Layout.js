import PropTypes from 'prop-types';
import Head from 'next/head';

import styled from 'styled-components';
import oc from 'open-color';

import Header from 'containers/Header';
import Sidebar from 'containers/Sidebar';

import Package from '../../../package';
import properties from '../../../properties'; // 导入配置文件

const Body = styled.div`
  display: flex;
  height: calc(100vh - 4rem);
`

const propTypes = {
  title: PropTypes.string
}

const defaultProps = {
  //title: `${properties.base_title} ${Package.version}`
  title: `${properties.base_title}`
}

const Layout = ({ title, children }) => (
  <div>
    <Head>
      <title>{title}</title>
    </Head>
    <Header/>
    <Body>
      <Sidebar/>
      {children}
    </Body>
  </div>
)

Layout.propTypes = propTypes;
Layout.defaultProps = defaultProps;

const ContainerWrapper = styled.div`
  flex: 1;
  overflow-y: scroll;
`;

Layout.Container = ({visible, children}) => visible ? (
  <ContainerWrapper>
    {children}
  </ContainerWrapper>
) : null;

Layout.Content = styled.div`
`;

export default Layout;
