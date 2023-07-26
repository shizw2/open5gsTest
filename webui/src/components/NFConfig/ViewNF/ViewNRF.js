import { NRFSection, SBISection,LOGSection } from './ViewCfgSection';

export const ViewNRF = ({ nfconfig }) => {  
  return (
    <div>
      {/* // 使用 LOGSection 组件 */}                  
      <LOGSection nfconfig={nfconfig} />
      {/* // 使用 NRFSection 组件 */}                  
      <NRFSection nfconfig={nfconfig} />
      {/* // 使用 SBISection 组件 */}   
      <SBISection nfconfig={nfconfig} />

      <div className="parameter-body">parameter:</div>
      <div className="max-body">max:</div>
      <div className="time-body">time:</div>
    </div>
  );
};