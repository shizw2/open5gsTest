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
    console.log("handleNFClick,offsetX:" + x + " offsetY:" + y + " nf:", nf)

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
    console.log("handleSvgClick,offsetX:" + offsetX + " offsetY:" + offsetY)
    this.setState({
      selectedNF: null,
      rectCoordinates: { x: offsetX, y: offsetY },
    });
  }

  render() {
    const { selectedNF, rectCoordinates } = this.state;
    // 网元数量
    const numElements = 10;
    
    const scale = 0.85;
    const w = 125 * scale;
    const h = 60 * scale;
    const selectedNFArray = ['nssf', 'udm', 'nrf', 'pcf', 'bsf', 'udr', 'ausf', 'amf', 'smf', "upf"];
    return (
      <div style={{ display: 'flex', justifyContent: 'center' }}>
        {/* 添加一个居中对齐的容器 */}
        <svg width={"1200"*scale} height={"800"*scale} onClick={(event) => this.handleSvgClick(event)}>
          {/* 架构图背景 */}
          <image href="/5GC.png" width={"1200"*scale} height={"800"*scale} />

          {/* NF网元1 */}
          {Array.from({ length: numElements }, (_, index) => {
            let x, y;
            if (index === 9) {
              x = 770 * scale; // 特殊情况下的x坐标
              y = 560 * scale; // 特殊情况下的y坐标
            } else {
              //x = index < 5 ? 56 + index * 183 : 133 + (index - 5) * 190; // 计算x坐标
              //y = index < 5 ? 155 : 353; // 计算y坐标
              x = index < 5 ? 57  * scale+ index * 183 * scale : 134 * scale + (index - 5) * 190 * scale; // 计算x坐标
              y = index < 5 ? 156 * scale : 354 * scale; // 计算y坐标	
            }

            return (
              <g key={index} onClick={(event) => this.handleNFClick(selectedNFArray[index], event)}>
                <rect
                  x={x}
                  y={y}
                  width={w}
                  height={h}
                  fill={selectedNF === selectedNFArray[index] ? 'green' : 'transparent'}
                  opacity="0.5"
                  style={{ mixBlendMode: 'multiply' }}
                  data-selected-index={index}
                />
              </g>
            );
          })}
        </svg>

        {rectCoordinates && (
          <div style={{ position: 'absolute', bottom: '-40px', left: '50%', transform: 'translateX(-50%)' }}>
            <h3>选中坐标</h3>
            <p>x: {rectCoordinates.x}, y: {rectCoordinates.y}</p>
          </div>
        )}
      </div>
    );
  }
}

export default ArchitectureDiagram;
