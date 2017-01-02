// @file	base_server.cpp
//
//

#include "base_server.h"

#include "iocp.h"
#include "socket_async.h"

namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	BaseServer::BaseServer()
		: _iocp(nullptr)
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	BaseServer::~BaseServer()
	{

	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	BaseServer::initialize(const string& address_in, SHORT port_in)
	{
		_iocp = new lib::Iocp();

		INT32 result = _iocp->initialize();
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = BaseAcceptor::initialize(address_in, port_in, _iocp);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	VOID BaseServer::close()
	{
		BaseConnector::close();
		BaseAcceptor::close();

		if (nullptr != _iocp)
		{
			_iocp->close();
			delete _iocp;
			_iocp = nullptr;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseServer::process_acceptor_incomes(INT32 key_in)
	{
		TSessionMap::iterator iter = _session_map.find(key_in);
		if (iter == _session_map.end())
		{
			return ERR_SHARED_INVALID_SESSION;
		}

		Session& session = iter->second;

		session.make_packet();

		// push packet to specific container.

		session.complete_receive();

		return ERR_SUCCEEDED;;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseServer::process_connector_incomes(INT32 key_in)
	{
		key_in;
		return INT32();
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseServer::process_new_session(const SOCKET & socket_in)
	{
		INT32 result = make_new_session(socket_in);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseServer::server_start()
	{
		INT32 result = BaseAcceptor::start();
		if (ERR_SUCCEEDED != result)
		{
			close();
			return result;
		}
		
		result = BaseConnector::start();
		if (ERR_SUCCEEDED != result)
		{
			close();
			return result;
		}
		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseServer::connect_to(const string & address_in, SHORT port_in)
	{
		SOCKET socket = INVALID_SOCKET;

		if (FALSE == BaseConnector::get_is_init())
		{
			INT32 result = BaseConnector::initialize(_iocp);
			if (ERR_SUCCEEDED != result)
			{
				return result;
			}
		}

		INT32 result = BaseConnector::add_connection(address_in, port_in, socket);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = make_new_session(socket);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseServer::make_new_session(const SOCKET & socket_in)
	{
		std::pair<TSessionMap::iterator, bool> iter_result = _session_map.insert(TSessionMap::value_type((INT32)socket_in, Session()));
		if (false == iter_result.second)
		{
			return ERR_SOCKET_CREATE;
		}

		Session& session = iter_result.first->second;
		INT32 result = session.initialize(socket_in);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = session.receive();
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		return ERR_SUCCEEDED;
	}
}

