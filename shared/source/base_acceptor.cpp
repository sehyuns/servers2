// @file	base_acceptor.cpp
//
//

#include "base_acceptor.h"

#include "iocp.h"
#include "socket_async.h"

namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	BaseAcceptor::BaseAcceptor()
		: _acceptor(nullptr)
		, _iocp(nullptr)
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	BaseAcceptor::~BaseAcceptor()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	BaseAcceptor::initialize(const string& address_in, SHORT port_in, lib::Iocp* iocp_in)
	{
		_iocp = iocp_in;

		_acceptor = new lib::SocketAsync();

		INT32 result = _acceptor->initialize();
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = _acceptor->init_server(address_in, port_in);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = _iocp->associate(*_acceptor);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = Thread::register_thread(string("base_acceptor"), &BaseAcceptor::worker_thread);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	VOID	BaseAcceptor::close()
	{
		Thread::close();

		if (nullptr != _acceptor)
		{
			_acceptor->close();
			delete _acceptor;
			_acceptor = nullptr;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseAcceptor::start()
	{
		return Thread::start_all_threads();
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseAcceptor::acceptor_thread()
	{
		while (TRUE)
		{
			SOCKET client_socket = _acceptor->accept();
			if (INVALID_SOCKET == client_socket)
			{
				return ERR_SOCKET_ACCEPT;
			}

			printf("accepted new client connection!!(%d)\n", (INT32)client_socket);

			INT32 result = _iocp->associate(client_socket);
			if (ERR_SUCCEEDED != result)
			{
				printf("Failed to combine with iocp!!\n");
				continue;
			}

			process_new_session(client_socket);
		}
		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	BaseAcceptor::worker_thread()
	{
		INT32 result = ERR_SUCCEEDED;

		while (TRUE)
		{
			INT32 key = 0;
			UINT32 transferred_bytes = 0;
			LPOVERLAPPED overlapped = nullptr;
			result = _iocp->pop_income(key, transferred_bytes, &overlapped);
			if (ERR_SUCCEEDED != result)
			{
				printf("Failed to get income(%d)\n", key);
				continue;
			}

			lib::SocketAsync::AsyncOverlapped* async_overlapped = (lib::SocketAsync::AsyncOverlapped*)overlapped;
			switch (async_overlapped->get_type())
			{
			case lib::EO_ACCEPT:
				printf("accept new connection!!\n");
				break;
			case lib::EO_RECEIVE:
				printf("receive incomes!!!\n");
				break;
			case lib::EO_SEND:
				printf("send some data!!\n");
				break;
			case lib::EO_SHUTDOWN:
				printf("received shutdown command!!\n");
				_endthreadex(ERR_SUCCEEDED);
				return ERR_SUCCEEDED;
			default:
				printf("not detected!!!\n");
				break;
			}

			process_acceptor_incomes(key);
		}
		return ERR_SUCCEEDED;
	}
}