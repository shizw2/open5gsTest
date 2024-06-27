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
  SmfPfcpSection,
  SubnetSection,
  SessionSection,
  GtpcSection,
  GtpuSection,
  DnsSection,
  PcscfSection,
  MtuSection,
  CtfSection,
  FreeDiameterFileSection,
  SMFInfoSection,
  CLISection,
} from './ViewCfgSection';


export const ViewSMF = ({ nfconfig }) => {  
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

      <div>
        {smf && (
          <div className="nf-section">
            <div className="nf-header">smf:</div>
            <NF_SBI_Section sbi={smf.sbi} />
            <SmfPfcpSection pfcp={smf.pfcp} />
            <SessionSection session={smf.session} />
            <GtpcSection gtpc={smf.gtpc} />
            <GtpuSection gtpu={smf.gtpu} />
            <MetricsSection metrics={smf.metrics} />
            <DnsSection dns={smf.dns} />
            <PcscfSection p-cscf={smf['p-cscf']} />
            <MtuSection mtu={smf.mtu} />
            <CtfSection ctf={smf.ctf} />
            <FreeDiameterFileSection freediameter={smf.freediameter} />
            <SMFInfoSection info={smf.info} />
          </div>
        )}
      </div>
{/*
      {upf && ( 
        <div className="upf-section">
          <div className="upf-header">upf:</div>
            {upf.pfcp && <PfcpSection pfcp={upf.pfcp} />}
        </div>
      )}
      {nfconfig && nfconfig.cli && <CLISection cli={nfconfig.cli} />}
*/}

      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      <div className="twenty-spaces">relative_capacity: {smf.relative_capacity && (smf.relative_capacity)}</div>
      */}
    </div>
  );
};