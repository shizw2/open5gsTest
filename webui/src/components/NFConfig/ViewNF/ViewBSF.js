import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  MetricsSection,
  MaxSection,
  ParameterSection,
  TimeSection,  
} from './ViewCfgSection';

export const ViewBSF= ({ nfconfig }) => {
  const { bsf, parameter, max, time } = nfconfig;

  return (
    <div>        
      <LOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      <div>
        {bsf && (
          <div className="nf-section">
            <div className="nf-header">bsf:</div>
            <NF_SBI_Section sbi={bsf.sbi} />
            <MetricsSection metrics={bsf.metrics} />
          </div>
        )}
      </div>
      <ParameterSection parameter={nfconfig.parameter} />

      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};