// @file	socket.cpp
//
//

#include "socket.h"

#include <ws2tcpip.h>

namespace lib
{
	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	Socket::Socket()
		: _socket(INVALID_SOCKET), 
		  _last_error(0)
	{

	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	Socket::~Socket()
	{

	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	Socket::initialize(const SOCKET& socket_in)
	{
		if (INVALID_SOCKET == socket_in)
		{
			_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (INVALID_SOCKET == _socket)
			{
				recorde_error();
				return ERR_SOCKET_CREATE;
			}
		}
		else
			_socket = socket_in;

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	VOID	Socket::close()
	{
		if (INVALID_SOCKET != _socket)
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	Socket::init_server(const string& address_in, SHORT port_in)
	{
		INT32 result = make_address(address_in, port_in);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = server_socket_option();
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = bind(_socket, (const sockaddr*)&_address, sizeof(_address));
		if (SOCKET_ERROR == result)
		{
			recorde_error();
			return ERR_SOCKET_BIND;
		}

		result = listen(_socket, DEFAULT_LISTEN_COUNT);
		if (SOCKET_ERROR == result)
		{
			recorde_error();
			return ERR_SOCKET_LISTEN;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	Socket::init_client(const string& address_in, SHORT port_in)
	{
		INT32 result = make_address(address_in, port_in);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = client_socket_option();
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	SOCKET Socket::accept()
	{
#if defined(_DEBUG)
		sockaddr_in client_addr;
		INT32 addr_len = sizeof(client_addr);
		memset(&client_addr, 0, addr_len);
		SOCKET client = WSAAccept(_socket, (sockaddr*)&client_addr, &addr_len, NULL, NULL);
#else
		SOCKET client = WSAAccept(_socket, NULL, NULL, NULL, NULL);
#endif
		if (INVALID_SOCKET == client)
		{
			recorde_error();
		}

#if defined(_DEBUG)
		// log client address
#endif

		return client;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 Socket::connect()
	{
		INT32 result = WSAConnect(_socket, (sockaddr*)&_address, sizeof(_address), NULL, NULL, NULL, NULL);
		if (SOCKET_ERROR == result)
		{
			recorde_error();
			return ERR_SOCKET_CONNECT;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	Socket::make_address(const string& address_in, SHORT port_in)
	{
		memset(&_address, 0, sizeof(_address));

		_address.sin_family = AF_INET;
		_address.sin_port = htons(port_in);
		INT result = inet_pton(AF_INET, address_in.c_str(), &_address.sin_addr.s_addr);
		if (result < 0)
		{
			recorde_error();
			return ERR_SOCKET_ADDRESS;
		}
		else if (0 == result)	// address 입력값이 제대로 되지 않았다.
		{
			return ERR_SOCKET_ADDRESS;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 Socket::server_socket_option()
	{
		DWORD opt_value = 1;
		int opt_len = sizeof(opt_value);
		if (SOCKET_ERROR == setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&opt_value, opt_len))
		{
			recorde_error();
			return ERR_SOCKET_OPTION;
		}

		if (SOCKET_ERROR == setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_value, opt_len))
		{
			recorde_error();
			return ERR_SOCKET_OPTION;
		}

		struct linger server_linger = { 1, 0 };
		if (SOCKET_ERROR == setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&server_linger, sizeof(struct linger)))
		{
			recorde_error();
			return ERR_SOCKET_OPTION;
		}

#if defined(TURN_OFF_NAGLE_ALGORITHEM)
		if (SOCKET_ERROR == setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_value, opt_len))
		{
			recorde_error();
			return ERR_SOCKET_OPTION;
		}
#endif

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 Socket::client_socket_option()
	{
		DWORD opt_value = 1;
		int opt_len = sizeof(opt_value);
		if (SOCKET_ERROR == setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&opt_value, opt_len))
		{
			recorde_error();
			return ERR_SOCKET_OPTION;
		}

		struct linger server_linger = { 1, 0 };
		if (SOCKET_ERROR == setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&server_linger, sizeof(struct linger)))
		{
			recorde_error();
			return ERR_SOCKET_OPTION;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	Socket::recorde_error()
	{
		return _last_error = WSAGetLastError();
	}
}