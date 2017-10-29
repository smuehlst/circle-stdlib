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

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <exception>

namespace {

char const FromKernel[] = "kernel";

void cxx_test(void);

}

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Serial (&m_Interrupt, TRUE),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel ()),
	m_DWHCI (&m_Interrupt, &m_Timer),
	m_EMMC (&m_Interrupt, &m_Timer, &m_ActLED),
	m_Console (&m_Serial)
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
		bOK = m_Interrupt.Initialize ();
	}

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
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_EMMC.Initialize ();
	}

	if (bOK)
	{
		bOK = m_DWHCI.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Console.Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	m_Logger.Write (FromKernel, LogNotice, "C++ Standard Library Demo");

	char const partition[] = "emmc1-1";

	// Mount file system
	CDevice * const pPartition = m_DeviceNameService.GetDevice(partition, TRUE);
	if (pPartition == 0) {
		m_Logger.Write(FromKernel, LogPanic, "Partition not found: %s",
				partition);
	}

	if (!m_FileSystem.Mount(pPartition)) {
		m_Logger.Write(FromKernel, LogPanic, "Cannot mount partition: %s",
				partition);
	}

	// Initialize newlib stdio with a reference to Circle's file system and to the console
	CGlueStdioInit(m_FileSystem, m_Console);

	cxx_test();

	m_Logger.Write(FromKernel, LogNotice, "C++ Standard Library Test finished");

	m_FileSystem.UnMount();

	return ShutdownHalt;
}

namespace {

struct ooops: std::exception {
	const char* what() const noexcept {
		return "Ooops!";
	}
};

void barf(void) {
	std::cerr << "Throwing exception..." << std::endl;
	throw ooops();
}

void cxx_test(void) {
	std::vector<std::string> const v = { "vector entry 1", "vector entry 2" };

	std::cout << "Opening file via std::ofstream..." << std::endl;
	std::ofstream ofs("test.txt", std::ofstream::out);
	try {
		barf();
	} catch (std::exception& e) {
		std::cerr << "Caught exception..." << std::endl;
		ofs << "lorem ipsum" << std::endl;
		std::string s(e.what());
		ofs << s.c_str() << std::endl;
	}
	std::cout << "Use <algorithm>..." << std::endl;
	for_each(v.begin(), v.end(),
			[&](std::string const &s) { ofs << s.c_str() << std::endl; });
	ofs.close();
}

}
