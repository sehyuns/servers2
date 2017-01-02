// @file	message.h
//
//

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#pragma once

#include "shared_defs.h"

namespace shared
{
	////////////////////////////////////////////////////////////////////////////////
	// @class	Message
	//
	// @brief
	//			
	////////////////////////////////////////////////////////////////////////////////
	class Message
	{
	public:
		explicit Message(INT32 msg_id_in)
			: _msg_id(msg_id_in)
			, _sequence(0)
		{
		}
		virtual ~Message()
		{
		}

	public:
		INT32			incr_sequence()		{ return ++_sequence; }

	private:
		INT32			_msg_id;
		INT32			_sequence;
	};
}

#endif