// @file	thread.h
//
//

#ifndef _THREAD_H_
#define _THREAD_H_

#pragma once

#include "lib_defs.h"

#include <process.h>
#include <unordered_map>

namespace lib
{

	////////////////////////////////////////////////////////////////////////////////
	// @class	Thread
	//
	// @brief	
	//			
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class Thread
	{
	public:
		struct ThreadArg
		{
			ThreadArg(void* obj_in, const string& name_in) : _object(obj_in), _name(name_in) {}

			void*		_object;
			string		_name;
		};

		typedef		INT32(T::*ThreadFunc)();

	public:
		Thread() {}
		virtual ~Thread() {}

		INT32			initialize() {}
		VOID			close() {}

	public:
		INT32			register_thread(const string& name_in, ThreadFunc func_in, SHORT thread_num_in = 1);

		INT32			start_all_threads();
		INT32			stop_all_threads();

		INT32			start_thread(const string& name_in);
		INT32			stop_thread(const string& name_in);

	public:
		static unsigned __stdcall thread_main(void* argument);
		INT32			run_thread(const string& name_in, void* object_in);

	private:

		typedef	std::vector<HANDLE>							THandleList;
		typedef	std::unordered_map<string, THandleList>		THandleMap;
		THandleMap	_handle_map;

		typedef	std::unordered_map<string, ThreadArg>		TArgumentMap;
		TArgumentMap	_argument_map;

		typedef	std::unordered_map<string, ThreadFunc>		TFuncMap;
		TFuncMap		_func_map;
	};


	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	INT32	Thread<T>::register_thread(const string& name_in, ThreadFunc func_in, SHORT thread_num_in)
	{
		THandleMap::iterator iter = _handle_map.find(name_in);
		if (iter != _handle_map.end())
			return ERR_THREAD_CREATE_THREAD;

		std::pair<TArgumentMap::iterator, bool> arg_result = _argument_map.insert(TArgumentMap::value_type(name_in, ThreadArg(this, name_in)));
		if (false == arg_result.second)
		{
			return ERR_THREAD_CREATE_THREAD;
		}

		std::pair<TFuncMap::iterator, bool> func_result = _func_map.insert(TFuncMap::value_type(name_in, func_in));
		if (false == func_result.second)
		{
			return ERR_THREAD_CREATE_THREAD;
		}

		THandleList handle_list;
		HANDLE thread_handle = INVALID_HANDLE_VALUE;
		for (INT32 ii = 0; ii < thread_num_in; ++ii)
		{
			thread_handle = (HANDLE)_beginthreadex(nullptr, 0, &thread_main, &(arg_result.first->second), CREATE_SUSPENDED, nullptr);
			handle_list.push_back(thread_handle);
		}

		_handle_map.insert(THandleMap::value_type(name_in, handle_list));

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	INT32	Thread<T>::start_all_threads()
	{
		for (auto& iter_ii : _handle_map)
		{
			for (auto& iter_jj : iter_ii.second)
			{
				ResumeThread(iter_jj);
			}
		}

		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	INT32	Thread<T>::stop_all_threads()
	{
		_endthreadex(0);
		return ERR_SUCCEEDED;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	unsigned Thread<T>::thread_main(void * argument)
	{
		ThreadArg* thread_arg = (ThreadArg*)argument;
		Thread* thread = (Thread*)thread_arg->_object;
		return thread->run_thread(thread_arg->_name, thread);
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	INT32 Thread<T>::run_thread(const string & name_in, void* object_in)
	{
		TFuncMap::iterator iter = _func_map.find(name_in);
		if (iter == _func_map.end())
			return -1;

		//T temp;
		//return (temp.*(iter->second))();

		T* object = (T*)object_in;
		return ((*object).*(iter->second))();
	}
}

#endif
