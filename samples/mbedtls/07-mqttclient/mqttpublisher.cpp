//
// mqttpublisher.cpp
//
// Copyright (C) 2018  R. Stange <rsta2@o2online.de>
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
#include "mqttpublisher.h"
#include "mqttconfig.h"
#include <circle/bcmpropertytags.h>
#include <circle/logger.h>
#include <circle/string.h>
#include <circle/timer.h>

static const char FromPublisher[] = "mqttpub";

using namespace CircleMbedTLS;

CMQTTPublisher::CMQTTPublisher (CNetSubSystem *pNetSubSystem)
:	CMQTTClient (pNetSubSystem, MAX_PACKET_SIZE, MAX_PACKETS_QUEUED, MAX_TOPIC_SIZE),
	m_bTimerRunning (FALSE)
{
	Connect (TRUE, MQTT_BROKER_HOSTNAME, MQTT_TLS_PORT,
		 PUB_CLIENT_ID, PUB_USER_NAME, PUB_PASSWORD);
}

void CMQTTPublisher::OnConnect (boolean bSessionPresent)
{
	CLogger::Get ()->Write (FromPublisher, LogDebug, "OnConnect (present %u)",
				bSessionPresent ? 1 : 0);

	m_nConnectTime = CTimer::Get ()->GetUptime ();
	m_nLastPublishTime = m_nConnectTime;
	m_bTimerRunning = TRUE;
}

void CMQTTPublisher::OnDisconnect (TMQTTDisconnectReason Reason)
{
	m_bTimerRunning = FALSE;

	CLogger::Get ()->Write (FromPublisher, LogDebug, "OnDisconnect (reason %u)",
				(unsigned) Reason);
}

void CMQTTPublisher::OnLoop (void)
{
	if (   !IsConnected ()
	    || !m_bTimerRunning)
	{
		return;
	}

	unsigned nRunTime = CTimer::Get ()->GetUptime () - m_nConnectTime;

	// disconnect after 120 seconds
	if (nRunTime >= 120)
	{
		Disconnect ();

		return;
	}

	// publish the CPU temperature every 30 seconds, starting 5 seconds after connect
	if (   nRunTime % 30 == 5
	    && nRunTime != m_nLastPublishTime)
	{
		m_nLastPublishTime = nRunTime;

		CString Payload;
		Payload.Format ("{ \"temp\": \"%.1f\" }", GetTemperature ());

		Publish (PUB_TOPIC, (const u8 *) (const char *) Payload, Payload.GetLength ());
	}
}

float CMQTTPublisher::GetTemperature (void)
{
	CBcmPropertyTags Tags;
	TPropertyTagTemperature TagTemperature;
	TagTemperature.nTemperatureId = TEMPERATURE_ID;
	if (!Tags.GetTag (PROPTAG_GET_TEMPERATURE, &TagTemperature, sizeof TagTemperature, 4))
	{
		return 0.0;
	}

	return (float) TagTemperature.nValue / 1000.0;
}
