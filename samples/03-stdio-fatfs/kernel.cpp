//
// kernel.cpp
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

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

CKernel::CKernel (void)
:	CStdlibAppStdio ("03-stdio-fatfs")
{
	mActLED.Blink (5);	// show we are alive
}

CStdlibApp::TShutdownMode CKernel::Run (void)
{
	mLogger.Write (GetKernelName (), LogNotice, "C Standard Library stdio Demo");

	mLogger.Write (GetKernelName (), LogNotice, "stdio test...");

	char const stdio_filename[] = "stdio.txt";
	FILE *fp = fopen(stdio_filename, "w");

	if (fp != nullptr)
	{
                auto const fp2 = fp;
		fprintf(fp, "Opened file with (FILE *) %p\n", fp2);
		fclose(fp);

		fp = fopen(stdio_filename, "r");
		if (fp != nullptr)
		{
			char buf[200];
			char *p;

			while ((p = fgets(buf, sizeof(buf), fp)) != nullptr)
			{
			        size_t const len = strlen(buf);
			        if (len > 0 && buf[len - 1] == '\n')
			        {
			                buf[len - 1] = 0;
			        }
				printf ("Read from file: '%s'\n", p);
			}
			fclose(fp);
		}
		else
		{
			mLogger.Write (GetKernelName (), LogPanic, "Cannot open file for reading with fopen()");
		}
	}
	else
	{
		mLogger.Write (GetKernelName (), LogPanic, "Cannot open file for writing with fopen()");
	}

        DIR * const dir = opendir ("/");

        if (dir != nullptr)
        {
                printf ("Listing \"/\" directory with readdir:\n");
                while (true)
                {
                        errno = 0;
                        struct dirent const * const dp = readdir (dir);
                        if (dp != nullptr)
                        {
                                printf ("\t%s\n", dp->d_name);
                        }
                        else
                        {
                                if (errno != 0 && errno != ENOENT)
                                {
                                        fprintf (stderr,
                                                "readdir failed with errno %d\n",
                                                errno);
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
                                fprintf (stderr,
                                        "readdir_r failed with error number %d\n",
                                        error);
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
                        perror ("closedir() failed ");
                }
        }
        else
        {
            perror ("opendir (\"/\") failed");
        }

	mLogger.Write (GetKernelName (), LogNotice, "C Standard Library Demo finished");

	return ShutdownHalt;
}
