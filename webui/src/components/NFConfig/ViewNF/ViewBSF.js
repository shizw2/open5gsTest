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

      <div>
        {bsf && (
          <div className="nf-section">
            <div className="nf-header">bsf:</div>
            <NF_SBI_Section sbi={bsf.sbi} />
            <MetricsSection metrics={bsf.metrics} />
            <div className="twenty-spaces">relative_capacity: {nfconfig.bsf.relative_capacity && (nfconfig.bsf.relative_capacity)}</div>
          </div>
        )}
      </div>

      {nfconfig && nfconfig.cli && <CLISection cli={nfconfig.cli} />}
      {/*
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};