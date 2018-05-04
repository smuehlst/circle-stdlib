//
// tlssimplesupport.h
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
#ifndef _circle_mbedtls_tlssimplesupport_h
#define _circle_mbedtls_tlssimplesupport_h

#include <circle/net/netsubsystem.h>
#include <circle-mbedtls/entropymodule.h>
#include <circle-mbedtls/x509certprofile.h>
#include <mbedtls/debug.h>

namespace CircleMbedTLS {

class CTLSSimpleSupport		/// Environment for the simple SSL/TLS support
{
public:
	/// \param pNetSubSystem Pointer to the network subsystem, TLS support is running on
	CTLSSimpleSupport (CNetSubSystem *pNetSubSystem)
	:	m_pNetSubSystem (pNetSubSystem)
	{
	}

	/// \return Pointer to the network subsystem, TLS support is running on
	CNetSubSystem *GetNet (void)
	{
		return m_pNetSubSystem;
	}

	/// \return Pointer to the security profile used in this environment
	CX509CertificateProfile *GetCertProfile (void)
	{
		return &m_CertProfile;
	}

	/// \param nThreshold Threshold for debug messages (0..5)
	void SetDebugThreshold (int nThreshold)
	{
		mbedtls_debug_set_threshold (nThreshold);
	}

public:
	CNetSubSystem *m_pNetSubSystem;

	CEntropyModule		m_EntropyModule;
	CX509CertificateProfile	m_CertProfile;
};

}

#endif
