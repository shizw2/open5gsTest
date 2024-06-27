import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  RTSUPIInfoSection,
  MetricsSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  CLISection
} from './ViewCfgSection';

export const ViewAUSF = ({ nfconfig }) => {  
  const {parameter, max, time } = nfconfig;
  return (
    <div>
      {/* // 使用 LOGSection 组件 */}
      <LOGSection nfconfig={nfconfig} />
      {/* // 使用 NRFSection 组件 */}
      {/*<NRFSection nfconfig={nfconfig} />*/}
      {/* // 使用 SBISection 组件 */}
      {/*<SBISection nfconfig={nfconfig} />*/}

      <div>
      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      {nfconfig && nfconfig.ausf && (
        <div className="nf-section">
          <div className="nf-header">ausf:</div>             
            <NF_SBI_Section  sbi={nfconfig.ausf.sbi} />
            <RTSUPIInfoSection info={nfconfig.ausf.info} />
            <MetricsSection metrics={nfconfig.ausf.metrics} />
        </div>
      )}
      </div>
      
      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      <div className="twenty-spaces">relative_capacity: {nfconfig.ausf.relative_capacity && (nfconfig.ausf.relative_capacity)}</div>
      */}
    </div>
  );
};