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
#include <circle/string.h>
#include <circle/debug.h>
#include <assert.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

namespace
{
	char const FromKernel[] = "kernel";
}

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Logger (m_Options.GetLogLevel ())
{
	m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}
	
	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}
	
	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}
	
	return bOK;
}

void CKernel::DoLongJmp (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Doing longmp...");
	longjmp(m_JumpBuf, 42);
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	m_Logger.Write (FromKernel, LogNotice, "C Standard Library Demo");

	m_Logger.Write (FromKernel, LogNotice, "Call printf() (expect -1)");
	int const printfResult = printf ("Hello world\n");
	m_Logger.Write (FromKernel, LogNotice, "printf result %d", printfResult);

	m_Logger.Write (FromKernel, LogNotice, "Call sin()");
	double const f = sin (1);
	m_Logger.Write (FromKernel, LogNotice, "sin(1) %f", f);

	m_Logger.Write (FromKernel, LogNotice, "Call acos()");
	double const g = acos (22);
	m_Logger.Write (FromKernel, LogNotice, "acos(22) %f", f);

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
		m_Logger.Write (FromKernel, LogNotice, "Found Element %d at distance %u", nKey,
				static_cast<unsigned int>(nElement - nSortedArray));
	}
	else
	{
		m_Logger.Write (FromKernel, LogNotice, "Element %d not found", nKey);
	}

	auto const nRetVal = setjmp (m_JumpBuf);
	if (nRetVal)
	{
		m_Logger.Write (FromKernel, LogNotice, "Value %d returned from subroutine by setjmp", nRetVal);
	}
	else
	{
		DoLongJmp ();
	}

	// Sort unsorted array with qsort
	int nArray[] = { 77, 33, 1, -20, 1000 };
	constexpr size_t nArraySize = sizeof(nArray) / sizeof(nArray[0]);
	qsort(nArray, nArraySize, sizeof (nArray[0]), nCmpIntFn);
	m_Logger.Write (FromKernel, LogNotice, "Array sorted with qsort():");
	for (auto const &i: nArray)
	{
		m_Logger.Write (FromKernel, LogNotice, "\t%d", i);
	}

	m_Logger.Write (FromKernel, LogNotice, "C Standard Library Demo finished");

	return ShutdownHalt;
}
