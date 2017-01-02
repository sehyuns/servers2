// @file	socket_async.h
//
//

#ifndef _SOCKET_ASYNC_H_
#define _SOCKET_ASYNC_H_

#pragma once

#include "socket.h"

#if defined(USE_ADVANCED_NETWORK)
#include <mswsock.h>
#endif

namespace lib
{
	class MessageBuffer;
}

namespace lib
{
	enum EOVERLAPPED_TYPE
	{
		  EO_ACCEPT = 0
		, EO_SEND
		, EO_RECEIVE
		, EO_SHUTDOWN
	};

	////////////////////////////////////////////////////////////////////////////////
	// @class	SocketAsync
	//
	// @brief	비동기 방식을 지원하는 socket API.
	//			
	////////////////////////////////////////////////////////////////////////////////
	class SocketAsync : public Socket
	{
	public:
		SocketAsync();
		~SocketAsync();

	public:
		virtual INT32	initialize(const SOCKET& socket_in = INVALID_SOCKET) override;
		virtual VOID	close() override;

	public:
#if defined(USE_ADVANCED_NETWORK)
		INT32			advanced_accept();
		INT32			advanced_connect();
#endif
		INT32			receive_async(MessageBuffer* buf_in);
		INT32			send_async(MessageBuffer* buf_in);

	public:
		struct AsyncOverlapped : public WSAOVERLAPPED 
		{
			INT32	_type;

			AsyncOverlapped() : _type(EO_ACCEPT)	{}
			VOID	set_type(INT32 type_in)			{ _type = type_in; }
			INT32	get_type()						{ return _type; }

		};

	private:
		AsyncOverlapped		_overlapped;

#if defined(USE_ADVANCED_NETWORK)
		LPFN_ACCEPTEX		_accept_func;
		LPFN_CONNECTEX		_connect_func;
#endif
	};
}

#endif