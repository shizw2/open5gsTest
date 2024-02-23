import PropTypes from 'prop-types';
import React, { Component } from 'react';
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
`;
const Button = styled.button`
  padding: 0.5rem 1rem;
  margin: 0 0.25rem;
  background: ${oc.gray[2]};
  color: ${oc.gray[6]};
  border: none;
  border-radius: 4px;
  cursor: pointer;
  &:hover {
    color:white;
    background: ${oc.indigo[6]};
  }
`;
const Pagination = styled.div`
  display: flex;
  font-size: 1rem;
  align-items: center;
  justify-content: center;
  margin-top: 1rem;
  color: ${oc.gray[6]};
`;
const PageInfo = styled.div`
  margin: 0 0.5rem;
`;
const propTypes = {
  profiles: PropTypes.arrayOf(PropTypes.object),
  onView: PropTypes.func,
  onEdit: PropTypes.func,
  onDelete: PropTypes.func,
}
/*
const List = ({ imeichecks, deletedId, onView, onEdit, onDelete }) => {
  const imeicheckList = imeichecks
    .map(imeicheck =>
      <Item 
        key={imeicheck.imei}
        imeicheck={imeicheck}
        disabled={deletedId===imeicheck.imei}
        onView={onView}
        onEdit={onEdit}
        onDelete={onDelete} />
    );

  return (
    <Wrapper>
      {imeicheckList}
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
    this.checkFirstPage(this.props.imeichecks);
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
    const { imeichecks } = this.props;
    const totalPages = Math.ceil(imeichecks.length / this.itemsPerPage);
    this.setState({ currentPage: totalPages });
  };

  componentDidMount() {
    this.checkFirstPage(this.props.imeichecks);
  }

  componentDidUpdate(prevProps) {
    if (prevProps.imeichecks !== this.props.imeichecks) {
      this.checkFirstPage(this.props.imeichecks);
    }
  }
  checkFirstPage(imeichecks) {
    const { currentPage } = this.state;
    if (currentPage !== 1) {
      this.handleFirstPage();
    }
  }
  render() {
    const { imeichecks, deletedId, onView, onEdit, onDelete } = this.props;
    const { currentPage } = this.state;   

    const reversedimeichecks = imeichecks.slice().reverse();
    // 计算总页数
    const totalPages = Math.ceil(reversedimeichecks.length / this.itemsPerPage);
  
    // 根据当前页码和每页显示的数量提取相应的数据
    const startIndex = (currentPage - 1) * this.itemsPerPage;
    const endIndex = startIndex + this.itemsPerPage;
    const currentimeichecks = reversedimeichecks.slice(startIndex, endIndex);
  
    const imeicheckList = currentimeichecks.map((imeicheck, index) => (
      <Item
        key={imeicheck.imei}
        imeicheck={imeicheck}
        disabled={deletedId===imeicheck.imei}
        onView={onView}
        onEdit={onEdit}
        onDelete={onDelete}
        index={startIndex + index + 1}
      />
    ));
  
    return (
      <div>
        <Wrapper>{imeicheckList}</Wrapper>
        <Pagination>
          <Button
            disabled={currentPage === 1|| totalPages === 0}
            onClick={this.handleFirstPage} // 添加点击事件处理程序
          >
            首页
          </Button>
          <Button
            disabled={currentPage === 1|| totalPages === 0}
            onClick={this.handlePrevPage}
          >
            前一页
          </Button>
          <PageInfo>
            当前页 {currentPage} 总页数 {totalPages}
          </PageInfo>
          <Button
            disabled={currentPage === totalPages|| totalPages === 0}
            onClick={this.handleNextPage}
          >
            后一页
          </Button>
          <Button
          disabled={totalPages===0}
          onClick={this.handleLastPage} // 添加点击事件处理程序
          >
          末页
          </Button>
          <PageInfo>
            总记录数 {imeichecks.length} 
          </PageInfo>
        </Pagination>
      </div>
    );
  }
}  
List.propTypes = propTypes;

export default List;
