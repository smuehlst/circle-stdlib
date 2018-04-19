//
// sslsocket.h
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
#ifndef _circle_mbedtls_sslsocket_h
#define _circle_mbedtls_sslsocket_h

#include <mbedtls/ssl.h>
#include <circle-mbedtls/sslconfig.h>
#include <circle-mbedtls/adaptedsocket.h>
#include <circle/net/netsocket.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/ipaddress.h>
#include <circle/types.h>
#include <stdint.h>

namespace CircleMbedTLS {

// TODO: support server and UDP sockets

class CSSLSocket : public CNetSocket	/// SSL/TLS socket
{
public:
	CSSLSocket (CNetSubSystem *pNetSubSystem, int nProtocol);
	~CSSLSocket (void);

	/// \brief Setup SSL/TLS configuration prior to Connect()
	int Setup (const CSSLConfig &rConfig);

	/// \brief Setup hostname of server prior to Connect()
	int SetHostname (const char *pHostname);

	/// \brief Connect to foreign host/port (TCP), setup foreign host/port address (UDP)
	/// \param rForeignIP IP address of host to be connected
	/// \param nOwnPort   Number of port to be connected
	/// \return Status (0 success, < 0 on error)
	int Connect (CIPAddress &rForeignIP, u16 nForeignPort);

	/// \brief Connect to foreign host/port (TCP), setup foreign host/port address (UDP)
	/// \param pHost Hostname or IP address string of host to be connected
	/// \param pPort Decimal number string of port to be connected
	/// \return Status (0 success, < 0 on error)
	int Connect (const char *pHost, const char *pPort);

	/// \brief Send a message to a remote host
	/// \param pBuffer Pointer to the message
	/// \param nLength Length of the message
	/// \param nFlags  MSG_DONTWAIT (non-blocking operation) or 0 (blocking operation)
	/// \return Length of the sent message (< 0 on error)
	int Send (const void *pBuffer, unsigned nLength, int nFlags);

	/// \brief Receive a message from a remote host
	/// \param pBuffer Pointer to the message buffer
	/// \param nLength Size of the message buffer in bytes
	/// \param nFlags MSG_DONTWAIT (non-blocking operation) or 0 (blocking operation)
	/// \return Length of received message (0 with MSG_DONTWAIT if no message available, < 0 on error)
	int Receive (void *pBuffer, unsigned nLength, int nFlags);

	/// \brief Get IP address of connected remote host
	/// \return Pointer to IP address (four bytes, 0-pointer if not connected)
	const u8 *GetForeignIP (void) const;

private:
	static int ReceiveCallback (void *pParam, u8 *pBuffer, size_t nLength);
	static int ReceiveTimeoutCallback (void *pParam, u8 *pBuffer, size_t nLength,
					   uint32_t nTimeoutMs);
	static int SendCallback (void *pParam, const u8 *pBuffer, size_t nLength);

private:
	CNetSubSystem *m_pNetSubSystem;

	mbedtls_ssl_context m_Context;

	CAdaptedSocket *m_pSocket;

	boolean m_bConnected;
};

}

#endif
