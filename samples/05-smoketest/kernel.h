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
#include <qemu/qemuhostfile.h>
#include <string>

class CKernel : public CStdlibAppStdio
{
public:
    CKernel(void);

    TShutdownMode Run(void);

private:
    void IoTest(void);
    void CxxTest(void);
    void PErrorExit [[noreturn]] (const char *s);
    void Report(const std::string &s);
    void Report(const char *s, ...);

    static void
    TimerHandler(TKernelTimerHandle hTimer, void *pParam, void *pContext);

    CQEMUHostFile m_LogFile;

    struct ooops : std::exception
    {
        const char *what() const noexcept;
    };

    void barf(void);

    struct a
    {
        a(CKernel &k);
        a(const a &other);
        ~a();
        static size_t counter;
        CKernel &kernel;
    };

    CScheduler m_Scheduler;
    CNetSubSystem m_Net;
};

#endif
