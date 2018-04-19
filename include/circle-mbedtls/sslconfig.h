//
// sslconfig.h
//
// Copyright (C) 2018  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _circle_mbedtls_sslconfig_h
#define _circle_mbedtls_sslconfig_h

#include <mbedtls/ssl.h>
#include <circle-mbedtls/x509certchain.h>
#include <circle-mbedtls/ctr_drbg.h>

namespace CircleMbedTLS {

typedef void TSSLDebugCallback (void *pContext, int nDebugLevel,
				const char *pFilename, int nLine, const char *pMessage);

class CSSLConfig	/// SSL/TLS configuration
{
public:
	CSSLConfig (void)
	{
		mbedtls_ssl_config_init (&m_Config);
	}

	~CSSLConfig (void)
	{
		mbedtls_ssl_config_free (&m_Config);
	}

	const mbedtls_ssl_config *Get (void) const
	{
		return &m_Config;
	}

	int SetDefaults (int nEndpoint, int nTransport, int nPreset)
	{
		return mbedtls_ssl_config_defaults (&m_Config, nEndpoint, nTransport, nPreset);
	}

	void SetAuthMode (int nAuthMode)
	{
		mbedtls_ssl_conf_authmode (&m_Config, nAuthMode);
	}

	void SetCA_Chain (CX509CertificateChain *pCA_Chain)
	{
		mbedtls_ssl_conf_ca_chain (&m_Config, pCA_Chain->Get (), 0);	// TODO: add CRLs
	}

	void SetRNG (CCTR_DRBG *pCTR_DRBG)
	{
		mbedtls_ssl_conf_rng (&m_Config, mbedtls_ctr_drbg_random, pCTR_DRBG->Get ());
	}

	void SetDebugCallback (TSSLDebugCallback *pCallback, void  *pParam)
	{
		mbedtls_ssl_conf_dbg (&m_Config, pCallback, pParam);
	}


private:
	mbedtls_ssl_config m_Config;
};

}

#endif
