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
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <exception>
#include <memory>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

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
    mLogger.Initialize (&m_LogFile);

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
    CxxTest ();

    mLogger.Write (GetKernelName (), LogNotice, "Shutting down...");

    return ShutdownHalt;
}

void
CKernel::Report(const char *s)
{
    mLogger.Write (GetKernelName (), LogNotice, "%s", s);
}

void
CKernel::Report(const std::string &s)
{
    Report(s.c_str());
}

void
CKernel::PErrorExit(const char *s)
{
    perror (s);
    mLogger.Write (GetKernelName (), LogError, "error '%s', exiting with code 1...", s);
    exit (1);
}

void
CKernel::IoTest (void)
{
    Report ("Test file operations...");

    string const stdio_filename = "stdio.txt";

    FILE *fp = fopen (stdio_filename.c_str (), "w");

    if (fp == nullptr)
    {
        PErrorExit ("Cannot open file for writing with fopen ()");
    }

    fprintf (fp, "Opened file with (FILE *) %p\n", fp);
    fclose (fp);

    fp = fopen (stdio_filename.c_str (), "r");
    if (fp == nullptr)
    {
         PErrorExit ("Cannot open file for reading with fopen()");
    }

    Report ("fopen () test succeeded");

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
    if (fseek (fp, 6L, SEEK_SET) != 0)
    {
        PErrorExit ("fseek () failed");
    }

    Report ("fseek () test succeeded");

    char c;
    if (fread (&c, 1, 1, fp) != 1)
    {
        PErrorExit ("fread failed");
    }

    Report ("fread () test succeeded");

    if (c != ' ')
    {
        fprintf (stderr, "Bad read result, expected ' ', got '%c'\n", c);
        exit (1);
    }

    Report ("fseek () test succeeded");

    if (fclose (fp) != 0)
    {
        PErrorExit ("fclose () failed");
    }

    Report ("fclose () test succeeded");

    // Test for issue #14
    fp = fopen ("this file does not exist", "r");
    if (fp != nullptr)
    {
        Report ("fopen () for non-existent file unexpectedly succeeded");
        exit (1);
    }
    mLogger.Write (GetKernelName (), LogNotice,
        "fopen () for non-existent file failed as expected with errno %d", errno);

    Report ("Test directory operations...");

    string const dirname = "subdir";

    if (mkdir (("/" + dirname).c_str(), 0) != 0)
    {
        PErrorExit ("mkdir () failed");
    }

    Report ("mkdir () test succeeded");

    DIR * const dir = opendir ("/");
    if (dir == nullptr)
    {
        PErrorExit ("opendir (\"/\") failed");
    }

    Report ("opendir () test succeeded");

    bool found = false;
    while (true)
    {
        errno = 0;
        struct dirent const *const dp = readdir (dir);
        if (dp != nullptr)
        {
            printf ("\t%s\n", dp->d_name);
            found = found || dp->d_name == dirname;
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

    if (!found)
    {
        PErrorExit (("Did not find directory '" + dirname + "'").c_str ());
    }

    Report ("readdir () test succeeded");

    printf ("Rewinding directory...\n");
    rewinddir (dir);

    Report ("rewinddir () test succeeded");

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

    Report ("closedir () test succeeded");

    string const subdir_filename = "/" + dirname + "/file.txt";

    fp = fopen (subdir_filename.c_str (), "w");
    if (fp == nullptr)
    {
        PErrorExit ("fopen () failed");
    }

    if (fprintf (fp, "bla bla") < 0)
    {
        PErrorExit ("fprintf () failed");
    }

    if (fclose (fp) != 0)
    {
        PErrorExit ("fclose () failed");
    }

    string const filename2 = "/file2.txt";

    if (rename (subdir_filename.c_str (), filename2.c_str ()) < 0)
    {
        PErrorExit ("rename () failed");
    }

    Report ("rename () test succeeded");

    if (unlink (filename2.c_str ()) < 0)
    {
        PErrorExit ("unlink () failed");
    }

    Report ("unlink () test succeeded");

    // Test fix for issue #22
    // https://github.com/smuehlst/circle-stdlib/issues/22
    #define SIZE_A 10

    uint32_t referenceArray[SIZE_A];
	auto const testArray = (uint32_t *) malloc(sizeof(uint32_t) * (SIZE_A + 1));
	for (int i = 0; i < SIZE_A; i++) {
		testArray[i] = i;
	}
    memcpy(referenceArray, testArray, sizeof(referenceArray));
	memmove(&testArray[1], &testArray[0], sizeof(uint32_t) * SIZE_A);

    if (memcmp(referenceArray, testArray + 1, sizeof(referenceArray)) != 0)
    {
        PErrorExit ("Fix for issue #22 is broken");
    }
    free(testArray);

    Report ("Fix for issue #22 works as expected");
}

void
CKernel::TimerHandler (TKernelTimerHandle, void *pParam, void*)
{
    bool *pTimerFired = static_cast<bool*> (pParam);
    *pTimerFired = true;
}

const char * CKernel::ooops::what() const noexcept
{
    return "Ooops!";
}

void CKernel::barf(void)
{
    Report("Throwing exception...");
    throw ooops();
}

CKernel::a::a(CKernel &k) : kernel(k)
{
    counter += 1;
}

CKernel::a::a(const a &other) : kernel(other.kernel)
{
    counter += 1;
}

CKernel::a::~a()
{
    assert(counter > 0);
    counter -= 1;
    if (counter == 0)
    {
        kernel.Report("all 'struct a' instances cleaned up...");
    }
}

size_t CKernel::a::counter = 0;

void CKernel::CxxTest(void)
{
    std::vector<std::string> const v = {"vector entry 1", "vector entry 2"};

    Report("Opening file via std::ofstream...");

    std::ofstream ofs("test.txt", std::ofstream::out);
    if (!ofs.is_open())
    {
        PErrorExit("Failed to open file 'test.txt'!");
    }

    try
    {
        barf();
    }
    catch (std::exception &e)
    {
        Report("Caught exception...");
        ofs << "lorem ipsum" << std::endl
            << e.what() << std::endl;

        if (ofs.fail())
        {
            PErrorExit("Writing to file failed");
        }
    }

    Report("Use <algorithm>...");
    for_each(v.begin(), v.end(),
                [&](std::string const &s)
                { ofs << s.c_str() << std::endl; });

    // Test out-of-memory condition
    try
    {
        std::vector<std::unique_ptr<std::vector<a>>> ptrs;
        while (true)
        {
            // provoke out-of-memory error, destructors of "a" and of the vector must be called
            Report("Allocating large array of 'a' instances");
            ptrs.emplace_back(new std::vector<a>(10000000U, a(*this)));
        }
    }
    catch (std::bad_alloc &ba)
    {
        Report(std::string("bad_alloc caught: ") + ba.what());
    }

    ofs.close();
    if (ofs.fail())
    {
        PErrorExit("Closing of file failed");
    }

    Report("File written via std::ofstream");

    Report("C++ smoke tests successful");
}