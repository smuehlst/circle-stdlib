//
// x509certchain.h
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
#ifndef _circle_mbedtls_x509certchain_h
#define _circle_mbedtls_x509certchain_h

#include <mbedtls/x509_crt.h>
#include <circle/types.h>

namespace CircleMbedTLS {

class CX509CertificateChain	/// Chain of X.509 certificates
{
public:
	CX509CertificateChain (void)
	{
		mbedtls_x509_crt_init (&m_Chain);
	}

	~CX509CertificateChain (void)
	{
		mbedtls_x509_crt_free (&m_Chain);
	}

	mbedtls_x509_crt *Get (void)
	{
		return &m_Chain;
	}

	/// \brief Add certificate in PEM or DER format to chain
	int AddCertificate (const u8 *pBuffer, size_t nLength)
	{
		return  mbedtls_x509_crt_parse (&m_Chain, pBuffer, nLength);
	}

private:
	mbedtls_x509_crt m_Chain;
};

}

#endif
