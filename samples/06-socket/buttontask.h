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
#ifndef _buttontask_h
#define _buttontask_h

#include <circle/sched/task.h>
#include <circle/sched/synchronizationevent.h>
#include <circle/gpiopin.h>
#include <circle/gpiomanager.h>
#include <circle/timer.h>

class CButtonTask : public CTask
{
public:
	typedef void TPressedHandler (unsigned nGPIOPin, void *pParam);

public:
	CButtonTask (unsigned nGPIOPin, CGPIOManager *pGPIOManager,
		     TPressedHandler *pHandler, void *pParam);

	void Run (void);

	void Terminate (void);

private:
	static void InterruptHandler (void *pParam);

	// Button debounce handler
	static void TimerHandler (TKernelTimerHandle hTimer, void *pParam, void *pContext);

private:
	unsigned		m_nGPIOPin;
	TPressedHandler		*m_pHandler;
	void			*m_pParam;
	CGPIOPin		m_GPIOPin;
	CSynchronizationEvent	m_Event;
	bool			m_bTerminate;
	TKernelTimerHandle	m_hTimer;
};

#endif
