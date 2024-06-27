import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  SUPIRangeSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  FreeDiameterSection,
  MetricsSection,
  DbUriSection,
  CLISection
} from './ViewCfgSection';

export const ViewUDR= ({ nfconfig }) => {
  const { udr, parameter, max, time } = nfconfig;

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
        {udr && (
          <div className="nf-section">
            <div className="nf-header">udr:</div>
            <NF_SBI_Section sbi={udr.sbi} />
            <SUPIRangeSection info={udr.info} />
            <MetricsSection metrics={udr.metrics} />
          </div>
        )}
      </div>
      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      <div className="twenty-spaces">relative_capacity: {udr.relative_capacity && (udr.relative_capacity)}</div>
      */}
    </div>
  );
};
