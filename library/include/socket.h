// @file	socket.h
//
//

#ifndef _SOCKET_H_
#define _SOCKET_H_

#pragma once

#pragma comment(lib, "ws2_32")

#include "lib_defs.h"

namespace lib
{
	////////////////////////////////////////////////////////////////////////////////
	// @class	Socket
	//
	// @brief	Wrapping basic socket API. 
	//			
	////////////////////////////////////////////////////////////////////////////////
	class Socket
	{
	public:
		Socket();
		virtual ~Socket();

	public:
		virtual INT32	initialize(const SOCKET& socket_in=INVALID_SOCKET);
		virtual VOID	close();

		INT32			init_server(const string& address_in, SHORT port_in);
		INT32			init_client(const string& address_in, SHORT port_in);

		virtual SOCKET	accept();
		virtual INT32	connect();

	public:
		INT32			get_last_error()									{ return _last_error; }
		operator SOCKET() { return _socket; }

	protected:
		INT32			recorde_error();

		INT32			make_address(const string& address_in, SHORT port_in);
		virtual INT32	server_socket_option();
		virtual INT32	client_socket_option();

	protected:
		SOCKET			_socket;
		sockaddr_in		_address;

		INT32			_last_error;
	};
}

#endif