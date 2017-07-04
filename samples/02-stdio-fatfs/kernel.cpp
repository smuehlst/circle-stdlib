//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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

#include <circle_glue.h>

namespace
{
	char const FromKernel[] = "kernel";
}

CKernel::CKernel (void)
:	m_Memory (TRUE),
	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel ()),
	m_EMMC (&m_Interrupt, &m_Timer, &m_ActLED)
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
	
	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_EMMC.Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	m_Logger.Write (FromKernel, LogNotice, "");
	m_Logger.Write (FromKernel, LogNotice, "C Standard Library stdio Demo");
	m_Logger.Write (FromKernel, LogNotice, "");

	char const partition[] = "emmc1-1";

	// Mount file system
	CDevice * const pPartition = m_DeviceNameService.GetDevice (partition, TRUE);
	if (pPartition == 0)
	{
		m_Logger.Write (FromKernel, LogPanic, "Partition not found: %s", partition);
	}

	if (!m_FileSystem.Mount (pPartition))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot mount partition: %s", partition);
	}

	m_Logger.Write (FromKernel, LogNotice, "stdio test...");

	// Initialize newlib stdio with a reference to Circle's file system
	CGlueStdioInit(m_FileSystem);

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
				m_Logger.Write (FromKernel, LogNotice, "Read from file: '%s'", p);
			}
			fclose(fp);
		}
		else
		{
			m_Logger.Write (FromKernel, LogPanic, "Cannot open file for reading with fopen()");
		}
	}
	else
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot open file for writing with fopen()");
	}

	m_FileSystem.UnMount ();

	return ShutdownHalt;
}
