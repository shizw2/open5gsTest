import {
  NRFSection,
  SBISection,
  LOGSection,
  NSSF_SBI_Section,
  MetricsSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  NSIComponent,
  CLISection
} from './ViewCfgSection';

export const ViewNSSF= ({ nfconfig }) => {
  const { nssf, parameter, max, time } = nfconfig;

  return (
    <div>   
      <LOGSection nfconfig={nfconfig} />
      {/* 
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />
      */}
      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      <div>
        {nssf && (
          <div className="nf-section">
            <div className="nf-header">nssf:</div>
            <NSSF_SBI_Section sbi={nssf.sbi} />
            <MetricsSection metrics={nssf.metrics} />
          </div>
        )}
      </div>

      {/*
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};
