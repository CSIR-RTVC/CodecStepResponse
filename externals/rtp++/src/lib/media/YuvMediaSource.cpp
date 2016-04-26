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
#include "stdafx.h"
#include <numeric>
#include <rtp++/media/YuvMediaSource.h>

namespace rtp_plus_plus
{
namespace media
{

YuvMediaSource::YuvMediaSource(const std::string& sFilename, const uint32_t uiWidth, const uint32_t uiHeight, bool bRepeat, uint32_t uiRepetitions)
  :m_in(sFilename.c_str() , std::ifstream::in | std::ifstream::binary),
    m_rIn(m_in),
    m_eType(MT_YUV_420P),
    m_sFilename(sFilename),
    m_uiWidth(uiWidth),
    m_uiHeight(uiHeight),
    m_bEos(false),
    m_bRepeat(bRepeat),
    m_uiRepetitions(uiRepetitions),
    m_uiCurrentLoop(0),
    m_uiYuvFrameSize(static_cast<std::size_t>(uiWidth * uiHeight * 1.5)),
    m_iTotalFrames(0),
    m_uiCurrentFrame(0),
    m_buffer(new uint8_t[m_uiYuvFrameSize], m_uiYuvFrameSize)
{
  checkInputStream();
  parseStream();
}

YuvMediaSource::YuvMediaSource(std::istream& in1, const uint32_t uiWidth, const uint32_t uiHeight, bool bRepeat, uint32_t uiRepetitions)
  :m_rIn(in1),
    m_eType(MT_YUV_420P),
    m_uiWidth(uiWidth),
    m_uiHeight(uiHeight),
    m_bEos(false),
    m_bRepeat(bRepeat),
    m_uiRepetitions(uiRepetitions),
    m_uiCurrentLoop(0),
    m_uiYuvFrameSize(static_cast<std::size_t>(uiWidth * uiHeight * 1.5)),
    m_iTotalFrames(0),
    m_uiCurrentFrame(0),
    m_buffer(new uint8_t[m_uiYuvFrameSize], m_uiYuvFrameSize)
{
  VLOG(2) << "YUV properties width: " << m_uiWidth
          << " height: " << m_uiHeight
          << " YUV frame size: " << m_uiYuvFrameSize;

  checkInputStream();
  parseStream();
}


bool YuvMediaSource::isGood() const
{
  if (m_bEos) return false;
  return true;
}

boost::optional<MediaSample> YuvMediaSource::getNextMediaSample()
{
  // we only deal in AUs
  return boost::optional<MediaSample>();
}

std::vector<MediaSample> YuvMediaSource::readMediaSample()
{
  std::vector<MediaSample> mediaSamples;

  m_rIn.seekg(m_uiCurrentFrame * m_uiYuvFrameSize, std::ios_base::beg);
  m_rIn.read((char*) m_buffer.data(), m_uiYuvFrameSize);
  size_t count = static_cast<size_t>(m_rIn.gcount());
  assert(count == m_uiYuvFrameSize);
  Buffer mediaData(new uint8_t[m_uiYuvFrameSize], m_uiYuvFrameSize);
  memcpy((char*)mediaData.data(), (char*)(m_buffer.data()), m_uiYuvFrameSize);
  MediaSample mediaSample;
  mediaSample.setData(mediaData);
  mediaSamples.push_back(mediaSample);
  return mediaSamples;
}

std::vector<MediaSample> YuvMediaSource::getNextAccessUnit()
{
  assert(m_iTotalFrames > 0);
  if (m_uiCurrentFrame < m_iTotalFrames)
  {
    std::vector<MediaSample> vFrame = readMediaSample();
    ++m_uiCurrentFrame;
    return vFrame;
  }
  else
  {
    if (m_bRepeat)
    {
      if (m_uiRepetitions != 0 && m_uiCurrentLoop < m_uiRepetitions)
      {
        // set for next read
        m_uiCurrentFrame = 1;
        ++m_uiCurrentLoop;
        return readMediaSample();
      }
      else if (m_uiRepetitions == 0)
      {
        // set for next read
        m_uiCurrentFrame = 1;
        return readMediaSample();
      }
      else
      {
        m_bEos = true;
      }
    }
    else
    {
      m_bEos = true;
    }
  }
  return std::vector<MediaSample>();
}

void YuvMediaSource::checkInputStream()
{
  if (!m_rIn.good())  m_bEos = true;
}

void YuvMediaSource::parseStream()
{
  assert(m_uiYuvFrameSize != 0);
  m_rIn.seekg(0, std::ios_base::end);
  std::streampos uiTotalFileSize = m_rIn.tellg();
  m_rIn.seekg(0, std::ios_base::beg);
  m_iTotalFrames = uiTotalFileSize / m_uiYuvFrameSize;
  VLOG(12) << "YUV properties width: " << m_uiWidth
          << " height: " << m_uiHeight
          << " YUV frame size: " << m_uiYuvFrameSize
          << " Total frames: " << m_iTotalFrames;
}

} // media
} // rtp_plus_plus
