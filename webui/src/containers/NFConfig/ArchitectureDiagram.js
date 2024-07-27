import React from 'react';
const selectedNFArray = ['nssf', 'udm', 'nrf', 'pcf', 'bsf', 'sepp', 'udr', 'ausf', 'amf', 'smf', 'eir', 'upf'];
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

  handleMouseOver(index) {
    
    this.setState({
      selectedNF: selectedNFArray[index],
    });
  }

  handleMouseOut() {
    this.setState({
      selectedNF: null,
    });
  }

  render() {
    const { selectedNF, rectCoordinates } = this.state;
    // 网元数量
    const numElements = 12;
    
    const scale = 0.84;
    const w = 125 * scale;
    const h = 60 * scale;
    return (
      <div style={{ display: 'flex', justifyContent: 'center' }}>
        {/* 添加一个居中对齐的容器 */}
        <svg width={"1200"*scale} height={"800"*scale} onClick={(event) => this.handleSvgClick(event)}>
          <defs>
            <filter id="shadow" x="-10%" y="-10%" width="120%" height="120%">
              <feDropShadow dx="0" dy="0" stdDeviation="20" floodColor="rgba(0, 0, 0, 0)" />
            </filter>
          </defs>
          {/* 架构图背景 */}
          <image href="/5GC.png" width={"1200"*scale} height={"800"*scale} />

          {/* NF网元1 */}
          {Array.from({ length: numElements }, (_, index) => {
            let x, y;
            if (index === 11) {
              x = 770 * scale; // 特殊情况下的x坐标
              y = 560 * scale; // 特殊情况下的y坐标
            } else {
              //x = index < 5 ? 56 + index * 183 : 133 + (index - 5) * 190; // 计算x坐标
              //y = index < 5 ? 155 : 353; // 计算y坐标
              x = index < 6 ? 57  * scale+ index * 183 * scale : 134 * scale + (index - 6) * 190 * scale; // 计算x坐标
              y = index < 6 ? 156 * scale : 354 * scale; // 计算y坐标	
            }

            return (
              <g
                key={index}
                onClick={(event) => this.handleNFClick(selectedNFArray[index], event)}
                onMouseOver={() => this.handleMouseOver(index)}
                onMouseOut={() => this.handleMouseOut()}
              >
                <rect
                  x={x}
                  y={y}
                  width={w}
                  height={h}
                  fill={selectedNF === selectedNFArray[index] ? 'orange' : 'transparent'}
                  opacity="0.5"
                  style={{
                    mixBlendMode: 'multiply',
                    cursor: 'pointer',
                    filter: selectedNF === selectedNFArray[index] ? 'url(#shadow)' : 'none',
                    //boxShadow: selectedNF === selectedNFArray[index] ? '0 0 200px rgba(0, 0, 0, 0.5)' : 'none',
                  }}
                  data-selected-index={index}
                />
              </g>
            );
          })}
        </svg>

        {/*
        {rectCoordinates && (
          <div style={{ position: 'absolute', bottom: '-40px', left: '50%', transform: 'translateX(-50%)' }}>
            <h3>选中坐标</h3>
            <p>x: {rectCoordinates.x}, y: {rectCoordinates.y}</p>
          </div>
        )}
        */}
      </div>
    );
  }
}

export default ArchitectureDiagram;
