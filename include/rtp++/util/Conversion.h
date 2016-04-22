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
#include <cstdint>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

/**
 * Numeric and bool string conversion functions
 */
// Should be deprecated since doesn't report failure
template <class T>
T convert(const std::string& sStringValue, T defaultValue )
{
  T val = defaultValue;
  std::istringstream istr(sStringValue);
  if (istr.good())
      istr >> val;
  return val;
}

template <class T>
T convert(const std::string& sStringValue, bool& bSuccess )
{
  T val;
  std::istringstream istr(sStringValue);
  istr >> val;
  bSuccess = !istr.fail() && istr.eof();
  return val;
}

// HACK to get the following to work
// uint8_t n = convert<uint8_t>("96")
// Without the overload, it simply forwards 
// the first digit into the temp val
// WARNING: these user is responsible to make sure that the value is in the valid range
template <>
inline uint8_t convert<uint8_t>(const std::string& sStringValue, uint8_t defaultValue)
{
  int32_t val(0);
  std::istringstream istr(sStringValue);
  istr >> val;
  return static_cast<uint8_t>(val);
}

template <>
inline uint8_t convert<uint8_t>(const std::string& sStringValue, bool& bSuccess )
{
  int32_t val(0);
  std::istringstream istr(sStringValue);
  istr >> val;
  bSuccess = !istr.fail() && istr.eof();
  return static_cast<uint8_t>(val);
}
// HACK END

template <>
inline bool convert<bool>(const std::string& sValue, bool defaultValue)
{
  if (sValue == "1") return true;
  else if (sValue == "0" || sValue.empty()) return false;
  else
  {
    std::string sCopy(sValue);
    boost::algorithm::to_lower(sCopy);
    if (sCopy == "false")
      return false;
    else 
      return true;
  }
}

template <class T>
std::string toString(T t)
{
  std::ostringstream ostr;
  ostr << t;
  return ostr.str();
}

template <class T>
std::string toString(const std::vector<T>& vecT, char delim = ' ')
{
#if 0
  std::ostringstream ostr;
  std::for_each(vecT.begin(), vecT.end(), [&ostr,delim](const T t)
  {
    ostr << t << delim;
  });
  return ostr.str();
#else
  std::ostringstream ostr;
  if (vecT.empty()) return "";
  ostr << vecT[0];
  for (size_t i = 1; i < vecT.size(); ++i)
  {
    ostr << delim << vecT[i];
  }
  return ostr.str();

#endif
}

// Converts "1", "true", "TRUE" to true and "0", "FALSE" and "false" to false
static bool stringToBool(const std::string& sValue)
{
  if (sValue == "1" || sValue == "TRUE" || sValue == "true")
  {
    return true;
  }
  else if (sValue == "0" || sValue == "FALSE" || sValue == "false")
  {
    return false;
  }
  else
  {
    // Default
    return true;
  }
}

static bool i2b(uint32_t uiNum)
{
  return (uiNum == 0) ? false: true;
}

static std::string boolToString(bool bValue)
{
  std::string sResult = bValue?"1":"0";
  return sResult;
}

