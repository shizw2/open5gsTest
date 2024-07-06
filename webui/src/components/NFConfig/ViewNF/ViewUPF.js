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
  UpfPfcpSection,
  GtpcSection,
  GtpuSection,
  MtuSection,
  CtfSection,
  FreeDiameterFileSection,
  InfoSection,
  SessionSection,
  DPDKSection,
  CLISection
} from './ViewCfgSection';


export const ViewUPF = ({ nfconfig }) => {  
  const { smf, parameter, max, time, upf } = nfconfig;
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
      {upf && (
        <div className="upf-section">
          <div className="upf-header">upf:</div>          
          <UpfPfcpSection pfcp={upf.pfcp} />
          <GtpuSection gtpu={upf.gtpu} />
          <SessionSection session={upf.session} />
          <DPDKSection dpdk={upf.dpdk} />
          <MetricsSection metrics={upf.metrics} />
        </div>
      )}

      {/* 
      <div className="smf-section">
      <div className="smf-header">smf:</div>
      {smf && smf.pfcp && smf.pfcp.addr && (
        <div>
          <div className="twenty-spaces">relative_capacity: {upf.relative_capacity && (upf.relative_capacity)}</div>
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