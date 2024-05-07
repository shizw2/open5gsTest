import {
  DbUriSection,
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  MetricsSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  CLISection
} from './ViewCfgSection';

export const ViewEIR= ({ nfconfig }) => {
  const { eir, parameter, max, time } = nfconfig;

  return (
    <div>
      <DbUriSection db_uri={nfconfig.db_uri} />
      <LOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      <div>
        {eir && (
          <div className="nf-section">
            <div className="nf-header">eir:</div>
            <NF_SBI_Section sbi={eir.sbi} />
            <MetricsSection metrics={eir.metrics} />
          </div>
        )}
      </div>
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      {nfconfig && nfconfig.cli && <CLISection cli={nfconfig.cli} />}
      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};