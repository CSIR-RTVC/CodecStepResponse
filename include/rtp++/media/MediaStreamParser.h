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
#include <vector>
#include <boost/optional.hpp>
#include <rtp++/media/MediaSample.h>

namespace rtp_plus_plus {
namespace media {

/**
 * @brief Media Stream Parser base class. This class defined the interface that
 * a media stream parser has to implement.
 */
class MediaStreamParser
{
public:
  virtual ~MediaStreamParser()
  {

  }
  /**
   * @brief This method flushes any previously buffered samples from the parser
   */
  virtual std::vector<MediaSample> flush() = 0;

  /**
   * @brief This method parses the passed in buffer and returns a media sample if one could be extracted
   * @param[in] pBuffer: the input buffer
   * @param[in] uiBufferSize: size of the input buffer
   * @param[out] sampleSize: the size of the sample extracted if > 0.
   *                            If this is zero, more data is required.
   *                            If this is -1, an error occurred parsing the buffer
   * @return A media sample parsed from the stream
   */
  virtual boost::optional<MediaSample> extract(const uint8_t* pBuffer, uint32_t uiBufferSize,
                                               int32_t& sampleSize, bool bMoreDataWaiting) = 0;

  /**
   * @brief This method takes the passed in buffer and looks for all samples in the buffer.
   * It should be called to handle media samples that passed in from some event loop
   * e.g. VLC or DirectShow where each buffer may contain multple media frames.
   * Each method call should contain a new access unit in the buffer.
   * @param [in] pBuffer Buffer containing the raw bitstream
   * @param [in] uiBufferSize Size of the buffer containing the raw sample
   * @param [in] dCurrentAccessUnitPts The tiestamp that should be applied to all NAL units
   * @return A vector containing all samples found in the buffer.
   */
  virtual std::vector<MediaSample> extractAll(const uint8_t* pBuffer, uint32_t uiBufferSize,
                                              double dCurrentAccessUnitPts) = 0;

protected:
  MediaStreamParser()
  {
  }
};

} // media
} // rtp_plus_plus
