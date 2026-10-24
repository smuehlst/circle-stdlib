//
// entropycontext.h
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
#ifndef _circle_mbedtls_entropycontext_h
#define _circle_mbedtls_entropycontext_h

namespace CircleMbedTLS {

#define CIRCLE_MBEDTLS_RNG_DEPRECATED \
	[[deprecated ("Since Mbed TLS 4.0 the PSA random generator is used implicitly")]]

/// \deprecated Since Mbed TLS 4.0 the entropy context is internal to the PSA
///	crypto subsystem, which is initialized by CCryptoModule. This class is a
///	no-op, which is kept for source compatibility only.
class CEntropyContext
{
public:
	CIRCLE_MBEDTLS_RNG_DEPRECATED
	CEntropyContext (void)
	{
	}

	CIRCLE_MBEDTLS_RNG_DEPRECATED
	void *Get (void)
	{
		return nullptr;
	}
};

}

#endif
