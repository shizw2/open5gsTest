import React, { Component } from 'react';
import PropTypes from 'prop-types';

import styled from 'styled-components';
import oc from 'open-color';
import { media } from 'helpers/style-utils';

import { Layout, Blank } from 'components';
import Item from './Item';
const Wrapper = styled.div`
  display: block;
  margin: 2rem;
  ${media.mobile`
    margin: 0.5rem 0.25rem;    
  `}
`
const Pagination = styled.div`
  display: flex;
  align-items: center;
  justify-content: center;
  margin-top: 1rem;
`;

const Button = styled.button`
  padding: 0.5rem 1rem;
  margin: 0 0.25rem;
  background: ${oc.indigo[6]};
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
`;

const PageInfo = styled.div`
  margin: 0 0.5rem;
`;
const propTypes = {
  oprlogs: PropTypes.arrayOf(PropTypes.object),
  onView: PropTypes.func,
}

/* 
const List = ({ oprlogs, onView}) => {
  const oprlogList = oprlogs
    .map((oprlog, index) =>
      <Item 
        key={oprlog._id}
        oprlog={oprlog}
        onView={onView}
        index={index + 1}        
      />
    );

  return (
    <Wrapper>
      {oprlogList}
    </Wrapper>
  )
}
*/
class List extends Component {
  constructor(props) {
    super(props);
    this.state = {
      currentPage: 1,
    };
  }

  itemsPerPage = 10;

  handlePrevPage = () => {
    this.setState((prevState) => ({
      currentPage: prevState.currentPage - 1,
    }));
  };

  handleNextPage = () => {
    this.setState((prevState) => ({
      currentPage: prevState.currentPage + 1,
    }));
  };
  handleFirstPage = () => {
    this.setState({ currentPage: 1 });
  };
  
  handleLastPage = () => {
    const { oprlogs } = this.props;
    const totalPages = Math.ceil(oprlogs.length / this.itemsPerPage);
    this.setState({ currentPage: totalPages });
  };
  render() {
    const { oprlogs, onView } = this.props;
    const { currentPage } = this.state;
    
    // 对 oprlogs 数组进行倒序排序
    const reversedOprlogs = oprlogs.slice().reverse();
    
    // 计算总页数
    const totalPages = Math.ceil(reversedOprlogs.length / this.itemsPerPage);
  
    // 根据当前页码和每页显示的数量提取相应的数据
    const startIndex = (currentPage - 1) * this.itemsPerPage;
    const endIndex = startIndex + this.itemsPerPage;
    const currentOprlogs = reversedOprlogs.slice(startIndex, endIndex);
  
    const oprlogList = currentOprlogs.map((oprlog, index) => (
      <Item
        key={oprlog._id}
        oprlog={oprlog}
        onView={onView}
        index={startIndex + index + 1}
      />
    ));
  
    return (
      <div>
        <Wrapper>{oprlogList}</Wrapper>
        <Pagination>
          <Button
            disabled={currentPage === 1}
            onClick={this.handleFirstPage} // 添加点击事件处理程序
          >
            首页
          </Button>
          <Button
            disabled={currentPage === 1}
            onClick={this.handlePrevPage}
          >
            前一页
          </Button>
          <PageInfo>
            当前页 {currentPage} 总页数 {totalPages}
          </PageInfo>
          <Button
            disabled={currentPage === totalPages}
            onClick={this.handleNextPage}
          >
            后一页
          </Button>
          <Button
          disabled={currentPage === totalPages}
          onClick={this.handleLastPage} // 添加点击事件处理程序
          >
          末页
          </Button>
        </Pagination>
      </div>
    );
  }
}  
List.propTypes = propTypes;

export default List;
