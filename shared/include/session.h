// @file	session.h
//
//

#ifndef _SESSION_H_
#define _SESSION_H_

#pragma once

#include "shared_defs.h"

#include "message_buffer.h"

namespace lib
{
	class SocketAsync;
}

namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	// @class	Session
	//
	// @brief	Network 접속자 정보.
	//			
	////////////////////////////////////////////////////////////////////////////////
	class Session
	{
	public:
		Session();
		~Session();

	public:
		INT32			initialize(const SOCKET& socket_in);
		INT32			initialize(lib::SocketAsync* socket_in);
		VOID			close();

		INT32			make_packet();

		INT32			receive();
		INT32			complete_receive();
		INT32			send();

	public:
		lib::MessageBuffer*		get_message_buffer()	{ return &_recv_buf; }

	private:
		lib::SocketAsync*		_socket;	// connector/acceptor에서 받아온다.

		lib::MessageBuffer		_recv_buf;
	};
}

#endif