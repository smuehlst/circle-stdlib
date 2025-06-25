//
// threadingmodule.cpp
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
#include <circle-mbedtls/threadingmodule.h>
#include <circle/sched/mutex.h>
#include <mbedtls/threading.h>
#include <assert.h>

using namespace CircleMbedTLS;

int CThreadingModule::m_nUseCount = 0;

CThreadingModule::CThreadingModule (void)
{
	if (m_nUseCount++ == 0)
	{
		mbedtls_threading_set_alt (mutex_init, mutex_free, mutex_lock, mutex_unlock);
	}
}

CThreadingModule::~CThreadingModule (void)
{
	assert (m_nUseCount > 0);
	if (--m_nUseCount == 0)
	{
		mbedtls_threading_free_alt ();
	}
}

void CThreadingModule::mutex_init (mbedtls_threading_mutex_t *mutex)
{
	CMutex **ppMutex = reinterpret_cast<CMutex **> (mutex);
	assert (ppMutex != 0);

	*ppMutex = new CMutex;
	assert (*ppMutex != 0);
}

void CThreadingModule::mutex_free (mbedtls_threading_mutex_t *mutex)
{
	CMutex **ppMutex = reinterpret_cast<CMutex **> (mutex);
	assert (ppMutex != 0);
	assert (*ppMutex != 0);

	delete *ppMutex;
	*ppMutex = 0;
}

int CThreadingModule::mutex_lock (mbedtls_threading_mutex_t *mutex)
{
	CMutex **ppMutex = reinterpret_cast<CMutex **> (mutex);
	if (   ppMutex == 0
	    || *ppMutex == 0)
	{
		return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
	}

	(*ppMutex)->Acquire ();

	return 0;
}

int CThreadingModule::mutex_unlock (mbedtls_threading_mutex_t *mutex)
{
	CMutex **ppMutex = reinterpret_cast<CMutex **> (mutex);
	if (   ppMutex == 0
	    || *ppMutex == 0)
	{
		return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
	}

	(*ppMutex)->Release ();

	return 0;
}
