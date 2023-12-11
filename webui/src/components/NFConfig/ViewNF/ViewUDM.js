import {
  NRFSection,
  SBISection,
  LOGSection,
  NF_SBI_Section,
  HnetSection,
  MaxSection,
  //ParameterSection,
  TimeSection
} from './ViewCfgSection';

export const ViewUDM = ({ nfconfig }) => {
  const { udm, parameter, max, time } = nfconfig;

  return (
    <div>
      {/* 使用 LOGSection 组件 */}
      <LOGSection nfconfig={nfconfig} />

      {/* 使用 NRFSection 组件 */}
      <NRFSection nfconfig={nfconfig} />

      {/* 使用 SBISection 组件 */}
      <SBISection nfconfig={nfconfig} />

      <div>
        {udm && (
          <div className="nf-section">
            <div className="nf-header">udm:</div>
            <NF_SBI_Section sbi={udm.sbi} />
            <HnetSection hnet={udm.hnet} />
          </div>
        )}
      </div>

      {/*
      <ParameterSection parameter={parameter} />
      <MaxSection max={max} />
      <TimeSection time={time} />
      */}
    </div>
  );
};
