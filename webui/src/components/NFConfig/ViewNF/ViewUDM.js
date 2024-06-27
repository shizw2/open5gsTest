import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  HnetSection,
  RTSUPIInfoSection,
  MetricsSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  CLISection
} from './ViewCfgSection';

export const ViewUDM = ({ nfconfig }) => {
  const { udm, parameter, max, time } = nfconfig;

  return (
    <div>
      {/* 使用 LOGSection 组件 */}
      <LOGSection nfconfig={nfconfig} />
      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      <div>
        {udm && (
          <div className="nf-section">
            <div className="nf-header">udm:</div>
            <NF_SBI_Section sbi={udm.sbi} />
            <HnetSection hnet={udm.hnet} />
            <RTSUPIInfoSection info={udm.info} />
            <MetricsSection metrics={udm.metrics} />
          </div>
        )}
      </div>
      
      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      {nfconfig && nfconfig.cli && <CLISection cli={nfconfig.cli} />}
      <div className="twenty-spaces">relative_capacity: {udm.relative_capacity && (udm.relative_capacity)}</div>
      */}
    </div>
  );
};
