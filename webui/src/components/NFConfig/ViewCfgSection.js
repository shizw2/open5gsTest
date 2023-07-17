export const NRFSection = ({ nfconfig }) => (
  <div>
    {nfconfig && nfconfig.nrf && (
      <div className="nrf-section">
        <div className="nrf-header">nrf:</div>                      
        {nfconfig.nrf.sbi && (
          <div className="sbi-section">
            <div className="twenty-spaces">sbi:</div>                                           
            {nfconfig.nrf.sbi.addr && nfconfig.nrf.sbi.addr.map((item, index) => (
              <div className="forty-spaces" key={index}>
                -addr: {item}
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