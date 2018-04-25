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
#include <math.h>
#include <stdlib.h>
#include <time.h>

CKernel::CKernel (void)
:	CStdlibAppStdio ("02-stdio-hello")
{
	mActLED.Blink (5);	// show we are alive
}

CStdlibApp::TShutdownMode CKernel::Run (void)
{
	mLogger.Write (GetKernelName (), LogNotice, "C Standard Library stdin/stdout/stderr Demo");

	mLogger.Write (GetKernelName (), LogNotice, "stdio test...");

	printf("Hello world!\n");
	fprintf(stderr, "Hello world on stderr!\n");

	printf("Type some characters and hit <RETURN>\n");

	char line[200];
	if (fgets(line, sizeof(line), stdin) != nullptr)
	{
		printf("Read '%s' from stdin...\n", line);
	}
	else
	{
		perror("fgets returned NULL");
	}

	char outstr[200];
        time_t const t = time (nullptr);
        struct tm * const tmp = localtime (&t);

        if (tmp == nullptr)
        {
                perror("localtime");
        }
        else if (strftime(outstr, sizeof(outstr), "%D %T", tmp) == 0)
        {
                fprintf(stderr, "strftime returned 0");
        }
        else
        {
                // Do not expect a valid current time as the Raspberry Pi
                // clock has not been set.
                printf("The current date/time is \"%s\"\n", outstr);
        }

	mLogger.Write (GetKernelName (), LogNotice, "C Standard Library Demo finished");

	return ShutdownHalt;
}
