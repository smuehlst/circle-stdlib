//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015-2020  R. Stange <rsta2@o2online.de>
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
#include "mqttconfig.h"
#include <circle/net/ntpdaemon.h>

using namespace CircleMbedTLS;

// Network configuration
#define NET_DEVICE_TYPE		NetDeviceTypeEthernet		// or: NetDeviceTypeWLAN

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
	, IPAddress, NetMask, DefaultGateway, DNSServer, NET_DEVICE_TYPE
#else
	, 0, 0, 0, 0, NET_DEVICE_TYPE
#endif
	),
	m_TLSSupport (&mNet)
{
	mActLED.Blink (5);	// show we are alive
}

CStdlibApp::TShutdownMode CKernel::Run (void)
{
	//m_TLSSupport.SetDebugThreshold (1);

#ifdef TLS_RSA_MIN_KEY_SIZE
	m_TLSSupport.GetCertProfile ()->SetRSAMinimumKeySize (TLS_RSA_MIN_KEY_SIZE);
#endif

#ifdef NTP_SERVER
	new CNTPDaemon (NTP_SERVER, &mNet);
	mScheduler.Sleep (4);
#endif

	CMQTTSubscriber *pSubscriber = new CMQTTSubscriber (&m_TLSSupport);
	mScheduler.Sleep (2);

	CMQTTPublisher *pPublisher = new CMQTTPublisher (&m_TLSSupport);

	do
	{
		mScheduler.Sleep (10);
	}
	while (   pSubscriber->IsConnected ()
	       || pPublisher->IsConnected ());

	return ShutdownHalt;
}
