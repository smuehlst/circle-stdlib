//
// adaptedsocket.h
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
#ifndef _circle_mbedtls_adaptedsocket_h
#define _circle_mbedtls_adaptedsocket_h

#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>
#include <circle/usb/netdevice.h>
#include <circle/ptrarray.h>
#include <circle/types.h>

namespace CircleMbedTLS {

class CAdaptedSocket	/// Helper class for adaption of the mbed TLS networking routines
{
public:
	CAdaptedSocket (CNetSubSystem *pNetSubSystem, int nProtocol);
	CAdaptedSocket (CNetSubSystem *pNetSubSystem, CSocket *pSocket);
	~CAdaptedSocket (void);

	int Connect (CIPAddress &rForeignIP, u16 nForeignPort);
	int Connect (const char *pHost, const char *pPort);

	int Bind (const char *pPort);
	int Accept (void *pForeignIP);

	int Receive (void *pBuffer, unsigned nLength);
	int ReceiveTimeout (void *pBuffer, unsigned nLength, unsigned nTimeoutMs);
	int Send (const void *pBuffer, unsigned nLength);

	void SetOptionBlocking (boolean bBlock);

	const u8 *GetForeignIP (void) const;

	int GetHandle (void) const;
	static CAdaptedSocket *Get (int nHandle);

private:
	static int AddSocket (CAdaptedSocket *pSocket);
	static void DeleteSocket (int nHandle);

private:
	CNetSubSystem *m_pNetSubSystem;
	CSocket *m_pSocket;

	int m_nHandle;				// aka file descriptor

	u8 m_RxBuffer[FRAME_BUFFER_SIZE];
	size_t m_nRxBytesValid;
	size_t m_nRxBytesUsed;

	boolean m_bBlock;

	static CPtrArray s_SocketArray;		// indexed using handle
};

}

#endif
