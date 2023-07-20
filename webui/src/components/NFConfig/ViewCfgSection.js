export const NRFSection = ({ nfconfig }) => (
  <div>
    {nfconfig && nfconfig.nrf && (
      <div className="nrf-section">
        <div className="nrf-header">nrf:</div>
        {nfconfig.nrf.sbi && (
          <div className="sbi-section">
            <div className="twenty-spaces">sbi:</div>
            {Array.isArray(nfconfig.nrf.sbi) ? (
              // 如果 nfconfig.nrf.sbi 是数组
              nfconfig.nrf.sbi.map((item, index) => (
                <div key={index}>
                  {item.addr && item.addr.map((addrItem, addrIndex) => (
                    <div className="forty-spaces" key={addrIndex}>
                      -addr: {addrItem}
                    </div>
                  ))}
                  {item.port && (
                    <div className="forty-spaces">
                      port: {item.port}
                    </div>
                  )}
                </div>
              ))
            ) : (
              // 如果 nfconfig.nrf.sbi 是对象
              <div>
                {nfconfig.nrf.sbi.addr && nfconfig.nrf.sbi.addr.map((item, index) => (
                  <div className="forty-spaces" key={index}>
                    addr: {item}
                  </div>
                ))}
                {nfconfig.nrf.sbi.port && (
                  <div className="forty-spaces">
                    port: {nfconfig.nrf.sbi.port}
                  </div>
                )}
              </div>
            )}
          </div>
        )}
      </div>
    )}
  </div>
);


export const SBISection = ({ nfconfig }) => (
  <div>
	  {nfconfig && nfconfig.sbi && (
		<div className="sbi-section">
		  <div className="sbi-header">sbi:</div>
		  <div className="sbi-body">                  
			{nfconfig.sbi.server && (
			  <div className="sbi-detail">
				<div className="twenty-spaces">server:</div>                                      
				  <div className="forty-spaces">no_tls:{String(nfconfig.sbi.server.no_tls)}</div> 
				  <div className="forty-spaces">no_verify:{String(nfconfig.sbi.server.no_verify)}</div>
				  <div className="forty-spaces">cacert:{nfconfig.sbi.server.cacert}</div>
				  <div className="forty-spaces">key:{nfconfig.sbi.server.key}</div>
				  <div className="forty-spaces">cert:{nfconfig.sbi.server.cert}</div>
			  </div>
			)}                  
		  </div>
		</div>
	  )}
  </div>
);

export const LOGSection = ({ nfconfig }) => (
  <div>
    {nfconfig && nfconfig.logger && (
      <div className="logger-section">
        <div className="logger-header">logger:</div>
        <div className="logger-body">
          <div className="eight-spaces">
            file: {nfconfig.logger.file}
          </div>
          <div className="eight-spaces">
            level: {nfconfig.logger.level}
          </div>
        </div>
      </div>
    )}
  </div>
);

export const MetricsSection = ({ metrics }) => (
  <div>
    {metrics && (
      <div className="metrics-section">
        <div className="twenty-spaces">metrics:</div>
        <div className="forty-spaces">
          {metrics.map((item, index) => (
            <div key={index}>
              <div>-addr: {item.addr}</div>
              <div>&nbsp;port: {item.port}</div>
            </div>
          ))}
        </div>
      </div>
    )}
  </div>
);

//NF下的sbi，如AMF，SMF内部的SBI配置
export const NF_SBI_Section = ({ sbi }) => (
  <div>
    {sbi && (
      <div className="sbi-section">
        <div className="twenty-spaces">sbi:</div>
        <div className="forty-spaces">
          {sbi.map((item, index) => (
            <div key={index}>
              <div>-addr: {item.addr}</div>
              <div>&nbsp;port: {item.port}</div>
            </div>
          ))}
        </div>
      </div>
    )}
  </div>
);

export const GuamiSection = ({ guamis }) => (
  <div>
    {guamis && (
      <div className="guami-section">
        <div className="twenty-spaces">guami:</div>
        <div className="forty-spaces">
          {guamis.map((item, index) => (
            <div key={index}>
              <div>-plmn_id:</div>
              <div className="twenty-spaces">mcc: {item.plmn_id.mcc}</div>
              <div className="twenty-spaces">mnc: {item.plmn_id.mnc}</div>
              <div>&nbsp;amf_id:</div>
              <div className="twenty-spaces">region: {item.amf_id.region}</div>
              <div className="twenty-spaces">set: {item.amf_id.set}</div>
            </div>
          ))}
        </div>
      </div>
    )}
  </div>
);

export const TaiSection = ({ tais }) => (
  <div>
    {tais && (
      <div className="tai-section">
        <div className="twenty-spaces">tai:</div>
        <div className="forty-spaces">
          {tais.map((item, index) => (
            <div key={index}>
              <div>-plmn_id:</div>
              <div className="twenty-spaces">mcc: {item.plmn_id.mcc}</div>
              <div className="twenty-spaces">mnc: {item.plmn_id.mnc}</div>
              <div>&nbsp;tac: {item.tac}</div>
            </div>
          ))}
        </div>
      </div>
    )}
  </div>
);

export const PlmnSupportSection = ({ plmnSupport }) => (
  <div>
    {plmnSupport && (
      <div className="plmn-support-section">
        <div className="twenty-spaces">plmn_support:</div>
        <div className="forty-spaces">
          {plmnSupport.map((item, index) => (
            <div key={index}>
              <div>-plmn_id:</div>
              <div className="twenty-spaces">mcc: {item.plmn_id.mcc}</div>
              <div className="twenty-spaces">mnc: {item.plmn_id.mnc}</div>
              {item.s_nssai && (
                <div>
                  <div>&nbsp;s_nssai:</div>
                  {item.s_nssai.map((nssai, nssaiIndex) => (
                    <div className="twenty-spaces" key={nssaiIndex}>
                      -sst: {nssai.sst}
                    </div>
                  ))}
                </div>
              )}
            </div>
          ))}
        </div>
      </div>
    )}
  </div>
);

export const SecuritySection = ({ security }) => (
  <div>
    {security && (
      <div className="security-section">
        <div className="twenty-spaces">security:</div>
        <div className="forty-spaces">
          <div>integrity_order : [{security.integrity_order.join(', ')}]</div>
          <div>ciphering_order : [{security.ciphering_order.join(', ')}]</div>
        </div>
      </div>
    )}
  </div>
);


export const TimeSection = ({ time }) => (
  <div>
      <div className="time-section">
        <div className="time-header">time:</div>  
        {time.t3512 && (
            <div className="twenty-spaces">
              <div>t3512:</div>
              <div className="twenty-spaces">value: {time.t3512.value}</div>
            </div>       
        )}
    
        {time.nf_instance && (     
            <div className="twenty-spaces">
              <div>nf_instance:</div>
              <div className="twenty-spaces">heartbeat: {time.nf_instance.heartbeat}</div>
            </div>  
        )}
      </div>
  </div>
);
