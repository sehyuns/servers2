// @file	base_connector.h
//
//

#ifndef _BASE_CONNECTOR_H_
#define _BASE_CONNECTOR_H_

#pragma once

#include "shared_defs.h"

#include "socket_async.h"
#include "thread.h"

#include <unordered_map>

namespace lib
{
	class Iocp;
}

namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	// @class	BaseConnector
	//
	// @brief	basic connector for client side. 다중 포인터에 접속이 가능하다.
	//			
	////////////////////////////////////////////////////////////////////////////////
	class BaseConnector : public lib::Thread<BaseConnector>
	{
	private:
		typedef		lib::Thread<BaseConnector>		Thread;

	public:
		BaseConnector();
		virtual ~BaseConnector();

	public:
		virtual INT32	initialize(lib::Iocp* iocp_in, SHORT thread_num_in=1);
		virtual VOID	close();

		virtual INT32	process_connector_incomes(INT32 key_in) = 0;

		INT32			start();

		INT32			add_connection(const string& address_in, SHORT port_in, SOCKET& socket_out);
		VOID			close_connection();

	public:
		INT32			worker_thread();

	public:
		BOOL			get_is_init()					{ return _is_init; }
		lib::SocketAsync*	get_socket(INT32 key_in);

	protected:
		typedef		std::unordered_map<INT32, lib::SocketAsync>		TSocketMap;
		TSocketMap			_socket_map;

		lib::Iocp*			_iocp;

		BOOL				_is_init;
	};
}

#endif