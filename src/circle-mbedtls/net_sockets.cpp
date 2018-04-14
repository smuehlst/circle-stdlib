//
// net_sockets.cpp
//
#include <mbedtls/net_sockets.h>
#include <circle-mbedtls/adaptedsocket.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/ipaddress.h>
#include <circle/net/in.h>
#include <circle/sched/scheduler.h>
#include <circle/types.h>
#include <assert.h>

using namespace CircleMbedTLS;

void mbedtls_net_init (mbedtls_net_context *ctx)
{
	assert (ctx != 0);
	ctx->fd = -1;
}

int mbedtls_net_connect (mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
	if (proto != MBEDTLS_NET_PROTO_TCP)
	{
		return MBEDTLS_ERR_NET_SOCKET_FAILED;
	}

	CAdaptedSocket *pSocket = new CAdaptedSocket (CNetSubSystem::Get (), IPPROTO_TCP);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_SSL_ALLOC_FAILED;
	}

	assert (ctx != 0);
	ctx->fd = pSocket->GetHandle ();
	assert (ctx->fd >= 0);

	return pSocket->Connect (host, port);
}

int mbedtls_net_bind (mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto)
{
	if (proto != MBEDTLS_NET_PROTO_TCP)
	{
		return MBEDTLS_ERR_NET_SOCKET_FAILED;
	}

	if (bind_ip != 0)
	{
		return MBEDTLS_ERR_NET_BIND_FAILED;
	}

	CAdaptedSocket *pSocket = new CAdaptedSocket (CNetSubSystem::Get (), IPPROTO_TCP);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_SSL_ALLOC_FAILED;
	}

	assert (ctx != 0);
	ctx->fd = pSocket->GetHandle ();
	assert (ctx->fd >= 0);

	return pSocket->Bind (port);
}

int mbedtls_net_accept (mbedtls_net_context *bind_ctx, mbedtls_net_context *client_ctx,
                        void *client_ip, size_t buf_size, size_t *ip_len)
{
	if (   client_ip != 0
	    && buf_size < sizeof (IP_ADDRESS_SIZE))
	{
		return MBEDTLS_ERR_NET_BUFFER_TOO_SMALL;
	}

	assert (bind_ctx != 0);
	CAdaptedSocket *pSocket = CAdaptedSocket::Get (bind_ctx->fd);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_NET_INVALID_CONTEXT;
	}

	int nResult = pSocket->Accept (client_ip);
	if (nResult < 0)
	{
		return MBEDTLS_ERR_NET_ACCEPT_FAILED;
	}

	assert (client_ctx != 0);
	client_ctx->fd = nResult;

	if (client_ip != 0)
	{
		assert (ip_len != 0);
		*ip_len = IP_ADDRESS_SIZE;
	}

	return 0;
}

int mbedtls_net_set_block (mbedtls_net_context *ctx)
{
	assert (ctx != 0);
	CAdaptedSocket *pSocket = CAdaptedSocket::Get (ctx->fd);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_NET_INVALID_CONTEXT;
	}

	pSocket->SetOptionBlocking (TRUE);

	return 0;
}

int mbedtls_net_set_nonblock (mbedtls_net_context *ctx)
{
	assert (ctx != 0);
	CAdaptedSocket *pSocket = CAdaptedSocket::Get (ctx->fd);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_NET_INVALID_CONTEXT;
	}

	pSocket->SetOptionBlocking (FALSE);

	return 0;
}

int mbedtls_net_recv (void *ctx, unsigned char *buf, size_t len)
{
	assert (ctx != 0);
	CAdaptedSocket *pSocket = CAdaptedSocket::Get (((mbedtls_net_context *) ctx)->fd);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_NET_INVALID_CONTEXT;
	}

	return pSocket->Receive (buf, len);
}

int mbedtls_net_recv_timeout (void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
	assert (ctx != 0);
	CAdaptedSocket *pSocket = CAdaptedSocket::Get (((mbedtls_net_context *) ctx)->fd);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_NET_INVALID_CONTEXT;
	}

	return pSocket->ReceiveTimeout (buf, len, timeout);
}

int mbedtls_net_send (void *ctx, const unsigned char *buf, size_t len)
{
	assert (ctx != 0);
	CAdaptedSocket *pSocket = CAdaptedSocket::Get (((mbedtls_net_context *) ctx)->fd);
	if (pSocket == 0)
	{
		return MBEDTLS_ERR_NET_INVALID_CONTEXT;
	}

	return pSocket->Send (buf, len);
}

void mbedtls_net_free (mbedtls_net_context *ctx)
{
	assert (ctx != 0);
	CAdaptedSocket *pSocket = CAdaptedSocket::Get (ctx->fd);

	delete pSocket;

	ctx->fd = -1;
}

void mbedtls_net_usleep (unsigned long usec)
{
	CScheduler::Get ()->usSleep (usec);
}
