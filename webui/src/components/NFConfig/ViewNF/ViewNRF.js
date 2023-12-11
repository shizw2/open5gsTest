import {
  NRFSection,
  SBISection,
  LOGSection, 
  MaxSection,
  //ParameterSection,
  TimeSection, 
} from './ViewCfgSection';

export const ViewNRF = ({ nfconfig }) => {
  const {parameter, max, time } = nfconfig;
  return (
    <div>
      {/* // 使用 LOGSection 组件 */}                  
      <LOGSection nfconfig={nfconfig} />
      {/* // 使用 NRFSection 组件 */}                  
      <NRFSection nfconfig={nfconfig} />
      {/* // 使用 SBISection 组件 */}   
      <SBISection nfconfig={nfconfig} />

      {/*
      <ParameterSection parameter={parameter} />
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};