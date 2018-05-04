//
// mqttsubscriber.cpp
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
#include "mqttsubscriber.h"
#include "mqttconfig.h"
#include <circle/logger.h>
#include <circle/timer.h>
#include <circle/util.h>

static const char FromSubscriber[] = "mqttsub";

using namespace CircleMbedTLS;

CMQTTSubscriber::CMQTTSubscriber (CTLSSimpleSupport *pTLSSupport)
:	CMQTTClient (pTLSSupport, MAX_PACKET_SIZE, MAX_PACKETS_QUEUED, MAX_TOPIC_SIZE),
	m_bTimerRunning (FALSE)
{
	Connect (TRUE, MQTT_BROKER_HOSTNAME, MQTT_TLS_PORT,
		 SUB_CLIENT_ID, SUB_USER_NAME, SUB_PASSWORD);
}

void CMQTTSubscriber::OnConnect (boolean bSessionPresent)
{
	CLogger::Get ()->Write (FromSubscriber, LogDebug, "OnConnect (present %u)",
				bSessionPresent ? 1 : 0);

	m_nConnectTime = CTimer::Get ()->GetUptime ();
	m_bTimerRunning = TRUE;

	Subscribe (SUB_TOPIC);
}

void CMQTTSubscriber::OnDisconnect (TMQTTDisconnectReason Reason)
{
	m_bTimerRunning = FALSE;

	CLogger::Get ()->Write (FromSubscriber, LogDebug, "OnDisconnect (reason %u)",
				(unsigned) Reason);
}

void CMQTTSubscriber::OnMessage (const char *pTopic, const u8 *pPayload,
				   size_t nPayloadLength, boolean bRetain)
{
	// pPayload is not 0-terminated, make a C-string out of it
	char PayloadString[256] = "";
	if (nPayloadLength > 0)
	{
		if (nPayloadLength > sizeof PayloadString-1)
		{
			nPayloadLength = sizeof PayloadString-1;
		}

		memcpy (PayloadString, pPayload, nPayloadLength);
		PayloadString[nPayloadLength] = '\0';
	}

	CLogger::Get ()->Write (FromSubscriber, LogNotice,
				"OnMessage (\"%s\", \"%s\")", pTopic, PayloadString);
}

void CMQTTSubscriber::OnLoop (void)
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
	}
}
