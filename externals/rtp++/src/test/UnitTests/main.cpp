#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#endif

/// Dynamic linking of boost test
#define BOOST_TEST_DYN_LINK
/// generate main entry points
#define BOOST_TEST_MODULE ts_shared master_test_suite

#ifdef _WIN32
// https://connect.microsoft.com/VisualStudio/feedback/details/737019
// http://social.msdn.microsoft.com/Forums/vstudio/en-US/878f09ff-408f-47a8-913e-15f3a983e237/stdmaketuple-only-allows-5-parameters-with-vc11
#define  _VARIADIC_MAX  6
#endif
#include <iostream>
#include <boost/test/unit_test.hpp>
#include "Media.h"

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

using namespace std;
using namespace rtp_plus_plus::test;

BOOST_AUTO_TEST_CASE( tc_test_1)
{
  BOOST_CHECK_EQUAL( true, true);
}

