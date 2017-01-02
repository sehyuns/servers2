// @file	lib_defs.h
//
//

#ifndef _LIB_DEFS_H_
#define _LIB_DEFS_H_

#pragma once


#include <string>
#include <tchar.h>
#include <stdlib.h>

#include <winsock2.h>

#include "lib_error.h"

using namespace std;

typedef		std::basic_string<TCHAR>		tstring;




#define		DEFAULT_LISTEN_COUNT		5

#define		TURN_OFF_NAGLE_ALGORITHEM		1

#endif