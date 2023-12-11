import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,  
  MaxSection,
  //ParameterSection,
  TimeSection,
  NSIComponent
} from './ViewCfgSection';

export const ViewNSSF= ({ nfconfig }) => {
  const { nssf, parameter, max, time } = nfconfig;

  return (
    <div>   
      <LOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      <div>
        {nssf && (
          <div className="nf-section">
            <div className="nf-header">nssf:</div>
            <NF_SBI_Section sbi={nssf.sbi} />
            <NSIComponent nsi={nssf.nsi} />
          </div>
        )}
      </div>

      {/*
      <ParameterSection parameter={parameter} />
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};
