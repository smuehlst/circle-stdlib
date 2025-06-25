//
// threadingmodule.h
//
// Copyright (C) 2025  R. Stange <rsta2@gmx.net>
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
#ifndef _circle_mbedtls_threadingmodule_h
#define _circle_mbedtls_threadingmodule_h

#include <threading_alt.h>

namespace CircleMbedTLS {

class CThreadingModule		// Support MBEDTLS_THREADING_C with MBEDTLS_THREADING_ALT
{
public:
	CThreadingModule (void);
	~CThreadingModule (void);

private:
	static void mutex_init (mbedtls_threading_mutex_t *mutex);
	static void mutex_free (mbedtls_threading_mutex_t *mutex);

	static int mutex_lock (mbedtls_threading_mutex_t *mutex);
	static int mutex_unlock (mbedtls_threading_mutex_t *mutex);

private:
	static int m_nUseCount;
};

}

#endif
