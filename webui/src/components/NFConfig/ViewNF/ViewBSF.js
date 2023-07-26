import { NRFSection, SBISection,LOGSection } from './ViewCfgSection';

export const ViewBSF = ({ nfconfig }) => {  
  return (
    <div>
      {/* // 使用 LOGSection 组件 */}                  
      <LOGSection nfconfig={nfconfig} />

      {/* // 使用 SBISection 组件 */}   
      <SBISection nfconfig={nfconfig} />

      <div>
      {nfconfig && nfconfig.bsf && (
        <div className="bsf-section">
          <div className="bsf-header">bsf:</div>                      
          {nfconfig.bsf.sbi && (
            <div className="sbi-section">
              <div className="twenty-spaces">sbi:</div>                                           
              {nfconfig.bsf.sbi.addr && nfconfig.bsf.sbi.addr.map((item, index) => (
                <div className="forty-spaces" key={index}>
                  -addr: {item}
                </div>
              ))}                  
              {nfconfig.bsf.sbi.port && (
                <div className="forty-spaces">
                  port: {nfconfig.bsf.sbi.port}
                </div>
              )}  
            </div>
          )} 
        </div>
      )}
      </div>

      {/* // 使用 NRFSection 组件 */}                  
      <NRFSection nfconfig={nfconfig} />
      
      <div className="parameter-body">parameter:</div>
      <div className="max-body">max:</div>
      <div className="time-body">time:</div>
    </div>
  );
};