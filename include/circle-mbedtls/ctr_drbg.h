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

#include <mbedtls/ctr_drbg.h>
#include <circle-mbedtls/entropycontext.h>
#include <circle/util.h>
#include <circle/types.h>

namespace CircleMbedTLS {

class CCTR_DRBG		/// Deterministic Random Bit Generator
{
public:
	CCTR_DRBG (CEntropyContext *pEntropy)
	:	m_pEntropy (pEntropy)
	{
		mbedtls_ctr_drbg_init (&m_Context);
	}

	~CCTR_DRBG (void)
	{
		mbedtls_ctr_drbg_free (&m_Context);
	}

	mbedtls_ctr_drbg_context *Get (void)
	{
		return &m_Context;
	}

	int Seed (const u8 *pPersonalizationData, size_t nLength)
	{
		return mbedtls_ctr_drbg_seed (&m_Context, mbedtls_entropy_func, m_pEntropy->Get (),
					      pPersonalizationData, nLength);
	}

	int Seed (const char *pPersonalizationString)
	{
		return Seed (reinterpret_cast<const u8 *> (pPersonalizationString),
			     strlen (pPersonalizationString));
	}

private:
	CEntropyContext *m_pEntropy;

	mbedtls_ctr_drbg_context m_Context;
};

}

#endif
