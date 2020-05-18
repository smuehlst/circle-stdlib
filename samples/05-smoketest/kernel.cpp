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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <dirent.h>

namespace
{
    const char FromKernel[] = "Smoke Test";
}

CKernel::CKernel (void)
    : CStdlibAppStdio ("05-smoketest")
{
    mActLED.Blink (5);  // show we are alive
}

CStdlibApp::TShutdownMode
CKernel::Run (void)
{
    mLogger.Write (GetKernelName (), LogNotice,
                    "Compile time: " __DATE__ " " __TIME__);

    mLogger.Write (GetKernelName (), LogNotice, "A timer will stop the loop");

    bool volatile nTimerFired = false;

    // start timer to elapse after 5 seconds
    mTimer.StartKernelTimer (5 * HZ, TimerHandler,
                              const_cast<bool*> (&nTimerFired));

    // generate a log message every second
    unsigned nTime = mTimer.GetTime ();
    while (!nTimerFired)
    {
        while (nTime == mTimer.GetTime ())
        {
            // just wait a second
        }

        nTime = mTimer.GetTime ();

        mLogger.Write (GetKernelName (), LogNotice, "Time is %u", nTime);
    }

    IoTest ();

    mLogger.Write (GetKernelName (), LogNotice, "Shutting down...");

    return ShutdownHalt;
}

void
CKernel::Report(const char *s)
{
    mLogger.Write (GetKernelName (), LogNotice, "%s\n", s);
}

void
CKernel::PErrorExit(const char *s)
{
    perror (s);
    mLogger.Write (GetKernelName (), LogError, "exiting with code 1...");
    exit (1);
}

void
CKernel::IoTest (void)
{
    char const stdio_filename[] = "stdio.txt";

    Report ("Test fopen");
    FILE *fp = fopen (stdio_filename, "w");

    Report ("Test file operations...");
    if (fp != nullptr)
    {
        fprintf (fp, "Opened file with (FILE *) %p\n", fp);
        fclose (fp);

        fp = fopen (stdio_filename, "r");
        if (fp != nullptr)
        {
            char buf[200];
            char *p;

            while ((p = fgets (buf, sizeof(buf), fp)) != nullptr)
            {
                size_t const len = strlen (buf);
                if (len > 0 && buf[len - 1] == '\n')
                {
                    buf[len - 1] = 0;
                }
                printf ("Read from file: '%s'\n", p);
            }

            // Read the blank after "Opened" by seeking
            int seekResult = fseek(fp, 6L, SEEK_SET);
            if (seekResult != 0)
            {
                PErrorExit ("fseek failed");
            }

            char c;
            if (fread (&c, 1, 1, fp) != 1)
            {
                PErrorExit ("fread failed");
            }

            if (c != ' ')
            {
                fprintf (stderr, "Bad read result, expected ' ', got '%c'\n", c);
                exit (1);
            }

            fclose (fp);
        }
        else
        {
            PErrorExit ("Cannot open file for reading with fopen()");
        }
    }
    else
    {
        PErrorExit ("Cannot open file for writing with fopen()");
    }

    Report ("Test directory operations...");

    DIR * const dir = opendir ("/");
    if (dir != nullptr)
    {
        Report ("Listing \"/\" directory with readdir:\n");
        while (true)
        {
            errno = 0;
            struct dirent const *const dp = readdir (dir);
            if (dp != nullptr)
            {
                printf ("\t%s\n", dp->d_name);
            }
            else
            {
                if (errno != 0 && errno != ENOENT)
                {
                    PErrorExit ("readdir () failed");
                }

                break;
            }
        }

        printf ("Rewinding directory...\n");
        rewinddir (dir);

        printf ("Listing \".\" directory with readdir_r:\n");
        while (true)
        {
            struct dirent de;
            struct dirent *dep;
            int error = readdir_r (dir, &de, &dep);
            if (error != 0)
            {
                PErrorExit ("readdir_r () failed");
            }
            if (dep != nullptr)
            {
                printf ("\t%s\n", dep->d_name);
            }
            else
            {
                break;
            }
        }
        printf ("Closing directory...\n");
        if (closedir (dir) != 0)
        {
            PErrorExit ("closedir() failed ");
        }
    }
    else
    {
        PErrorExit ("opendir (\"/\") failed");
    }
}

void
CKernel::TimerHandler (TKernelTimerHandle, void *pParam, void*)
{
    bool *pTimerFired = static_cast<bool*> (pParam);
    *pTimerFired = true;
}
