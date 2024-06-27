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
      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      <div>
        {eir && (
          <div className="nf-section">
            <div className="nf-header">eir:</div>
            <NF_SBI_Section sbi={eir.sbi} />
            <MetricsSection metrics={eir.metrics} />
          </div>
        )}
      </div>

      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      */}
    </div>
  );
};