import { NRFSection, SBISection,LOGSection,MetricsSection,NF_SBI_Section,GuamiSection,TaiSection,PlmnSupportSection,SecuritySection ,TimeSection} from './ViewCfgSection';

export const ViewAMF = ({ nfconfig }) => {  
  console.log("nfconfig:", nfconfig); // 在这里添加打印语句
  return (
    <div>
      {/* // 使用 LOGSection 组件 */}                  
      <LOGSection nfconfig={nfconfig} />
      {/* // 使用 NRFSection 组件 */}                  
      <NRFSection nfconfig={nfconfig} />
      {/* // 使用 SBISection 组件 */}   
      <SBISection nfconfig={nfconfig} />

      <div>
      {nfconfig && nfconfig.amf && (
        <div className="nf-section">
          <div className="nf-header">amf:</div>             
          
              <NF_SBI_Section  sbi={nfconfig.amf.sbi} />

              <div className="twenty-spaces">ngap:</div> 
              <div className="forty-spaces">
              {nfconfig.amf.ngap.map((item, index) => (
                <div key={index}>
                  <div>-addr: {item.addr}</div>
                </div>
              ))}
              </div>          
              
              <div className="twenty-spaces">icps: </div> 
              <div className="forty-spaces">spsnum: {nfconfig.amf.icps.spsnum}</div>
              <div className="forty-spaces">port: {nfconfig.amf.icps.port}</div>
              

              <MetricsSection metrics={nfconfig.amf.metrics} />
              <GuamiSection guamis={nfconfig.amf.guami} />              
              <TaiSection tais={nfconfig.amf.tai} />      
              <PlmnSupportSection plmnSupport={nfconfig.amf.plmn_support} />      
              <SecuritySection security={nfconfig.amf.security} />              
              
              <div className="twenty-spaces">network_name:</div>
              <div className="forty-spaces">full: {nfconfig.amf.network_name.full}</div>
              <div className="forty-spaces">short: {nfconfig.amf.network_name.short}</div>

              <div className="twenty-spaces">amf_name: {nfconfig.amf.amf_name}</div>  
                      
        </div>
      )}
      </div>
      <TimeSection time={nfconfig.time} />   
    </div>
  );
};