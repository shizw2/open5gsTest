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
  SubnetSection,
  GtpcSection,
  GtpuSection,
  DnsSection,
  PcscfSection,
  MtuSection,
  CtfSection,
  FreeDiameterFileSection,
  SMFInfoSection,
  CLISection
} from './ViewCfgSection';


export const ViewSMF = ({ nfconfig }) => {  
  const { smf, parameter, max, time, upf } = nfconfig;
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
            <PfcpSection pfcp={smf.pfcp} />
            <SubnetSection subnet={smf.subnet} />
            {/*<GtpcSection gtpc={smf.gtpc} />*/}
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

      {upf && ( 
        <div className="upf-section">
          <div className="upf-header">upf:</div>
            {upf.pfcp && <PfcpSection pfcp={upf.pfcp} />}
        </div>
      )}
      {parameter && <ParameterSection parameter={nfconfig.parameter} />}
      {nfconfig && nfconfig.cli && <CLISection cli={nfconfig.cli} />}
      {/*
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};