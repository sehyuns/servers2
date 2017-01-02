// @file	base_acceptor.h
//
//

#ifndef _BASE_ACCEPTOR_H_
#define _BASE_ACCEPTOR_H_

#pragma once

#include "thread.h"

namespace lib
{
	class Iocp;
	class SocketAsync;
}

namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	// @class	BaseAcceptor
	//
	// @brief	Basic acceptor for server side.
	//			
	////////////////////////////////////////////////////////////////////////////////
	class BaseAcceptor : public lib::Thread<BaseAcceptor>
	{
		typedef		lib::Thread<BaseAcceptor>		Thread;

	public:
		BaseAcceptor();
		virtual ~BaseAcceptor();

	public:
		virtual INT32	initialize(const string& address_in, SHORT port_in, lib::Iocp* iocp_in);
		virtual VOID	close();

		virtual INT32	process_new_session(const SOCKET& socket_in) = 0;
		virtual INT32	process_acceptor_incomes(INT32 key_in) = 0;

		INT32			start();

	public:
		INT32			acceptor_thread();
		INT32			worker_thread();

	protected:
		lib::SocketAsync*	_acceptor;

		lib::Iocp*			_iocp;
	};
}

#endif