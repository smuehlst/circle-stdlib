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

#include <mbedtls/entropy.h>

namespace CircleMbedTLS {

class CEntropyContext
{
public:
	CEntropyContext (void)
	{
		mbedtls_entropy_init (&m_Context);
	}

	~CEntropyContext (void)
	{
		mbedtls_entropy_free (&m_Context);
	}

	mbedtls_entropy_context *Get (void)
	{
		return &m_Context;
	}

private:
	mbedtls_entropy_context m_Context;
};

}

#endif
