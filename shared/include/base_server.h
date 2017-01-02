// @file	base_server.h
//
//

#ifndef _BASE_SERVER_H_
#define _BASE_SERVER_H_

#pragma once

#include "shared_defs.h"

#include "base_acceptor.h"
#include "base_connector.h"
#include "session.h"

#include <unordered_map>

namespace lib
{
	class Iocp;
}

namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	// @class	BaseServer
	//
	// @brief
	//			
	////////////////////////////////////////////////////////////////////////////////
	class BaseServer : public BaseAcceptor, public BaseConnector
	{
	public:
		BaseServer();
		virtual ~BaseServer();

	public:
		virtual INT32	initialize(const string& address_in, SHORT port_in);
		virtual VOID	close() override;
		
		virtual INT32	process_acceptor_incomes(INT32 key_in) override;
		virtual INT32	process_connector_incomes(INT32 key_in) override;
		virtual INT32	process_new_session(const SOCKET& socket_in) override;

		INT32			server_start();

		INT32			connect_to(const string& address_in, SHORT port_in);

		INT32			make_new_session(const SOCKET& socket_in);

	private:
		lib::Iocp*			_iocp;

		typedef		std::unordered_map<INT32, Session>		TSessionMap;
		TSessionMap			_session_map;
	};
}

#endif