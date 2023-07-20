import { NRFSection, SBISection,LOGSection,MetricsSection,NF_SBI_Section,GuamiSection,TaiSection,PlmnSupportSection,SecuritySection ,TimeSection} from './ViewCfgSection';

export const ViewAUSF = ({ nfconfig }) => {  
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
      {nfconfig && nfconfig.ausf && (
        <div className="nf-section">
          <div className="nf-header">ausf:</div>             
          
              <NF_SBI_Section  sbi={nfconfig.ausf.sbi} />
                      
        </div>
      )}
      </div>

      <div className="parameter-body">parameter:</div>
      <div className="max-body">max:</div>
      <TimeSection time={nfconfig.time} />      
    </div>
  );
};