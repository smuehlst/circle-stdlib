//
// tlssimpleclientsocket.h
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
#ifndef _circle_mbedtls_tlssimpleclientsocket_h
#define _circle_mbedtls_tlssimpleclientsocket_h

#include <circle-mbedtls/tlssocket.h>
#include <circle-mbedtls/entropycontext.h>
#include <circle-mbedtls/ctr_drbg.h>
#include <circle-mbedtls/tlsconfig.h>
#include <circle-mbedtls/x509certprofile.h>
#include <circle-mbedtls/x509certchain.h>
#include <circle/net/netsubsystem.h>
#include <circle/types.h>

namespace CircleMbedTLS {

class CTLSSimpleClientSocket : public CTLSSocket	/// Easy to use socket for SSL/TLS clients
{
public:
	CTLSSimpleClientSocket (CNetSubSystem *pNetSubSystem, int nProtocol);
	~CTLSSimpleClientSocket (void);

	/// \brief Add certificate in PEM or DER format
	/// \note Has to be called before Setup()
	/// \note Can be called multiple times
	int AddCertificate (const u8 *pBuffer, size_t nLength);

	/// \brief Add certificate from file in PEM or DER format
	/// \note Has to be called before Setup()
	/// \note Can be called multiple times
	int AddCertificateFile (const char *pFilename);

	/// \brief Add certificate(s) from path in PEM or DER format
	/// \return Number of added certificates (< 0 for error)
	/// \note Has to be called before Setup()
	/// \note Scans for files with the extension ".pem", ".crt" or ".der"
	int AddCertificatePath (const char *pPath);

	/// \brief Setup SSL/TLS configuration prior to Connect()
	/// \param pServername The hostname of the server to be connected
	/// \param pPersonalizationString Optional string for seeding the random bit generator
	/// \param nRSAMinimumKeySize Minimum size of verified RSA keys in bits
	int Setup (const char *pServername, const char *pPersonalizationString = 0,
		   unsigned nRSAMinimumKeySize = 1024);

private:
	static void DebugCallback (void *pContext, int nLevel,
				   const char *pFile, int nLine, const char *pMsg);

private:
	CEntropyContext		m_Entropy;
	CCTR_DRBG		m_CTR_DRBG;
	CX509CertificateProfile	m_CertProfile;
	CX509CertificateChain	m_CertChain;
	CTLSConfig		m_TLSConfig;

	boolean m_bCertAdded;
};

}

#endif
