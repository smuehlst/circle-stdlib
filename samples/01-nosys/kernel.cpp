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
#include <errno.h>

CKernel::CKernel (void)
        : CStdlibAppScreen("01-nosys")
{
	mActLED.Blink (5);	// show we are alive
}

void CKernel::DoLongJmp (void)
{
	mLogger.Write (GetKernelName (), LogNotice, "Doing longmp...");
	longjmp (m_JumpBuf, 42);
}

CStdlibApp::TShutdownMode CKernel::Run (void)
{
	mLogger.Write (GetKernelName (), LogNotice, "C Standard Library Demo");

	mLogger.Write (GetKernelName (), LogNotice, "Call printf () (expect result -1)");
	int const printfResult = printf ("Hello world\n");
	mLogger.Write (GetKernelName (), LogNotice, "printf result %d", printfResult);

	mLogger.Write (GetKernelName (), LogNotice, "Call sin (1)");
	double const f = sin (1);
	mLogger.Write (GetKernelName (), LogNotice, "sin (1) %f", f);

	mLogger.Write (GetKernelName (), LogNotice, "Call acos (22), expect NaN with errno EDOM=%d", EDOM);
	double const g = acos (22);
	assert (isnan(g));
	mLogger.Write (GetKernelName (), LogNotice, "acos (22): isnan () is true, errno=%d", errno);

	// Search element in sorted array
	int const nSortedArray[] = { -10, -1, 0, 1, 2, 3, 4, 5, 6 };

	auto const nCmpIntFn =
		[](void const *a, void const *b)
		{
			return *static_cast<int const *>(a) - *static_cast<int const *>(b);
		};
	int const nKey = 1;
	auto const nElement =
		static_cast<int *>(
			bsearch (&nKey, nSortedArray,
					sizeof(nSortedArray) / sizeof(nSortedArray[0]), sizeof(nSortedArray[0]),
					nCmpIntFn));

	if (nElement)
	{
		mLogger.Write (GetKernelName (), LogNotice, "Found Element %d at distance %u", nKey,
				static_cast<unsigned int>(nElement - nSortedArray));
	}
	else
	{
		mLogger.Write (GetKernelName (), LogNotice, "Element %d not found", nKey);
	}

	auto const nRetVal = setjmp (m_JumpBuf);
	if (nRetVal)
	{
		mLogger.Write (GetKernelName (), LogNotice, "Value %d returned from subroutine by setjmp", nRetVal);
	}
	else
	{
		DoLongJmp ();
	}

	// Sort unsorted array with qsort
	int nArray[] = { 77, 33, 1, -20, 1000 };
	constexpr size_t nArraySize = sizeof(nArray) / sizeof(nArray[0]);
	qsort(nArray, nArraySize, sizeof (nArray[0]), nCmpIntFn);
	mLogger.Write (GetKernelName (), LogNotice, "Array sorted with qsort():");
	for (auto const &i: nArray)
	{
		mLogger.Write (GetKernelName (), LogNotice, "\t%d", i);
	}

	mLogger.Write (GetKernelName (), LogNotice, "C Standard Library Demo finished");

	return ShutdownHalt;
}
