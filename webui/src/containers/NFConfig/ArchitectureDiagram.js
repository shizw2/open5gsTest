import React from 'react';

class ArchitectureDiagram extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      selectedNF: null,
    };
  }

  handleNFClick(nf) {
    this.setState({
      selectedNF: nf,
    });
  }

  render() {
    const { selectedNF } = this.state;

    return (
      <div>
        <svg width="400" height="300">
          {/* A网元 */}
          <rect
            onClick={() => this.handleNFClick({ name: 'A', config: 'A配置信息' })}
            x="50"
            y="50"
            width="100"
            height="50"
            fill={selectedNF && selectedNF.name === 'A' ? 'blue' : 'white'}
          />

          {/* B网元 */}
          <rect
            onClick={() => this.handleNFClick({ name: 'B', config: 'B配置信息' })}
            x="200"
            y="50"
            width="100"
            height="50"
            fill={selectedNF && selectedNF.name === 'B' ? 'green' : 'white'}
          />

          {/* C网元 */}
          <rect
            onClick={() => this.handleNFClick({ name: 'C', config: 'C配置信息' })}
            x="150"
            y="150"
            width="100"
            height="50"
            fill={selectedNF && selectedNF.name === 'C' ? 'yellow' : 'white'}
          />
        </svg>

        {selectedNF && (
          <div>
            <h3>{selectedNF.name} 网元配置</h3>
            <p>{selectedNF.config}</p>
          </div>
        )}

        {/* 插入PNG图片 */}
        <div>
          <img src="/ABC.png" alt="ABC" style={{ display: 'block', width: '400px', height: 'auto' }} />
        </div>
      </div>
    );
  }
}

export default ArchitectureDiagram;
