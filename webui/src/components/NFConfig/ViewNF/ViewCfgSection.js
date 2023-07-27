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
        {time && time.t3512 && (
            <div className="twenty-spaces">
              <div>t3512:</div>
              <div className="twenty-spaces">value: {time.t3512.value}</div>
            </div>       
        )}
    
        {time && time.nf_instance && (     
            <div className="twenty-spaces">
              <div>nf_instance:</div>
              <div className="twenty-spaces">heartbeat: {time.nf_instance.heartbeat}</div>
            </div>  
        )}
      </div>
  </div>
);

export const HnetSection = ({ hnet }) => (
  <div>
    {hnet && (
      <div className="hnet-section">
        <div className="hnet-header">hnet:</div>
        <div className="hnet-body">
          {hnet.map((entry) => (
            <div className="hnet-entry" key={entry.id}>
              <div className="twenty-spaces">id: {entry.id}</div>
              <div className="twenty-spaces">scheme: {entry.scheme}</div>
              <div className="twenty-spaces">key: {entry.key}</div>
            </div>
          ))}
        </div>
      </div>
    )}
  </div>
);

export const ParameterSection = ({ parameter }) => (
  <div>
    <div className="parameter-section">
      <div className="parameter-header">{parameter ? parameter.name : 'parameter'}:</div>
      {parameter ? (
        <div className="parameter-body">
          {parameter.no_ipv4 && <div className="eight-spaces">no_ipv4: true</div>}
        </div>
      ) : (
        <div> </div>
      )}
    </div>
  </div>
);


export const MaxSection = ({ max }) => (
  <div>
    <div className="max-section">
      <div className="max-header">max:</div>
      {max ? (
        <div className="max-body">
          {max.ue && <div className="twenty-spaces">ue: {max.ue}</div>}
          {max.peer && <div className="twenty-spaces">peer: {max.peer}</div>}
        </div>
      ) : (
        <div> </div>
      )}
    </div>
  </div>
);


