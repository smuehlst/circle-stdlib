//
// adaptedsocket.cpp
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
#include <circle-mbedtls/adaptedsocket.h>
#include <circle/net/dnsclient.h>
#include <circle/net/ipaddress.h>
#include <circle/net/in.h>
#include <circle/sched/scheduler.h>
#include <circle/timer.h>
#include <circle/util.h>
#include <mbedtls/net_sockets.h>
#include <assert.h>

using namespace CircleMbedTLS;

CPtrArray CAdaptedSocket::s_SocketArray;

CAdaptedSocket::CAdaptedSocket (CNetSubSystem *pNetSubSystem, int nProtocol)
:	m_pNetSubSystem (pNetSubSystem),
	m_pSocket (new CSocket (pNetSubSystem, nProtocol)),
	m_nHandle (AddSocket (this)),
	m_nRxBytesValid (0),
	m_nRxBytesUsed (0),
	m_bBlock (TRUE)
{
	assert (nProtocol == IPPROTO_TCP);	// TODO: support IPPROTO_UDP
	assert (m_nHandle >= 0);
}

// create from accepted connection
CAdaptedSocket::CAdaptedSocket (CNetSubSystem *pNetSubSystem, CSocket *pSocket)
:	m_pNetSubSystem (pNetSubSystem),
	m_pSocket (pSocket),
	m_nHandle (AddSocket (this)),
	m_nRxBytesValid (0),
	m_nRxBytesUsed (0),
	m_bBlock (TRUE)
{
	assert (m_pSocket != 0);
	assert (m_nHandle >= 0);
}

CAdaptedSocket::~CAdaptedSocket (void)
{
	DeleteSocket (m_nHandle);
	m_nHandle = -1;

	delete m_pSocket;
	m_pSocket = 0;
}

int CAdaptedSocket::Connect (CIPAddress &rForeignIP, u16 nForeignPort)
{
	assert (m_pSocket != 0);
	if (m_pSocket->Connect (rForeignIP, nForeignPort) < 0)
	{
		return MBEDTLS_ERR_NET_CONNECT_FAILED;
	}

	return 0;
}

int CAdaptedSocket::Connect (const char *pHost, const char *pPort)
{
	assert (pHost != 0);
	assert (pPort != 0);

	char *pEnd = 0;
	unsigned long ulPort = strtoul (pPort, &pEnd, 10);
	if (   (pEnd != 0 && *pEnd != '\0')
	    || (ulPort == 0 || ulPort > 0xFFFF))
	{
		return MBEDTLS_ERR_NET_CONNECT_FAILED;
	}

	assert (m_pNetSubSystem != 0);
	CIPAddress IPAddress;
	CDNSClient DNSClient (m_pNetSubSystem);
	if (!DNSClient.Resolve (pHost, &IPAddress))
	{
		return MBEDTLS_ERR_NET_UNKNOWN_HOST;
	}

	return Connect (IPAddress, static_cast<u16> (ulPort));
}

int CAdaptedSocket::Bind (const char *pPort)
{
	assert (pPort != 0);

	char *pEnd = 0;
	unsigned long ulPort = strtoul (pPort, &pEnd, 10);
	if (   (pEnd != 0 && *pEnd != '\0')
	    || (ulPort == 0 || ulPort > 0xFFFF))
	{
		return MBEDTLS_ERR_NET_CONNECT_FAILED;
	}

	assert (m_pSocket != 0);
	if (m_pSocket->Bind (static_cast<u16> (ulPort)) < 0)
	{
		return MBEDTLS_ERR_NET_BIND_FAILED;
	}

	if (m_pSocket->Listen (MBEDTLS_NET_LISTEN_BACKLOG) < 0)
	{
		return MBEDTLS_ERR_NET_BIND_FAILED;
	}

	return 0;
}

int CAdaptedSocket::Accept (void *pForeignIP)
{
	CIPAddress IPAddress;
	u16 nPort;

	assert (m_pSocket != 0);
	CSocket *pConnection = m_pSocket->Accept (&IPAddress, &nPort);
	if (pConnection == 0)
	{
		return MBEDTLS_ERR_NET_ACCEPT_FAILED;
	}

	assert (m_pNetSubSystem != 0);
	CAdaptedSocket *pNewSocket = new CAdaptedSocket (m_pNetSubSystem, pConnection);
	if (pNewSocket == 0)
	{
		delete pConnection;

		return MBEDTLS_ERR_SSL_ALLOC_FAILED;
	}

	if (pForeignIP != 0)
	{
		IPAddress.CopyTo (static_cast<u8 *> (pForeignIP));
	}

	int nHandle = AddSocket (pNewSocket);
	assert (nHandle >= 0);

	return nHandle;
}

