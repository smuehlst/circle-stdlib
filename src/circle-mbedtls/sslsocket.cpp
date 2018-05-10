//
// sslsocket.cpp
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
#include <circle-mbedtls/sslsocket.h>
#include <circle/net/dnsclient.h>
#include <circle/net/ipaddress.h>
#include <circle/net/in.h>
#include <circle/logger.h>
#include <mbedtls/net_sockets.h>
#include <assert.h>

using namespace CircleMbedTLS;

CSSLSocket::CSSLSocket (CNetSubSystem *pNetSubSystem, int nProtocol)
:	CNetSocket (pNetSubSystem),
	m_pNetSubSystem (pNetSubSystem),
	m_pSocket (new CAdaptedSocket (pNetSubSystem, nProtocol)),
	m_bConnected (FALSE)
{
	mbedtls_ssl_init (&m_Context);
}

CSSLSocket::~CSSLSocket (void)
{
	if (m_bConnected)
	{
		mbedtls_ssl_close_notify (&m_Context);
	}

	mbedtls_ssl_free (&m_Context);

	delete m_pSocket;
	m_pSocket = 0;

	m_pNetSubSystem = 0;
}

int CSSLSocket::Setup (const CSSLConfig &rConfig)
{
	assert (!m_bConnected);
	return mbedtls_ssl_setup (&m_Context, rConfig.Get ());
}

int CSSLSocket::SetHostname (const char *pHostname)
{
	assert (!m_bConnected);
	assert (pHostname != 0);
	return mbedtls_ssl_set_hostname (&m_Context, pHostname);
}

int CSSLSocket::Connect (CIPAddress &rForeignIP, u16 nForeignPort)
{
	if (m_pSocket == 0)
	{
		return MBEDTLS_ERR_SSL_ALLOC_FAILED;
	}

	assert (!m_bConnected);

	int nResult = m_pSocket->Connect (rForeignIP, nForeignPort);
	if (nResult != 0)
	{
		return nResult;
	}

	mbedtls_ssl_set_bio (&m_Context, m_pSocket, SendCallback, ReceiveCallback, 0);

	while ((nResult = mbedtls_ssl_handshake (&m_Context)) != 0)
	{
		if (   nResult != MBEDTLS_ERR_SSL_WANT_READ
		    && nResult != MBEDTLS_ERR_SSL_WANT_WRITE)
		{
			return nResult;
		}
	}

	m_bConnected = TRUE;

	u32 nFlags = mbedtls_ssl_get_verify_result (&m_Context);
	if (nFlags != 0)
	{
		char Info[512];
		mbedtls_x509_crt_verify_info (Info, sizeof Info, "x509: ", nFlags);

		CLogger::Get ()->Write ("sslsocket", LogWarning, Info);

		return MBEDTLS_ERR_X509_CERT_VERIFY_FAILED;
	}

	return 0;
}

int CSSLSocket::Connect (const char *pHost, const char *pPort)
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

int CSSLSocket::Receive (void *pBuffer, unsigned nLength, int nFlags)
{
	assert (m_pSocket != 0);
	m_pSocket->SetOptionBlocking (nFlags != MSG_DONTWAIT ? TRUE : FALSE);

	assert (m_bConnected);
	int nResult = mbedtls_ssl_read (&m_Context, reinterpret_cast<u8 *> (pBuffer), nLength);
	switch (nResult)
	{
	case MBEDTLS_ERR_SSL_WANT_READ:
	case MBEDTLS_ERR_SSL_WANT_WRITE:
		assert (nFlags == MSG_DONTWAIT);
		nResult = 0;
		break;

	case 0:			// EOF
		nResult = -1;
		break;
	}

	return nResult;
}

int CSSLSocket::Send (const void *pBuffer, unsigned nLength, int nFlags)
{
	assert (m_bConnected);
	return mbedtls_ssl_write (&m_Context, reinterpret_cast<const u8 *> (pBuffer), nLength);
}

const u8 *CSSLSocket::GetForeignIP (void) const
{
	if (m_pSocket == 0)
	{
		return 0;
	}

	return m_pSocket->GetForeignIP ();
}

int CSSLSocket::ReceiveCallback (void *pParam, u8 *pBuffer, size_t nLength)
{
	CAdaptedSocket *pSocket = reinterpret_cast<CAdaptedSocket *> (pParam);
	assert (pSocket != 0);
	return pSocket->Receive (pBuffer, nLength);
}

int CSSLSocket::ReceiveTimeoutCallback (void *pParam, u8 *pBuffer, size_t nLength,
					uint32_t nTimeoutMs)
{
	CAdaptedSocket *pSocket = reinterpret_cast<CAdaptedSocket *> (pParam);
	assert (pSocket != 0);
	return pSocket->ReceiveTimeout (pBuffer, nLength, nTimeoutMs);
}

int CSSLSocket::SendCallback (void *pParam, const u8 *pBuffer, size_t nLength)
{
	CAdaptedSocket *pSocket = reinterpret_cast<CAdaptedSocket *> (pParam);
	assert (pSocket != 0);
	return pSocket->Send (pBuffer, nLength);
}
