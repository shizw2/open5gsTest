import { NRFSection, SBISection,LOGSection,MetricSection } from './ViewCfgSection';

export const ViewAMF = ({ nfconfig }) => {  
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
          {nfconfig.amf.sbi && (
            <div className="sbi-section">
              <div className="twenty-spaces">sbi:</div>                                           
              {nfconfig.amf.sbi.addr && nfconfig.amf.sbi.addr.map((item, index) => (
                <div className="forty-spaces" key={index}>
                  -addr: {item}
                </div>
              ))}                  
              {nfconfig.amf.sbi.port && (
                <div className="forty-spaces">
                  port: {nfconfig.amf.sbi.port}
                </div>
              )}  
            </div>
          )} 

          <div className="twenty-spaces">ngap:</div> 
          {nfconfig.amf.ngap.addr && nfconfig.amf.ngap.addr.map((item, index) => (
                <div className="forty-spaces" key={index}>
                  -addr: {item}
                </div>
          ))}  
          
          <div className="twenty-spaces">icps:</div> 
          <div className="forty-spaces">spsnum: {nfconfig.amf.icps.spsnum}</div>
          <div className="forty-spaces">port: {nfconfig.amf.icps.port}</div>
          
          {/*<div className="twenty-spaces">metrics:</div> 
          <div className="forty-spaces">-addr: {nfconfig.metrics.addr}</div>
          <div className="forty-spaces">port: {nfconfig.metrics.port}</div>*/}
          <MetricSection metric={nfconfig.amf.metric} />
          
        </div>
      )}
      </div>

      <div className="parameter-body">parameter:</div>
      <div className="max-body">max:</div>
      <div className="time-body">time:</div>
    </div>
  );
};