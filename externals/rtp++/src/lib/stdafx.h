/**********
This file is part of rtp++ .

rtp++ is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

rtp++ is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with rtp++.  If not, see <http://www.gnu.org/licenses/>.

**********/
// "CSIR"
// Copyright (c) 2016 CSIR.  All rights reserved.
#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#endif

// for boost 1.56 + VS2013 warnings
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Included before CoreDecl.h
// To prevent double inclusion of winsock on windows
#ifdef _WIN32
#define NOMINMAX
#include <WinSock2.h>
#endif

//#include "CoreDecl.h"

#define BOOST_TEST_DYN_LINK

#ifdef _WIN32
#pragma warning(push)     // disable for this header only
#pragma warning(disable:4251) 
// To get around compile error on windows: ERROR macro is defined
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>
#ifdef _WIN32
#pragma warning(pop)     // restore original warning level
#endif

// Define this directive in both CorePch.h AND the application to be debugged
// #define BOOST_ASIO_ENABLE_HANDLER_TRACKING

/**
 * @def LOG_MODIFY_WITH_CARE Use this sentinel for any logging that needs to be extracted
 * from the log files and should be changed carefully, since the scripts might break.
 */

#define LOG_MODIFY_WITH_CARE "_LogSentinel_"