export const NSIComponent = ({ nsi }) => {
  return (
    <div>
      {nsi && (
        <div className="nsi-section">
          <div className="twenty-spaces">nsi:</div>
          {nsi.map((item, index) => (
            <div key={index} className="twenty-spaces">
              <div className="twenty-spaces">-addr: {item.addr}</div>
              <div className="twenty-spaces">&nbsp;port: {item.port}</div>
              <div className="twenty-spaces">
                &nbsp;s_nssai:
                <div className="twenty-spaces">sst: {item.s_nssai && item.s_nssai.sst}</div>
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export const FreeDiameterSection = ({ freeDiameter }) => {
  return (
    <div className="twenty-spaces">
      <div>freeDiameter:</div>
      {freeDiameter && (
        <div>
          <div className="twenty-spaces">identity: {freeDiameter.identity}</div>
          <div className="twenty-spaces">realm: {freeDiameter.realm}</div>
          <div className="twenty-spaces">listen_on: {freeDiameter.listen_on}</div>
          <div className="twenty-spaces">no_fwd: {String(freeDiameter.no_fwd)}</div>
          {freeDiameter.load_extension && (
            <div className="twenty-spaces">
              load_extension:
              {freeDiameter.load_extension.map((extension, index) => (
                <div key={index} className="twenty-spaces">
                  -module: {extension.module}
                  {extension.conf && (
                    <div className="twenty-spaces">conf: {extension.conf}</div>
                  )}
                </div>
              ))}
            </div>
          )}
          {freeDiameter.connect && (
            <div className="twenty-spaces">
              connect:
              {freeDiameter.connect.map((connection, index) => (
                <div key={index} className="twenty-spaces">
                  -identity: {connection.identity}
                  <div className="twenty-spaces">&nbsp;addr: {connection.addr}</div>
                </div>
              ))}
            </div>
          )}
        </div>
      )}
    </div>
  );
};

export const PfcpSection = ({ pfcp }) => {
  return (
    <div>
      {pfcp && (
        <div className="pfcp-section">
          <div className="twenty-spaces">pfcp:</div>
          {pfcp.map((item, index) => (
            <div key={index} className="twenty-spaces">
              <div className="twenty-spaces">-addr: {item.addr}</div>          
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export const GtpcSection = ({ gtpc }) => {
  return (
    <div>
      {gtpc && (
        <div className="gtpc-section">
          <div className="twenty-spaces">gtpc:</div>
          {gtpc.map((item, index) => (
            <div key={index} className="twenty-spaces">
              <div className="twenty-spaces">-addr: {item.addr}</div>          
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export const GtpuSection = ({ gtpu }) => {
  return (
    <div>
      {gtpu && (
        <div className="gtpu-section">
          <div className="twenty-spaces">gtpu:</div>
          {gtpu.map((item, index) => (
            <div key={index} className="twenty-spaces">
              <div className="twenty-spaces">-addr: {item.addr}</div>          
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export const SubnetSection = ({ subnet }) => {
  return (
    <div>
      {subnet && (
        <div className="subnet-section">
          <div className="twenty-spaces">subnet:</div>
          {subnet.map((item, index) => (
            <div key={index} className="twenty-spaces">
              <div className="twenty-spaces">-addr: {item.addr}</div>          
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export const DnsSection = ({ dns }) => {
  return (
    <div>
      {dns && (
        <div className="dns-section">
          <div className="twenty-spaces">dns:</div>
          {dns.map((item, index) => (
            <div key={index} className="twenty-spaces">
              <div className="twenty-spaces">-addr: {item.addr}</div>          
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export const MtuSection = ({ mtu }) => {
  return (
    <div>
      {mtu && (
        <div className="mtu-section">
          <div className="twenty-spaces">mtu: {mtu}</div>
        </div>
      )}
    </div>
  );
};

export const CtfSection = ({ ctf }) => {
  return (
    <div>
      {ctf && (
        <div className="ctf-section">
          <div className="twenty-spaces">ctf:</div>
          <div className="forty-spaces">enabled: {ctf.enabled}</div>
        </div>
      )}
    </div>
  );
};

export const FreeDiameterFileSection = ({ freeDiameter }) => {
  return (
    <div>
      {freeDiameter && (
        <div className="freeDiameter-section">
          <div className="twenty-spaces">freeDiameter: {freeDiameter}</div>
        </div>
      )}
    </div>
  );
};

export const InfoSection = ({ info }) => {
  return (
    <div>
      {info && (
        <div className="info-section">
          <div className="twenty-spaces">info:</div>
          {info.map((item, index) => (
            <div key={index} className="twenty-spaces">
              <div className="twenty-spaces">- s_nssai:</div>
              {item.s_nssai && (
                <div className="twenty-spaces">
                  {item.s_nssai.map((s_item, s_index) => (
                    <div key={s_index} className="twenty-spaces">
                      <div className="twenty-spaces">- sst: {s_item.sst}</div>
                      {s_item.dnn && (
                        <div className="twenty-spaces">
                          <div className="twenty-spaces">dnn:</div>
                          {s_item.dnn.map((dnn_item, dnn_index) => (
                            <div key={dnn_index} className="twenty-spaces">
                              <div className="twenty-spaces">- {dnn_item}</div>
                            </div>
                          ))}
                        </div>
                      )}
                    </div>
                  ))}
                </div>
              )}
              {item.tai && (
                <div className="twenty-spaces">
                  <div className="twenty-spaces">tai:</div>
                  {item.tai.map((tai_item, tai_index) => (
                    <div key={tai_index} className="twenty-spaces">
                      <div className="twenty-spaces">- plmn_id:</div>
                      <div className="twenty-spaces">
                        <div className="twenty-spaces">mcc: {tai_item.plmn_id.mcc}</div>
                        <div className="twenty-spaces">mnc: {tai_item.plmn_id.mnc}</div>
                      </div>
                      <div className="twenty-spaces">&nbsp;tac: {tai_item.tac}</div>
                    </div>
                  ))}
                </div>
              )}
            </div>
          ))}
        </div>
      )}
    </div>
  );
};


export const DbUriSection = ({ db_uri }) => {
  return (
    <div>
      {db_uri && (
        <div className="db-uri-section">db_uri: {db_uri}</div>
      )}
    </div>
  );
};
