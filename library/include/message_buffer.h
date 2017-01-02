// @file	message_buffer.h
//
//

#ifndef _MESSAGE_BUFFER_H_
#define _MESSAGE_BUFFER_H_

#pragma once

#include "lib_defs.h"

namespace lib
{
	////////////////////////////////////////////////////////////////////////////////
	// @class	MessageBuffer
	//
	// @brief	Wrapping WSABUF. 비동기 통신에서 사용되는 버퍼
	//			
	////////////////////////////////////////////////////////////////////////////////
	class MessageBuffer
	{
	public:
		explicit MessageBuffer(INT32 length_in = 1024)
			: _buf_length(length_in)
			, _buffer(nullptr)
		{
			_buffer = new CHAR[_buf_length];
			memset(_buffer, 0, _buf_length);

			_wsa_buf.buf = _buffer;
			_wsa_buf.len = _buf_length;
		}

		~MessageBuffer()
		{
			if (nullptr != _buffer)
			{
				delete _buffer;
				_buffer = nullptr;
			}
		}

	public:
		WSABUF*			get_wsa_buf()										{ return &_wsa_buf; }
		CHAR*			get_buffer()										{ return _buffer; }
		INT32			get_buf_length()									{ return _buf_length; }

		VOID			set_buffer(const BYTE* buf_in, INT32 length_in)		{ memcpy_s(_buffer, _buf_length, buf_in, length_in); }

		VOID			reset()												{ memset(_buffer, 0, _buf_length); }

	public:
		operator CHAR*()													{ return _buffer; }

	private:
		WSABUF			_wsa_buf;
		CHAR*			_buffer;
		INT32			_buf_length;
	};
}

#endif