//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2020  R. Stange <rsta2@o2online.de>
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
#include "kernel.h"

namespace {
	const char FromKernel[] = "kernel";
}

CKernel::CKernel (void)
:	//m_LogFile ("circle.log"),	// uncomment this to write the log to a regular file
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer)
{
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Logger.Initialize (&m_LogFile);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	m_Logger.Write (FromKernel, LogNotice, "A timer will stop the loop");

	bool volatile nTimerFired = false;

	// start timer to elapse after 15 seconds
	m_Timer.StartKernelTimer (15 * HZ, TimerHandler, const_cast<bool *>(&nTimerFired));

	// generate a log message every second
	unsigned nTime = m_Timer.GetTime ();
	while (!nTimerFired)
	{
		while (nTime == m_Timer.GetTime ())
		{
			// just wait a second
		}

		nTime = m_Timer.GetTime ();

		m_Logger.Write (FromKernel, LogNotice, "Time is %u", nTime);
	}

	m_Logger.Write (FromKernel, LogNotice, "Shutting down...");

	return ShutdownHalt;
}

void CKernel::TimerHandler (TKernelTimerHandle, void *pParam, void *)
{
	bool *pTimerFired = static_cast<bool *>(pParam);
	*pTimerFired = true;
}
