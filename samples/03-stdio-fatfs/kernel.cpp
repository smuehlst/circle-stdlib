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
		fprintf(fp, "Opened file with (FILE *) %p\n", fp);
		fclose(fp);

		fp = fopen(stdio_filename, "r");
		if (fp != nullptr)
		{
			char buf[200];
			char *p;

			while ((p = fgets(buf, sizeof(buf), fp)) != nullptr)
			{
				printf ("Read from file: '%s'", p);
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

	mLogger.Write (GetKernelName (), LogNotice, "C Standard Library Demo finished");

	return ShutdownHalt;
}
