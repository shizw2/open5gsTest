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
        <img src="5g-architecture.png" alt="5G Architecture Diagram" />

        <div>
          {selectedNF && (
            <div>
              <h3>{selectedNF.name} Configuration</h3>
              <p>Configuration details for {selectedNF.name}</p>
            </div>
          )}
        </div>

        <button onClick={() => this.handleNFClick({ name: 'AMF' })}>AMF</button>
        <button onClick={() => this.handleNFClick({ name: 'SMF' })}>SMF</button>
        <button onClick={() => this.handleNFClick({ name: 'NRF' })}>NRF</button>
      </div>
    );
  }
}

export default ArchitectureDiagram;
