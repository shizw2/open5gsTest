import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  HnetSection,
  MaxSection,
  //ParameterSection,
  TimeSection,
  DbUriSection
} from './ViewCfgSection';

export const ViewSCP = ({ nfconfig }) => {
  const { scp, parameter, max, time } = nfconfig;

  return (
    <div>
      <DbUriSection db_uri={nfconfig.db_uri} />
      <LOGSection nfconfig={nfconfig} />      
     
      <div>
        {scp && (
          <div className="nf-section">
            <div className="nf-header">scp:</div>
            <NF_SBI_Section sbi={scp.sbi} />
          </div>
        )}
      </div>
      
      <NRFSection nfconfig={nfconfig} />

      {/*
      <ParameterSection parameter={parameter} />
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};
