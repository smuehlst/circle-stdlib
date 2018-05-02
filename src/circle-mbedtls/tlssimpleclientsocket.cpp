//
// tlssimpleclientsocket.cpp
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
#include <circle-mbedtls/tlssimpleclientsocket.h>
#include <assert.h>
#include <stdio.h>

using namespace CircleMbedTLS;

CTLSSimpleClientSocket::CTLSSimpleClientSocket (CNetSubSystem *pNetSubSystem, int nProtocol)
:	CTLSSocket (pNetSubSystem, nProtocol),
	m_CTR_DRBG (&m_Entropy),
	m_bCertAdded (FALSE)
{
}

CTLSSimpleClientSocket::~CTLSSimpleClientSocket (void)
{
}

int CTLSSimpleClientSocket::AddCertificate (const u8 *pBuffer, size_t nLength)
{
	int nResult = m_CertChain.AddCertificate (pBuffer, nLength);
	if (nResult != 0)
	{
		return nResult;
	}

	m_bCertAdded = TRUE;

	return 0;
}

int CTLSSimpleClientSocket::AddCertificateFile (const char *pFilename)
{
	int nResult = m_CertChain.AddCertificateFile (pFilename);
	if (nResult != 0)
	{
		return nResult;
	}

	m_bCertAdded = TRUE;

	return 0;
}

int CTLSSimpleClientSocket::AddCertificatePath (const char *pPath)
{
	int nResult = m_CertChain.AddCertificatePath (pPath);
	if (nResult <= 0)
	{
		return nResult;
	}

	m_bCertAdded = TRUE;

	return nResult;
}

int CTLSSimpleClientSocket::Setup (const char *pServername, const char *pPersonalizationString,
				   unsigned nRSAMinimumKeySize)
{
	int nResult = m_CTR_DRBG.Seed (pPersonalizationString);
	if (nResult != 0)
	{
		return nResult;
	}

	nResult = m_TLSConfig.SetDefaults (MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
					   MBEDTLS_SSL_PRESET_DEFAULT);
	if (nResult != 0)
	{
		return nResult;
	}

	if (m_bCertAdded)
	{
		m_TLSConfig.SetAuthMode (MBEDTLS_SSL_VERIFY_REQUIRED);

		m_CertProfile.SetRSAMinimumKeySize (nRSAMinimumKeySize);
		m_TLSConfig.SetCertProfile (&m_CertProfile);

		m_TLSConfig.SetCA_Chain (&m_CertChain);
	}
	else
	{
		m_TLSConfig.SetAuthMode (MBEDTLS_SSL_VERIFY_NONE);
	}

	m_TLSConfig.SetRNG (&m_CTR_DRBG);
	m_TLSConfig.SetDebugCallback (DebugCallback, stdout);

        nResult = CTLSSocket::Setup (m_TLSConfig);
	if (nResult != 0)
	{
		return nResult;
	}

	assert (pServername != 0);
	return SetHostname (pServername);
}

void CTLSSimpleClientSocket::DebugCallback (void *pContext, int nLevel,
					    const char *pFilename, int nLine, const char *pMsg)
{
	FILE *pFile = reinterpret_cast<FILE *> (pContext);

	fprintf (pFile, "%s(%d): %s", pFilename, nLine, pMsg);
	fflush (pFile);
}
