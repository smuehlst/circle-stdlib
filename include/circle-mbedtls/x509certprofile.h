//
// x509certprofile.h
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
#ifndef _circle_mbedtls_x509certprofile_h
#define _circle_mbedtls_x509certprofile_h

#include <mbedtls/x509_crt.h>
#include <string.h>

namespace CircleMbedTLS {

class CX509CertificateProfile	/// Security profile for certificate verification
{
public:
	CX509CertificateProfile (void)
	{
		// init from default profile
		memcpy (&m_Profile, &mbedtls_x509_crt_profile_default, sizeof m_Profile);
	}

	const mbedtls_x509_crt_profile *Get (void) const
	{
		return &m_Profile;
	}

	/// \brief Set specific minimum size for RSA keys
	void SetRSAMinimumKeySize (unsigned nBitLen)
	{
		m_Profile.rsa_min_bitlen = nBitLen;
	}

private:
	mbedtls_x509_crt_profile m_Profile;
};

}

#endif
