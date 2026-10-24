//
// ctr_drbg.h
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
#ifndef _circle_mbedtls_ctr_drbg_h
#define _circle_mbedtls_ctr_drbg_h

#include <mbedtls/psa_util.h>
#include <circle-mbedtls/entropycontext.h>
#include <circle/types.h>

namespace CircleMbedTLS {

/// \deprecated Since Mbed TLS 4.0 the Deterministic Random Bit Generator is
///	internal to the PSA crypto subsystem, which is initialized by
///	CCryptoModule. This class is a no-op, which is kept for source
///	compatibility only.
class CCTR_DRBG		/// Deterministic Random Bit Generator
{
public:
	CIRCLE_MBEDTLS_RNG_DEPRECATED
	CCTR_DRBG (CEntropyContext * /* pEntropy */)
	{
	}

	/// \return Context for mbedtls_psa_get_random()
	CIRCLE_MBEDTLS_RNG_DEPRECATED
	void *Get (void)
	{
		return MBEDTLS_PSA_RANDOM_STATE;
	}

	CIRCLE_MBEDTLS_RNG_DEPRECATED
	int Seed (u8 const * /* pPersonalizationData */, size_t /* nLength */)
	{
		return 0;
	}

	CIRCLE_MBEDTLS_RNG_DEPRECATED
	int Seed (char const * /* pPersonalizationString */)
	{
		return 0;
	}
};

}

#endif