int CAdaptedSocket::Receive (void *pBuffer, unsigned nLength)
{
	assert (pBuffer != 0);

	// CSocket::Receive() always needs a buffer of FRAME_BUFFER_SIZE, but our
	// Receive() may be called with smaller buffers, we manage this here.
	unsigned nBytesAvail = m_nRxBytesValid-m_nRxBytesUsed;
	if (nBytesAvail == 0)
	{
		assert (m_pSocket != 0);
		int nResult = m_pSocket->Receive (m_RxBuffer, sizeof m_RxBuffer,
						  m_bBlock ? 0 : MSG_DONTWAIT);
		if (nResult == 0)		// no data available
		{
			CScheduler::Get ()->Yield ();

			return MBEDTLS_ERR_SSL_WANT_READ;
		}
		if (nResult < 0)		// reset from peer
		{
			return 0;		// EOF
		}

		m_nRxBytesValid = nResult;
		m_nRxBytesUsed = 0;
	}

	nBytesAvail = m_nRxBytesValid-m_nRxBytesUsed;
	if (nLength > nBytesAvail)
	{
		nLength = nBytesAvail;
	}

	memcpy (pBuffer, &m_RxBuffer[m_nRxBytesUsed], nLength);
	m_nRxBytesUsed += nLength;

	return nLength;
}

int CAdaptedSocket::ReceiveTimeout (void *pBuffer, unsigned nLength, unsigned nTimeoutMs)
{
	assert (pBuffer != 0);

	// CSocket::Receive() always needs a buffer of FRAME_BUFFER_SIZE, but our
	// Receive() may be called with smaller buffers, we manage this here.
	unsigned nBytesAvail = m_nRxBytesValid-m_nRxBytesUsed;
	if (nBytesAvail == 0)
	{
		unsigned nTimeoutHZ = MSEC2HZ (nTimeoutMs);
		unsigned nStartTicks = CTimer::Get ()->GetTicks ();

		int nResult = 0;
		while (nResult == 0)		// as long there is no data available
		{
			assert (m_pSocket != 0);
			nResult = m_pSocket->Receive (m_RxBuffer, sizeof m_RxBuffer,
						      nTimeoutMs == 0 ? 0 : MSG_DONTWAIT);
			if (nResult == 0)
			{
				CScheduler::Get ()->Yield ();

				if (CTimer::Get ()->GetTicks ()-nStartTicks >= nTimeoutHZ)
				{
					return MBEDTLS_ERR_SSL_TIMEOUT;
				}
			}
			if (nResult < 0)	// reset from peer
			{
				return 0;	// EOF
			}
		}

		m_nRxBytesValid = nResult;
		m_nRxBytesUsed = 0;
	}

	nBytesAvail = m_nRxBytesValid-m_nRxBytesUsed;
	if (nLength > nBytesAvail)
	{
		nLength = nBytesAvail;
	}

	memcpy (pBuffer, &m_RxBuffer[m_nRxBytesUsed], nLength);
	m_nRxBytesUsed += nLength;

	return nLength;
}

int CAdaptedSocket::Send (const void *pBuffer, unsigned nLength)
{
	assert (m_pSocket != 0);
	int nResult = m_pSocket->Send (pBuffer, nLength, MSG_DONTWAIT);
	if (nResult < 0)
	{
		return MBEDTLS_ERR_NET_SEND_FAILED;
	}

	return nResult;
}

void CAdaptedSocket::SetOptionBlocking (boolean bBlock)
{
	m_bBlock = bBlock;
}

const u8 *CAdaptedSocket::GetForeignIP (void) const
{
	if (m_pSocket == 0)
	{
		return 0;
	}

	return m_pSocket->GetForeignIP ();
}

int CAdaptedSocket::GetHandle (void) const
{
	assert (m_nHandle >= 0);
	return m_nHandle;
}

CAdaptedSocket *CAdaptedSocket::Get (int nHandle)
{
	if (nHandle < 0)
	{
		return 0;
	}

	return reinterpret_cast<CAdaptedSocket *> (s_SocketArray[nHandle]);
}

int CAdaptedSocket::AddSocket (CAdaptedSocket *pSocket)
{
	assert (pSocket != 0);

	for (unsigned i = 0; i < s_SocketArray.GetCount (); i++)	// search for free slot
	{
		if (s_SocketArray[i] == 0)
		{
			s_SocketArray[i] = pSocket;

			return i;
		}
	}

	return s_SocketArray.Append (pSocket);		// not available: new slot
}

void CAdaptedSocket::DeleteSocket (int nHandle)
{
	assert (nHandle >= 0);
	s_SocketArray[nHandle] = 0;
}
