import {
  DbUriSection,
  NRFSection,
  SBISection,
  OldLOGSection,
  OldNF_SBI_Section,
  OldMetricsSection,
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
      <OldLOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      <div>
        {eir && (
          <div className="nf-section">
            <div className="nf-header">eir:</div>
            <OldNF_SBI_Section sbi={eir.sbi} />
            <OldMetricsSection metrics={eir.metrics} />
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