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
} from './ViewCfgSection';


export const ViewSMF = ({ nfconfig }) => {  
  const { pcf, parameter, max, time } = nfconfig;
  return (
    <div>
      <LOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      <div>
        {smf && (
          <div className="nf-section">
            <div className="nf-header">smf:</div>
            <NF_SBI_Section sbi={smf.sbi} />
            <MetricsSection metrics={smf.metrics} />            
          </div>
        )}
      </div>
      
      <ParameterSection parameter={parameter} />
      <MaxSection max={max} />
      <TimeSection time={time} />
    </div>
  );
};