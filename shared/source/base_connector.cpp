// @file	base_connector.cpp
//
//

#include "base_connector.h"

#include "iocp.h"
#include "socket_async.h"


namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	BaseConnector::BaseConnector()
		: _iocp(nullptr)
		, _is_init(FALSE)
	{

	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	BaseConnector::~BaseConnector()
	{

	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	BaseConnector::initialize(lib::Iocp* iocp_in, SHORT thread_num_in)
	{
		_iocp = iocp_in;

		INT32 result = Thread::register_thread(string("connector_worker"), &BaseConnector::worker_thread, thread_num_in);
		if (ERR_SUCCEEDED != result)
		{
			close();
			return result;
		}

		_is_init = TRUE;

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	VOID	BaseConnector::close()
	{
		Thread::close();
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseConnector::start()
	{
		return Thread::start_all_threads();
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32 BaseConnector::add_connection(const string & address_in, SHORT port_in, SOCKET& socket_out)
	{
		lib::SocketAsync connector;

		INT32 result = connector.initialize();
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = connector.init_client(address_in, port_in);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		result = _iocp->associate(connector);
		if (ERR_SUCCEEDED != result)
		{
			return result;
		}

		_socket_map.insert(TSocketMap::value_type((INT32)connector, connector));

		socket_out = connector;

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	INT32	BaseConnector::worker_thread()
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

			process_connector_incomes(key);
		}
		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	lib::SocketAsync * BaseConnector::get_socket(INT32 key_in)
	{
		TSocketMap::iterator iter = _socket_map.find(key_in);
		if (iter == _socket_map.end())
			return nullptr;

		return &iter->second;
	}
}