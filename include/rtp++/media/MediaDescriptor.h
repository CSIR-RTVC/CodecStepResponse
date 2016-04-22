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

namespace rtp_plus_plus {
namespace media {

class MediaTypeDescriptor
{
public:
  enum MediaType_t
  {
    MT_NOT_SET,
    MT_VIDEO,
    MT_AUDIO,
    MT_TEXT,
    MT_MUXED,
    MT_STREAM
  };
  enum MediaSubtype_t
  {
    MST_NOT_SET,
    MST_YUV_420P,
    MST_H264,
    MST_H264_SVC,
    MST_H265,
    MST_AMR
  };

  MediaTypeDescriptor(uint32_t uiWidth = 0, uint32_t uiHeight = 0, double dFps = 0.0)
    :m_eType(MT_NOT_SET),
      m_eSubtype(MST_NOT_SET),
      m_uiWidth(uiWidth),
      m_uiHeight(uiHeight),
      m_dFps(dFps)
  {

  }

  MediaTypeDescriptor(MediaType_t eType, MediaSubtype_t eSubType, uint32_t uiWidth = 0, uint32_t uiHeight = 0, double dFps = 0.0)
    :m_eType(eType),
      m_eSubtype(eSubType),
      m_uiWidth(uiWidth),
      m_uiHeight(uiHeight),
      m_dFps(dFps)
  {

  }

  uint32_t getWidth() const { return m_uiWidth; }
  uint32_t getHeight() const { return m_uiHeight; }
  double getFps() const { return m_dFps; }

  MediaType_t m_eType;
  MediaSubtype_t m_eSubtype;

  // video parameters
  uint32_t m_uiWidth;
  uint32_t m_uiHeight;
  double m_dFps;
  // TODO: audio parameters
};

} // media
} // rtp_plus_plus
