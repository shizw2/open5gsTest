import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,  
  MaxSection,
  ParameterSection,
  TimeSection,
  FreeDiameterSection,
  MetricsSection,
  SUPIRangeSection,
  DbUriSection,
  CLISection
} from './ViewCfgSection';

export const ViewPCF= ({ nfconfig }) => {
  const { pcf, parameter, max, time } = nfconfig;

  return (
    <div> 
      <DbUriSection db_uri={nfconfig.db_uri} />
      <LOGSection nfconfig={nfconfig} />

      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      <div>
        {pcf && (
          <div className="nf-section">
            <div className="nf-header">pcf:</div>
            <NF_SBI_Section sbi={pcf.sbi} />
            <FreeDiameterSection freeDiameter={pcf.freeDiameter} />
            <SUPIRangeSection info={pcf.info} />
            <MetricsSection metrics={pcf.metrics} />
          </div>
        )}
      </div>
      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      <div className="twenty-spaces">relative_capacity: {pcf.relative_capacity && (pcf.relative_capacity)}</div>
      */}
    </div>
  );
};
