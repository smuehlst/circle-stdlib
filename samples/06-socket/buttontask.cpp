//
// buttontask.h
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
#include "buttontask.h"
#include <circle/logger.h>
#include <circle/sched/scheduler.h>
#include <assert.h>

LOGMODULE ("buttontask");

CButtonTask::CButtonTask (unsigned nGPIOPin, CGPIOManager *pGPIOManager,
			  TPressedHandler *pHandler, void *pParam)
:	m_nGPIOPin (nGPIOPin),
	m_pHandler (pHandler),
	m_pParam (pParam),
	m_GPIOPin (nGPIOPin, GPIOModeInputPullUp, pGPIOManager),
	m_bTerminate (false),
	m_hTimer (0)
{
}

void CButtonTask::Run (void)
{
	m_GPIOPin.ConnectInterrupt (InterruptHandler, this);
	m_GPIOPin.EnableInterrupt (GPIOInterruptOnFallingEdge);

	LOGDBG ("Button task started (pin %u)", m_nGPIOPin);

	while (!m_bTerminate)
	{
		m_Event.Clear ();
		m_Event.Wait ();

		if (!m_bTerminate)
		{
			LOGDBG ("Call pressed handler (pin %u)", m_nGPIOPin);

			assert (m_pHandler);
			(*m_pHandler) (m_nGPIOPin, m_pParam);
		}
	}

	m_GPIOPin.DisableInterrupt ();
	m_GPIOPin.DisconnectInterrupt ();

	LOGDBG ("Terminate button task (pin %u)", m_nGPIOPin);
}

void CButtonTask::Terminate (void)
{
	m_bTerminate = true;

	m_Event.Set ();

	CScheduler::Get ()->Yield ();
}

void CButtonTask::InterruptHandler (void *pParam)
{
	CButtonTask *pThis = static_cast<CButtonTask *> (pParam);
	assert (pThis);

	if (pThis->m_hTimer)
	{
		CTimer::Get ()->CancelKernelTimer (pThis->m_hTimer);

		pThis->m_hTimer = 0;
	}

	pThis->m_hTimer = CTimer::Get ()->StartKernelTimer (MSEC2HZ (50), TimerHandler, pThis);
}

void CButtonTask::TimerHandler (TKernelTimerHandle hTimer, void *pParam, void *pContext)
{
	CButtonTask *pThis = static_cast<CButtonTask *> (pParam);
	assert (pThis);

	pThis->m_hTimer = 0;

	if (pThis->m_GPIOPin.Read () == LOW)
	{
		pThis->m_Event.Set ();
	}
}
