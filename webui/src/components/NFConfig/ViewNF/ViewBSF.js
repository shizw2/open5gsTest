import {
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

export const ViewBSF= ({ nfconfig }) => {
  const { bsf, parameter, max, time } = nfconfig;

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
        {bsf && (
          <div className="nf-section">
            <div className="nf-header">bsf:</div>
            <NF_SBI_Section sbi={bsf.sbi} />
            <MetricsSection metrics={bsf.metrics} />
          </div>
        )}
      </div>

      {/*
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      <div className="twenty-spaces">relative_capacity: {nfconfig.bsf.relative_capacity && (nfconfig.bsf.relative_capacity)}</div>
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};