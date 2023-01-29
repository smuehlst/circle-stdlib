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

    // start timer to elapse after 3 seconds
    mTimer.StartKernelTimer (3 * HZ, TimerHandler,
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
    errno = 0;
    fp = fopen ("this file does not exist", "r");
    if (fp != nullptr)
    {
        Report ("fopen () for non-existent file unexpectedly succeeded");
        exit (1);
    }

    if (errno != ENOENT)
    {
        mLogger.Write (GetKernelName (), LogError,
        "fopen () for non-existent file fsets wrong errno %d", errno);
        exit (1);
    }
    
    mLogger.Write (GetKernelName (), LogNotice,
        "fopen () for non-existent file failed as expected with errno %d", errno);

    string const truncate_filename = "truncated.txt";

    fp = fopen (truncate_filename.c_str (), "w");

    if (fp == nullptr)
    {
        PErrorExit ("Cannot open file for writing with fopen ()");
    }

    for (unsigned int i = 0; i < 10000; i += 1)
    {
        if (fputc (1, fp) == EOF)
        {
            PErrorExit ("fputc () failed");
        }
    }

    Report ("fputc () test succeeded");

    int const fildes = fileno (fp);
    if (fsync (fildes) != 0)
    {
        PErrorExit ("fsync () failed");
    }

    Report ("fsync () test succeeded");

    errno = 0;
    rewind (fp);
    if (errno != 0)
    {
        PErrorExit ("rewind () failed");
    }

    Report ("rewind () test succeeded");

    if (fildes == -1)
    {
        PErrorExit ("fileno () failed");
    }

    Report ("fileno () test succeeded");

    off_t const desired_file_size = 1111;
    if (ftruncate (fildes, desired_file_size) == -1)
    {
        PErrorExit ("ftruncate () failed");
    }

    Report ("ftruncate () test succeeded");

    if (!(isatty(0) == 1
            && isatty(1) == 1
            && isatty(2) == 1
            && isatty(fildes) == 0))
    {
        PErrorExit ("isatty () failed");
    }

    Report ("isatty () test succeeded");

    struct stat statbuf;

    if (fstat (fildes, &statbuf) != 0)
    {
        PErrorExit ("fstat () failed");
    }

    if (statbuf.st_size != desired_file_size)
    {
        mLogger.Write (GetKernelName (), LogError,
            "fstat () reports wrong file size (expected %d, got %d), exiting with code 1...",
            static_cast<int>(desired_file_size), static_cast<int>(statbuf.st_size));
        exit (1);
    }

    Report ("fstat () test succeeded");

    fclose (fp);

    memset(&statbuf, 0, sizeof(statbuf));

    if (stat (truncate_filename.c_str(), &statbuf) != 0)
    {
        PErrorExit ("stat () failed");
    }

    if (statbuf.st_size != desired_file_size)
    {
        mLogger.Write (GetKernelName (), LogError,
            "stat () reports wrong file size (expected %d, got %d), exiting with code 1...",
            static_cast<int>(desired_file_size), static_cast<int>(statbuf.st_size));
        exit (1);
    }

    Report ("stat () test succeeded");

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

    {
        int non_existing_unlink_result = unlink ("no such directory/no such file");

        if (non_existing_unlink_result != -1 || errno != ENOTDIR)
        {
            mLogger.Write (GetKernelName (), LogError,
                "unlink () of non-existing file with path: unexpected result "
                "(result == %d, errno == %d)", non_existing_unlink_result, errno);
            exit (1);
        }

        non_existing_unlink_result = unlink ("no such file");

        if (non_existing_unlink_result != -1 || errno != ENOENT)
        {
            mLogger.Write (GetKernelName (), LogError,
                "unlink () of non-existing file in current directory: unexpected result "
                "(result == %d, errno == %d)", non_existing_unlink_result, errno);
            exit (1);
        }
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

    Report ("Testing chdir()");

    {
        errno = 0;
        int const non_existing_dir_result = chdir ("no such directory");

        if (non_existing_dir_result != -1 || errno != ENOENT)
        {
            mLogger.Write (GetKernelName (), LogError,
                "chdir() to non-existing directory: unexpected result "
                "(result == %d, errno == %d)", non_existing_dir_result, errno);
            exit (1);
        }
    }

    {
        string const filename3 {"file3.txt"};
        if (chdir (dirname.c_str()) != 0)
        {
            PErrorExit ("chdir() failed unexpectedly");
        }

        auto const subdir_fp = fopen (filename3.c_str (), "w");
        if (subdir_fp == nullptr)
        {
            PErrorExit ("fopen() after chdir() failed unexpectedly");
        }

        if (fprintf (subdir_fp, "File created after chdir\n") < 0)
        {
            PErrorExit ("fprintf() failed unexpectedly");
        }

        if (fclose (subdir_fp) != 0)
        {
            PErrorExit ("fclose() failed unexpectedly");
        }

        if (chdir ("..") != 0)
        {
            PErrorExit ("chdir(\"..\") failed unexpectedly");
        }

        string const relative_path = dirname + "/" + filename3;
        if (stat (relative_path.c_str(), &statbuf) != 0)
        {
            PErrorExit ("stat () with relative path for file in subdirectory failed");
        }

        string const absolute_path = "/" + relative_path;
        if (stat (absolute_path.c_str(), &statbuf) != 0)
        {
            PErrorExit ("stat () with absolute path for file in subdirectory failed");
        }

        if (chdir ("/") != 0)
        {
            PErrorExit ("chdir(\"/\") failed unexpectedly");
        }

        if (stat (relative_path.c_str(), &statbuf) != 0)
        {
            PErrorExit ("stat () with relative path for file in subdirectory failed");
        }
    }

    Report ("chdir() test successful");

    Report ("Unimplemented functions");

    {
        errno = 0;

        int const fork_result = fork();

        if (fork_result != -1 || errno != ENOSYS)
        {
            mLogger.Write (GetKernelName (), LogError,
                "fork(): unexpected result "
                "(result == %d, errno == %d)", fork_result, errno);
            exit (1);
        }

        Report ("fork() is not implemented, fails as expected");
    }
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
    Report("Provoke out-of-memory condition...");
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