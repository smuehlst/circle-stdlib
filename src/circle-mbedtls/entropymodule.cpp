//
// entropymodule.cpp
//
// Copyright (C) 2018-2020  R. Stange <rsta2@o2online.de>
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
#include <circle-mbedtls/entropymodule.h>
#include <mbedtls/platform.h>
#include <psa/crypto.h>
#include <circle/timer.h>
#include <circle/util.h>
#include <assert.h>

ASSERT_STATIC (RAND_MAX == 0x7FFFFFFF);

using namespace CircleMbedTLS;

CEntropyModule *CEntropyModule::s_pThis = nullptr;

CEntropyModule::CEntropyModule (void)
#ifdef USE_QEMU_FOR_TEST
:	m_bInitialized (FALSE)
#endif
{
	s_pThis = this;
}

int CEntropyModule::GetEntropy (unsigned char *pOutput, size_t nLength)
{
	assert (s_pThis != nullptr);
	assert (pOutput != nullptr);

#ifdef USE_QEMU_FOR_TEST
	if (!s_pThis->m_bInitialized)
	{
		srand (static_cast<int> (CTimer::Get ()->GetClockTicks ()));

		s_pThis->m_bInitialized = TRUE;
	}
#endif

	while (nLength > 0)
	{
#ifndef USE_QEMU_FOR_TEST
		u32 const nRandom = s_pThis->m_RNG.GetNumber ();
#else
		u32 const nRandom = static_cast<u32> (rand ());
#endif

		size_t const nChunk = nLength < sizeof nRandom ? nLength : sizeof nRandom;
		memcpy (pOutput, &nRandom, nChunk);

		pOutput += nChunk;
		nLength -= nChunk;
	}

	return 0;
}

// implements the MBEDTLS_PSA_DRIVER_GET_ENTROPY config option
int mbedtls_platform_get_entropy (psa_driver_get_entropy_flags_t flags, size_t *estimate_bits,
				  unsigned char *output, size_t output_size)
{
	if (flags != 0)
	{
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (CEntropyModule::GetEntropy (output, output_size) != 0)
	{
		return PSA_ERROR_INSUFFICIENT_ENTROPY;
	}

	assert (estimate_bits != nullptr);
	*estimate_bits = 8 * output_size;

	return 0;
}
