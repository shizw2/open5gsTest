import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  N32_Section,
  MetricsSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  CLISection
} from './ViewCfgSection';

export const ViewSEPP= ({ nfconfig }) => {
  const { sepp, parameter, max, time } = nfconfig;

  return (
    <div>
      <LOGSection nfconfig={nfconfig} />
      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      <div>
        {sepp && (
          <div className="nf-section">
            <div className="nf-header">sepp:</div>
            <NF_SBI_Section sbi={sepp.sbi} />
            <N32_Section n32={sepp.n32} />
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