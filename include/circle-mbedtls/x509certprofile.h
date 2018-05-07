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
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/ecp.h>
#include <circle/types.h>
#include <string.h>

namespace CircleMbedTLS {

class CX509CertificateProfile	/// Security profile for certificate verification
{
public:
	CX509CertificateProfile (void)
	{
		// init from default profile
		Set (&mbedtls_x509_crt_profile_default);
	}

	const mbedtls_x509_crt_profile *Get (void) const
	{
		return &m_Profile;
	}

	void Set (const mbedtls_x509_crt_profile *pProfile)
	{
		memcpy (&m_Profile, pProfile, sizeof m_Profile);
	}

	// Message digests (see: mbedtls/md.h)

	void SetAllowedMessageDigests (u32 nFlags)
	{
		m_Profile.allowed_mds = nFlags;
	}

	u32 GetAllowedMessageDigests (void) const
	{
		return m_Profile.allowed_mds;
	}

	void EnableMessageDigest (mbedtls_md_type_t Type)
	{
		m_Profile.allowed_mds |= MBEDTLS_X509_ID_FLAG (Type);
	}

	void DisableMessageDigest (mbedtls_md_type_t Type)
	{
		m_Profile.allowed_mds &= ~MBEDTLS_X509_ID_FLAG (Type);
	}

	boolean IsMessageDigestAllowed (mbedtls_md_type_t Type) const
	{
		return m_Profile.allowed_mds & MBEDTLS_X509_ID_FLAG (Type) ? TRUE : FALSE;
	}

	// Public key algorithms (see: mbedtls/pk.h)

	void SetAllowedPublicKeys (u32 nFlags)
	{
		m_Profile.allowed_pks = nFlags;
	}

	u32 GetAllowedPublicKeys (void) const
	{
		return m_Profile.allowed_pks;
	}

	void EnablePublicKey (mbedtls_pk_type_t Type)
	{
		m_Profile.allowed_pks |= MBEDTLS_X509_ID_FLAG (Type);
	}

	void DisablePublicKey (mbedtls_pk_type_t Type)
	{
		m_Profile.allowed_pks &= ~MBEDTLS_X509_ID_FLAG (Type);
	}

	boolean IsPublicKeyAllowed (mbedtls_pk_type_t Type) const
	{
		return m_Profile.allowed_pks & MBEDTLS_X509_ID_FLAG (Type) ? TRUE : FALSE;
	}

	// Elliptic Curves (see: mbedtls/ecp.h)

	void SetAllowedEllipticCurves (u32 nFlags)
	{
		m_Profile.allowed_curves = nFlags;
	}

	u32 GetAllowedEllipticCurves (void) const
	{
		return m_Profile.allowed_curves;
	}

	void EnableEllipticCurve (mbedtls_ecp_group_id Type)
	{
		m_Profile.allowed_curves |= MBEDTLS_X509_ID_FLAG (Type);
	}

	void DisableEllipticCurve (mbedtls_ecp_group_id Type)
	{
		m_Profile.allowed_curves &= ~MBEDTLS_X509_ID_FLAG (Type);
	}

	boolean IsEllipticCurveAllowed (mbedtls_ecp_group_id Type) const
	{
		return m_Profile.allowed_curves & MBEDTLS_X509_ID_FLAG (Type) ? TRUE : FALSE;
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
