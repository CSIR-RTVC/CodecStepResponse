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
#include <boost/system/error_code.hpp>

namespace rtp_plus_plus {
namespace experimental {

/**
 * @brief The ICooperativeCodec class allows the network and scheduling layer to communicate with the codec layer
 */
class INetworkCodecCooperation
{
public:

  virtual ~INetworkCodecCooperation()
  {

  }

#if 0
  enum BitrateTargetPreciseness
  {
    BT_PRECISE_FRAME,  // target bitrate is precise per generated frame
    BT_PRECISE_AVERAGE // target bitrate is averaged over a period
  };
#endif

  virtual boost::system::error_code setBitrate(uint32_t uiTargetBitrateKbps) = 0;
#if 0
  virtual boost::system::error_code switchBitrate(uint32_t uiSwitchType) = 0;
  virtual boost::system::error_code generateIdr() = 0;
  virtual boost::system::error_code setMaxFrameSize(uint32_t uiMaxSizeBytesBytes) = 0;
  virtual boost::system::error_code setFramerate(uint32_t uiFramerate) = 0;
  virtual boost::system::error_code changeFramerate(uint32_t uiFramerate) = 0;
  virtual boost::system::error_code changeGopStructure() = 0;
  virtual boost::system::error_code updateReferencePicture() = 0;
#endif

private:

};

} // experimental
} // rtp_plus_plus
