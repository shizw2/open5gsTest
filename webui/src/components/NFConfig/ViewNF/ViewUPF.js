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
  DPDKSection,
  CLISection
} from './ViewCfgSection';


export const ViewUPF = ({ nfconfig }) => {  
  const { smf, parameter, max, time, upf } = nfconfig;
  return (
    <div>
      <LOGSection nfconfig={nfconfig} />
      <NRFSection nfconfig={nfconfig} />
      <SBISection nfconfig={nfconfig} />

      {upf && (
        <div className="upf-section">
          <div className="upf-header">upf:</div>          
          <PfcpSection pfcp={upf.pfcp} /> 
          <GtpuSection gtpu={upf.gtpu} />
          <SubnetSection subnet={upf.subnet} />
          <DPDKSection dpdk={upf.dpdk} />
          <MetricsSection metrics={upf.metrics} /> 
        </div>
      )}

      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      {nfconfig && nfconfig.cli && <CLISection cli={nfconfig.cli} />}
      {/* 
      <div className="smf-section">
      <div className="smf-header">smf:</div>
      {smf && smf.pfcp && smf.pfcp.addr && (
        <div>
          <div className="twenty-spaces">pfcp: </div> 
          <div className="twenty-spaces">&nbsp;&nbsp;addr: {smf.pfcp.addr}</div>
        </div>
      )}
      </div>
      
            
      <div className="smf-section">
          <div className="smf-header">smf:</div>          
          <div className="smf-section">
            {smf && (
              <div className="twenty-spaces">pfcp: </div> 
              <div className="twenty-spaces">&nbsp;&nbsp;addr: {smf.pfcp.addr}</div>
            )}
          </div>
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

      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};