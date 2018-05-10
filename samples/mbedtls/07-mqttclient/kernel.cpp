//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015-2018  R. Stange <rsta2@o2online.de>
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
#include "mqttpublisher.h"
#include "mqttsubscriber.h"
#include <circle/net/ntpdaemon.h>
#include <mbedtls/debug.h>

using namespace CircleMbedTLS;

// Network configuration
#define USE_DHCP

#ifndef USE_DHCP
static const u8 IPAddress[]      = {192, 168, 0, 250};
static const u8 NetMask[]        = {255, 255, 255, 0};
static const u8 DefaultGateway[] = {192, 168, 0, 1};
static const u8 DNSServer[]      = {192, 168, 0, 1};
#endif

#define NTP_SERVER		"pool.ntp.org"

CKernel::CKernel (void)
        : CStdlibAppNetwork ("mqttclient", CSTDLIBAPP_DEFAULT_PARTITION
#ifndef USE_DHCP
	, IPAddress, NetMask, DefaultGateway, DNSServer
#endif
	)
{
	mActLED.Blink (5);	// show we are alive
}

CStdlibApp::TShutdownMode CKernel::Run (void)
{
	//mbedtls_debug_set_threshold (1);

#ifdef NTP_SERVER
	new CNTPDaemon (NTP_SERVER, &mNet);
	mScheduler.Sleep (4);
#endif

	new CMQTTSubscriber (&mNet);
	mScheduler.Sleep (2);

	new CMQTTPublisher (&mNet);

	for (unsigned nCount = 0; 1; nCount++)
	{
		mScheduler.Yield ();

		mScreen.Rotor (0, nCount);
	}

	return ShutdownHalt;
}
