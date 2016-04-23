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

#include <string>
#include <boost/algorithm/string.hpp>
#include <rtp++/util/Conversion.h>

#define H265_PACKETIZATION_MODE "packetization-mode"
#define H265_PROFILE_LEVEL_ID "profile-level-id"
#define H265_SPROP_PARAMETER_SETS "sprop-parameter-sets"

namespace rtp_plus_plus {
namespace media {
namespace h265 {

/**
  * Parser for H265 format description
  */
struct H265FormatDescription
{
  /**
    * Constructor for H265 format description with minimal error checking.
    * @param sFmtp The fmtp line from an SDP description
    */
  H265FormatDescription(const std::string& sFmtp)
    :m_bValid(false)
  {
    std::vector<std::string> vResults;
    // if the string contains the format type there is a space before the fmtp string
    boost::algorithm::split(vResults, sFmtp, boost::algorithm::is_any_of(" ;"));

    if (vResults.empty()) return;

    // check for payload type
    bool bSuccess = true;
    uint8_t uiPayloadType = convert<uint8_t>(vResults[0], bSuccess);
    if (bSuccess)
    {
      PayloadType = uiPayloadType;
    }

    for ( size_t i = 0; i < vResults.size(); ++i) 
    {
      std::vector<std::string> params;
      boost::algorithm::split(params, vResults[i], boost::algorithm::is_any_of("="));
      if (params[0] == H265_PACKETIZATION_MODE)
      {
        PacketizationMode = params[1];
      }
      else if (params[0] == H265_PROFILE_LEVEL_ID)
      {
        ProfileLevelId = params[1];
      }
      else if (params[0] == H265_SPROP_PARAMETER_SETS)
      {
        SpropParameterSets = params[1];
        boost::algorithm::split(params, SpropParameterSets, boost::algorithm::is_any_of(","));
        if (params.size() == 2)
        {
          Sps = params[0];
          Pps = params[1];
        }
      }
    }

    // check if we managed to find all fields
    if (
        !PacketizationMode.empty() &&
        !ProfileLevelId.empty() &&
        !SpropParameterSets.empty() &&
        !Sps.empty() &&
        !Pps.empty()
       )
    {
      m_bValid = true;
    }
  }

  uint8_t PayloadType;
  std::string PacketizationMode;
  std::string ProfileLevelId;
  std::string SpropParameterSets;
  std::string Sps;
  std::string Pps;

private:
  bool m_bValid;
};

} // h265
} // media
} // rtp_plus_plus
