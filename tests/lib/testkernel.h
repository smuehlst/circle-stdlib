#ifndef _testkernel_h
#define _testkernel_h

#include <circle_stdlib_app.h>
#include <qemu/qemuhostfile.h>
#include "doctest/doctest.h"
#include "doctest_logger.h"
#include <circle/net/netsubsystem.h>
#include <circle/sched/scheduler.h>
#include <circle/startup.h>

class CTestKernel : public CStdlibAppStdio
{
public:
    CTestKernel(const char *name)
        : CStdlibAppStdio(name)
    {
    }

    virtual bool Initialize(void)
    {
        if (!CStdlibAppStdio::Initialize() || !mLogger.Initialize(&m_LogFile) || !m_Net.Initialize())
        {
            return false;
        }

        CGlueNetworkInit(m_Net);

        return true;
    }

    TShutdownMode Run(void)
    {
        doctest::Context context;

        mLogger.Write(GetKernelName(), LogNotice,
                      "Compile time: " __DATE__ " " __TIME__);

        DoctestLogger::set_circle_logger(&mLogger);

        int const res = context.run();

        mLogger.Write(GetKernelName(), LogNotice, "Shutting down with exit status %d...", res);

        return static_cast<CStdlibApp::TShutdownMode>(res);
    }

    static int RunTests(const char *testname)
    {
        CTestKernel Kernel(testname);
        if (!Kernel.Initialize())
        {
            halt();
            return EXIT_HALT;
        }

        CStdlibApp::TShutdownMode const ShutdownMode = Kernel.Run();

        // This program is intended for QEMU only, and the exit
        // status is provided from Kernel.Run() via ShutdownMode.
        // The CTestKernel destructor must no be executed, so exit here
        // instead of returning to main.
        if (ShutdownMode != CStdlibApp::ShutdownNone)
        {
            exit(1);
        }

        halt();

        return static_cast<int>(ShutdownMode);
    }

private:
    CQEMUHostFile m_LogFile;

    CScheduler m_Scheduler;
    CNetSubSystem m_Net;
};

#endif
