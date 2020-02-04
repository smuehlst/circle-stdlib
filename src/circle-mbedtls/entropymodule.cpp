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
#include <mbedtls/entropy.h>
#include <mbedtls/entropy_poll.h>
#include <circle/timer.h>
#include <assert.h>

ASSERT_STATIC (RAND_MAX == 0x7FFFFFFF);

using namespace CircleMbedTLS;

CEntropyModule *CEntropyModule::s_pThis = 0;

CEntropyModule::CEntropyModule (void)
#ifdef USE_QEMU_FOR_TEST
:	m_bInitialized (FALSE)
#endif
{
	s_pThis = this;
}

int CEntropyModule::EntropyPoll (void *pData, unsigned char *pOutput,
				 size_t nLength, size_t *pResultLength)
{
#ifdef USE_QEMU_FOR_TEST
	if (!s_pThis->m_bInitialized)
	{
		srand ((int) CTimer::Get ()->GetClockTicks ());

		s_pThis->m_bInitialized = TRUE;
	}
#endif

	assert (pData == 0);	// not used here

	assert (nLength >= 4);	// TODO: allow smaller lengths
	if (nLength < 4)
	{
		return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
	}

	size_t nResultLength = 0;
	for (; nLength >= 4; nLength -= 4, nResultLength += 4, pOutput += 4)
	{
#ifndef USE_QEMU_FOR_TEST
		*(u32 *) pOutput = s_pThis->m_RNG.GetNumber ();
#else
		*(u32 *) pOutput = (u32) rand ();
#endif
	}

	*pResultLength = nResultLength;

	return 0;
}

// implements the MBEDTLS_ENTROPY_HARDWARE_ALT config option
int mbedtls_hardware_poll (void *pData, unsigned char *pOutput,
			   size_t nLength, size_t *pResultLength)
{
	return CEntropyModule::EntropyPoll (pData, pOutput, nLength, pResultLength);
}
