import {
  NRFSection,
  NRFSBISection,
  LOGSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  ServingSection,
  MetricsSection,
  CLISection
} from './ViewCfgSection';

export const ViewNRF = ({ nfconfig }) => {
  const {nrf, parameter, max, time } = nfconfig;
  return (
    <div>
      {/* // 使用 LOGSection 组件 */}
      <LOGSection nfconfig={nfconfig} />
      {/* // 使用 NRFSection 组件 */}
      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      <div>
        {nrf && (
          <div className="nf-section">
            <div className="nf-header">nrf:</div>   
              <NRFSBISection sbi={nrf.sbi} />
              <ServingSection serving={nrf.serving} />
              <MetricsSection metrics={nrf.metrics} />
          </div>
        )}
      </div>
      {/* // 使用 SBISection 组件 */}   
      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      <div className="twenty-spaces">relative_capacity: {nfconfig.nrf.relative_capacity && (nfconfig.nrf.relative_capacity)}</div>
      */}
    </div>
  );
};