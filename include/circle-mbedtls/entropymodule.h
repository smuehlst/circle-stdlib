//
// entropymodule.h
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
#ifndef _circle_mbedtls_entropymodule_h
#define _circle_mbedtls_entropymodule_h

// use rand() instead of hardware RNG, because the RNG may not work with QEMU
//#define USE_QEMU_FOR_TEST		// not for production!

#include <stdlib.h>
#include <circle/bcmrandom.h>

namespace CircleMbedTLS {

class CEntropyModule
{
public:
	CEntropyModule (void);

	static int EntropyPoll (void *pData, unsigned char *pOutput,
				size_t nLength, size_t *pResultLength);

private:
#ifndef USE_QEMU_FOR_TEST
	CBcmRandomNumberGenerator m_RNG;
#else
	boolean m_bInitialized;
#endif

	static CEntropyModule *s_pThis;
};

}

#endif
