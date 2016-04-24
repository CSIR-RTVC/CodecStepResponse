#pragma once
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include "Conversion.h"

namespace rtp_plus_plus {

class StringTokenizer
{
public:

  static std::vector<std::string> tokenize(const std::string& sText, const std::string& sTokens = " ", bool trim = false, bool bDropEmptyTokens = false)
  {
    std::vector<std::string> vTokens;
    std::string sTrimmed = sText;
    boost::algorithm::trim(sTrimmed);
    if (sTrimmed.empty()) return vTokens;

    size_t last_pos = 0;
    for (size_t pos = 0; pos < sTrimmed.length(); ++pos)
    {
      for (size_t tokenPos = 0; tokenPos != sTokens.length(); ++tokenPos)
      {
        if (sTrimmed[pos] == sTokens[tokenPos])
        {
          if (bDropEmptyTokens)
          {
            // avoid tokenising empty strings
            if (last_pos != pos)
            {
              std::string sTok = sTrimmed.substr(last_pos, pos - last_pos);
              boost::algorithm::trim(sTok);
              if (!sTok.empty())
              {
                vTokens.push_back(sTok);
              }
              last_pos = pos + 1;
            }
          }
          else
          {
            vTokens.push_back(sTrimmed.substr(last_pos, pos - last_pos));
            last_pos = pos + 1;
          }
        }
      }
    }
    // push back last token
    vTokens.push_back(sTrimmed.substr(last_pos));

    if (trim)
    {
      // remove white space
      BOOST_FOREACH(std::string& val, vTokens)
     {
       boost::algorithm::trim(val);
     }
    }

    return vTokens;
  }

  template <typename T>
  static std::vector<T> tokenizeV2(const std::string& sText, const std::string& sTokens = " ", bool trim = false)
  {
    std::vector<T> vTokens;
    size_t last_pos = 0;
    for (size_t pos = 0; pos < sText.length(); ++pos)
    {
      for (size_t tokenPos = 0; tokenPos != sTokens.length(); ++tokenPos)
      {
        if (sText[pos] == sTokens[tokenPos])
        {
          std::string sTemp = sText.substr(last_pos, pos - last_pos);
          if (trim) boost::algorithm::trim(sTemp);
          vTokens.push_back(convert<T>(sTemp, 0));
          last_pos = pos + 1;
        }
      }
    }
    // push back last token
    std::string sTemp = sText.substr(last_pos);
    if (trim) boost::algorithm::trim(sTemp);
    vTokens.push_back(convert<T>(sTemp, 0));

    return vTokens;
  }
};

} // rtp_plus_plus

