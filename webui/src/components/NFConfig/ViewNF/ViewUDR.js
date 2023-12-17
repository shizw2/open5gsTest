import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  SUPIRangeSection,
  MaxSection,
  ParameterSection,
  TimeSection,
  FreeDiameterSection,
  MetricsSection,
  DbUriSection,
} from './ViewCfgSection';

export const ViewUDR= ({ nfconfig }) => {
  const { udr, parameter, max, time } = nfconfig;

  return (
    <div>   
      <DbUriSection db_uri={nfconfig.db_uri} />
      <LOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      <div>
        {udr && (
          <div className="nf-section">
            <div className="nf-header">udr:</div>
            <NF_SBI_Section sbi={udr.sbi} />
            <SUPIRangeSection info={udr.info} />
            <MetricsSection metrics={udr.metrics} />
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
