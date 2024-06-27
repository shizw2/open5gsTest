import{
  NRFSection,
  SBISection,
  LOGSection,
  MetricsSection,
  NF_SBI_Section,
  GuamiSection,
  TaiSection,
  PlmnSupportSection,
  AccessControlSection,
  ParameterSection,
  SecuritySection,
  TimeSection,
  CLISection
} from './ViewCfgSection';

export const ViewAMF = ({ nfconfig }) => {  
  console.log("nfconfig:", nfconfig); // 在这里添加打印语句
  return (
    <div>
      {/* // 使用 LOGSection 组件 */}
      <LOGSection nfconfig={nfconfig} />
      {/* // 使用 NRFSection 组件 */}
      {/*<NRFSection nfconfig={nfconfig} />*/}
      {/* // 使用 SBISection 组件 */}
      {/*<SBISection nfconfig={nfconfig} />*/}

      <div>
      {nfconfig && nfconfig.global && (
        <div className="global-section">
          <div className="global-header">global:</div>
          {nfconfig.global.cli && <CLISection cli={nfconfig.global.cli} />}
          {nfconfig.global.parameter && <ParameterSection parameter={nfconfig.global.parameter} />}
        </div>
      )}

      {nfconfig && nfconfig.amf && (
        <div className="nf-section">
          <div className="nf-header">amf:</div>
            <NF_SBI_Section  sbi={nfconfig.amf.sbi} />

            {nfconfig.amf.ngap && (
            <div>
              <div className="twenty-spaces">ngap:</div>
              <div className="forty-spaces">server:</div>
              <div className="sixty-spaces">
              {nfconfig.amf.ngap.server.map((item, index) => (
                <div key={index}>
                  <div>- address: {item.address}</div>
                </div>
              ))}
              </div>
            </div>
            )}

            {nfconfig.amf.icps && (
            <div>
              <div className="twenty-spaces">icps: </div> 
              <div className="forty-spaces">spsnum: {nfconfig.amf.icps.spsnum}</div>
              <div className="forty-spaces">port: {nfconfig.amf.icps.port}</div>
            </div>
            )}

            <MetricsSection metrics={nfconfig.amf.metrics} />
            <GuamiSection guamis={nfconfig.amf.guami} />
            <TaiSection tais={nfconfig.amf.tai} />
            <PlmnSupportSection plmnSupport={nfconfig.amf.plmn_support} />
            <AccessControlSection accessControl={nfconfig.amf.access_control} />
            <SecuritySection security={nfconfig.amf.security} />              

            {nfconfig.amf.network_name && (
            <div>
              <div className="twenty-spaces">network_name:</div>
              <div className="forty-spaces">full: {nfconfig.amf.network_name.full}</div>
              <div className="forty-spaces">short: {nfconfig.amf.network_name.short}</div>
            </div>
            )}

            {nfconfig.amf.amf_name && (
            <div>
              <div className="twenty-spaces">amf_name: {nfconfig.amf.amf_name}</div>
            </div>
            )}
            
            <div className="twenty-spaces">
              {nfconfig.amf.time && <TimeSection time={nfconfig.amf.time} />}
            </div>
          </div>
        )}
      </div>
    </div>
  );
};