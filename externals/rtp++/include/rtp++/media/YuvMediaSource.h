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
#include <fstream>
#include <string>
#include <vector>
#include <rtp++/media/MediaSample.h>
#include <rtp++/media/MediaSource.h>
#include <rtp++/util/Buffer.h>

namespace rtp_plus_plus {
namespace media {

/**
 * @brief YUV media source
 */
class YuvMediaSource : public MediaSource
{
public:
  /**
   * @brief YuvMediaSource
   * @param[in] sFilename The name of the file containing the Annex B bitstream
   * @param[in] uiWidth Width of YUV media
   * @param[in] uiHeight Height of YUV media
   * @param[in] bLoopSource Configures the source to loop on end of stream
   * @param[in] uiLoopCount Configures the number of times the source is looped
   * IFF bLoopSource is true. A value of 0 means that the source will loop
   * indefinitely
   */
  YuvMediaSource(const std::string& sFilename, const uint32_t uiWidth, const uint32_t uiHeight, bool bLoopSource, uint32_t uiLoopCount);
  /**
   * @brief YuvMediaSource
   * @param in1 A reference to the istream that has opened the Annex B stream
   * @param[in] uiWidth Width of YUV media
   * @param[in] uiHeight Height of YUV media
   * @param bLoopSource Configures the source to loop on end of stream
   * @param uiLoopCount Configures the number of times the source is looped
   * IFF bLoopSource is true. A value of 0 means that the source will loop
   * indefinitely
   */
  YuvMediaSource(std::istream& in1, const uint32_t uiWidth, const uint32_t uiHeight, bool bLoopSource, uint32_t uiLoopCount);
  /**
   * @brief isGood This method can be used to check if NAL units can be read. If the end of
   * stream is reached and the source is not configured to loop or if the maximum loop count
   * is reached, this will return false. This will also return false if the file could not be
   * found or if there was an error parsing the file.
   * @return if the NalUnitMediaSource is in a state to be read from
   */
  bool isGood() const override;
  /**
   * @brief Overridden from MediaSource. getNextMediaSample() does not apply to a NAL unit source as
   * we only want to deal with entire AUs
   * @return A null pointer
   */
  boost::optional<MediaSample> getNextMediaSample() override;
  /**
   * @brief Overridden from MediaSource. getNextAccessUnit() returns the next access unit in the source.
   * @return The next access unit in the source and an empty vector if isGood() == false
   */
  std::vector<MediaSample> getNextAccessUnit() override;

private:
  /**
   * @brief checkInputStream makes sure that the stream is in a good state and otherwise updates m_bEos
   */
  void checkInputStream();
  /**
   * @brief parseAnnexBStream parses the stream and extracts the NAL unit and access unit info
   */
  void parseStream();
  /**
   * @brief readMediaSample
   * @return
   */
  std::vector<MediaSample> readMediaSample();
private:

  // stream handle when reading from file
  std::ifstream m_in;
  // stream reference used for actual read
  std::istream& m_rIn;
  // for now we only support YUV420
  enum MediaType
  {
    MT_YUV_420P,
  };
  MediaType m_eType;
  std::string m_sFilename;
  uint32_t m_uiWidth;
  uint32_t m_uiHeight;
  // state of source
  bool m_bEos;
  // if source should loop
  bool m_bLoopSource;
  // total number of loops if m_bLoopSource
  uint32_t m_uiLoopCount;
  // Current loop
  uint32_t m_uiCurrentLoop;
  // frame size
  std::size_t m_uiYuvFrameSize;
  // total frames
  int64_t m_iTotalFrames;
  // current frame
  uint32_t m_uiCurrentFrame;
  // buffer to read data into
  Buffer m_buffer;
};

} // media
} // rtp_plus_plus
