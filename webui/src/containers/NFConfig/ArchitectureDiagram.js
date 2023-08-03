import React from 'react';

class ArchitectureDiagram extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      selectedNF: null,
      rectCoordinates: null,
    };
  }

  handleNFClick(nf, event) {
    event.stopPropagation(); // 阻止事件向上冒泡
    const rect = event.currentTarget.getBoundingClientRect();
    const x = rect.x; // 获取矩形左上角的x坐标
    const y = rect.y; // 获取矩形左上角的y坐标
    console.log("handleNFClick,offsetX:"+x+"offsetY:"+y+"nf:",nf)
    
    this.setState({
    selectedNF: nf,
    rectCoordinates: { x, y },
    });
  
    this.props.onSelectedNFChange(nf);
    this.props.onRectCoordinatesChange({ x, y });  
    // 调用viewHandler的show方法，传递A的_id作为参数
    this.props.onViewHandlerShow(nf);
    
  }

  handleSvgClick(event) {
    const svgRect = event.currentTarget.getBoundingClientRect();
    const offsetX = event.clientX - svgRect.left;
    const offsetY = event.clientY - svgRect.top;
    console.log("handleSvgClick,offsetX:"+offsetX+"offsetY:"+offsetY)
    this.setState({
      selectedNF: null,
      rectCoordinates: { x: offsetX, y: offsetY },
    });
  }

  render() {
    const { selectedNF, rectCoordinates } = this.state;

    return (
      <div>
        <svg width="400" height="300" onClick={(event) => this.handleSvgClick(event)}>
          {/* 架构图背景 */}
          <image href="/ABC.svg" width="400" height="300" />
          
          {/* A网元 */}
          <g onClick={(event) => this.handleNFClick('A', event)}>
             <rect x="35" y="57" width="136" height="44" fill={selectedNF === 'A' ? 'gray' : 'transparent'} opacity="0.5" style={{ mixBlendMode: 'multiply' }} />
            {/*<text x="75" y="80" fill="blue">A</text>*/}
          </g>

          {/* B网元 */}
          <g onClick={(event) => this.handleNFClick('B', event)}>
            <rect x="230" y="57" width="136" height="44" fill={selectedNF === 'B' ? 'gray' : 'transparent'} opacity="0.5" style={{ mixBlendMode: 'multiply' }}/>
           
          </g>

          {/* C网元 */}
          <g onClick={(event) => this.handleNFClick('C', event)}>
            <rect x="107" y="190" width="136" height="44" fill={selectedNF === 'C' ? 'gray' : 'transparent'} opacity="0.5" style={{ mixBlendMode: 'multiply' }}/>
           
          </g>

          
        </svg>

        {selectedNF && (
          <div>
            <h3>{selectedNF} 网元配置</h3>
            <p>{selectedNF}配置信息</p>
          </div>
        )}

        {rectCoordinates && (
          <div>
            <h3>选中坐标</h3>
            <p>x: {rectCoordinates.x}, y: {rectCoordinates.y}</p>
          </div>
        )}
      </div>
    );
  }
}

export default ArchitectureDiagram;
