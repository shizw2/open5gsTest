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
  PfcpSection,
  GtpcSection,
  GtpuSection,
  MtuSection,
  CtfSection,
  FreeDiameterFileSection,
  InfoSection,
  SubnetSection,
} from './ViewCfgSection';


export const ViewUPF = ({ nfconfig }) => {  
  const { smf, parameter, max, time, upf } = nfconfig;
  return (
    <div>
      <LOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      <div className="upf-section">
          <div className="upf-header">upf:</div>          
          <PfcpSection pfcp={upf.pfcp} /> 
          <GtpuSection gtpu={upf.gtpu} />
          <SubnetSection subnet={upf.subnet} />
          <MetricsSection metrics={upf.metrics} /> 
      </div>
      
      <div>  
        <div className="nf-section">
          <div className="nf-header">smf:</div>
          <div className="smf-section">
            {smf && (
              <div>
                <NF_SBI_Section sbi={smf.sbi} />
                <PfcpSection pfcp={smf.pfcp} />               
              </div>
            )}
          </div>
        </div>
      </div> 
      
      
      <ParameterSection parameter={parameter} />
      <MaxSection max={max} />
      <TimeSection time={time} />
    </div>
  );
};