//
// kernel.h
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
#ifndef _kernel_h
#define _kernel_h

#include <circle_stdlib_app.h>
#include <string>
#include <circle/cputhrottle.h>
#include <circle/gpiomanager.h>
#include "buttontask.h"

#ifdef CIRCLE_QEMU
#include <qemu/qemuhostfile.h>
#endif

class CKernel : public CStdlibAppStdio
{
public:
    CKernel(void);

    TShutdownMode Run(void);
    CKernelOptions const& GetOptions() const { return mOptions; }
    CCPUThrottle const& GetCPUThrottle() const { return m_CPUThrottle; }

private:
#ifdef CIRCLE_QEMU
    CQEMUHostFile m_LogFile;
#endif
    CCPUThrottle m_CPUThrottle;
    CScheduler m_Scheduler;
    CNetSubSystem m_Net;
    CGPIOManager m_GPIOManager;

    static void ButtonPressedHandler(unsigned nGPIOPin, void *pParam);
};

#endif
