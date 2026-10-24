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
#include <circle/sched/semaphore.h>
#include <mbedtls/threading.h>
#include <psa/crypto.h>
#include <assert.h>

using namespace CircleMbedTLS;

namespace {

// Condition variable built from a semaphore and a waiter count.
//
// The waiter count is only modified while the associated mutex is held:
// mbedtls_condition_variable_signal() and _broadcast() are called with the
// mutex locked by the library. A signal, which arrives between releasing the
// mutex and blocking on the semaphore in cond_wait(), is buffered in the
// semaphore count, so that it cannot get lost.
struct TConditionVariable
{
	TConditionVariable (void)
	:	Semaphore (0),
		nWaiters (0)
	{
	}

	CSemaphore Semaphore;
	unsigned nWaiters;
};

CMutex *GetMutex (mbedtls_platform_mutex_t const *mutex)
{
	assert (mutex != nullptr);
	return static_cast<CMutex *> (mutex->pMutex);
}

TConditionVariable *GetConditionVariable (mbedtls_platform_condition_variable_t const *cond)
{
	assert (cond != nullptr);
	return static_cast<TConditionVariable *> (cond->pCondVar);
}

}

int CThreadingModule::m_nUseCount = 0;

CThreadingModule::CThreadingModule (void)
{
	if (m_nUseCount++ == 0)
	{
		mbedtls_threading_set_alt (mutex_init, mutex_destroy, mutex_lock, mutex_unlock,
					   cond_init, cond_destroy, cond_signal, cond_broadcast,
					   cond_wait);
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

int CThreadingModule::mutex_init (mbedtls_platform_mutex_t *mutex)
{
	assert (mutex != nullptr);

	mutex->pMutex = new CMutex;
	if (mutex->pMutex == nullptr)
	{
		return PSA_ERROR_INSUFFICIENT_MEMORY;
	}

	return 0;
}

void CThreadingModule::mutex_destroy (mbedtls_platform_mutex_t *mutex)
{
	CMutex *const pMutex = GetMutex (mutex);
	assert (pMutex != nullptr);

	delete pMutex;
	mutex->pMutex = nullptr;
}

int CThreadingModule::mutex_lock (mbedtls_platform_mutex_t *mutex)
{
	CMutex *const pMutex = GetMutex (mutex);
	if (pMutex == nullptr)
	{
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	pMutex->Acquire ();

	return 0;
}

int CThreadingModule::mutex_unlock (mbedtls_platform_mutex_t *mutex)
{
	CMutex *const pMutex = GetMutex (mutex);
	if (pMutex == nullptr)
	{
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	pMutex->Release ();

	return 0;
}

int CThreadingModule::cond_init (mbedtls_platform_condition_variable_t *cond)
{
	assert (cond != nullptr);

	cond->pCondVar = new TConditionVariable;
	if (cond->pCondVar == nullptr)
	{
		return PSA_ERROR_INSUFFICIENT_MEMORY;
	}

	return 0;
}

void CThreadingModule::cond_destroy (mbedtls_platform_condition_variable_t *cond)
{
	TConditionVariable *const pCondVar = GetConditionVariable (cond);
	assert (pCondVar != nullptr);
	assert (pCondVar->nWaiters == 0);

	delete pCondVar;
	cond->pCondVar = nullptr;
}

int CThreadingModule::cond_signal (mbedtls_platform_condition_variable_t *cond)
{
	TConditionVariable *const pCondVar = GetConditionVariable (cond);
	if (pCondVar == nullptr)
	{
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	if (pCondVar->nWaiters > 0)
	{
		pCondVar->nWaiters--;
		pCondVar->Semaphore.Up ();
	}

	return 0;
}

int CThreadingModule::cond_broadcast (mbedtls_platform_condition_variable_t *cond)
{
	TConditionVariable *const pCondVar = GetConditionVariable (cond);
	if (pCondVar == nullptr)
	{
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	for (; pCondVar->nWaiters > 0; pCondVar->nWaiters--)
	{
		pCondVar->Semaphore.Up ();
	}

	return 0;
}

int CThreadingModule::cond_wait (mbedtls_platform_condition_variable_t *cond,
				 mbedtls_platform_mutex_t *mutex)
{
	TConditionVariable *const pCondVar = GetConditionVariable (cond);
	CMutex *const pMutex = GetMutex (mutex);
	if (   pCondVar == nullptr
	    || pMutex == nullptr)
	{
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	pCondVar->nWaiters++;
	pMutex->Release ();

	pCondVar->Semaphore.Down ();	// the waiter count has been decremented by the signaler

	pMutex->Acquire ();

	return 0;
}
