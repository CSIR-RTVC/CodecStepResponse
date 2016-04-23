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
#include <rtp++/media/MediaSink.h>

namespace rtp_plus_plus
{
namespace media
{

MediaSink::MediaSink(const std::string& sDest)
  :m_pFileOut(0),
    m_out(&std::cout)
{
  VLOG(2) << "Writing incoming media to " << sDest;
  if (sDest != "cout")
  {
    m_pFileOut = new std::ofstream(sDest.c_str(), std::ofstream::binary);
    m_out = m_pFileOut;
  }
}

MediaSink::~MediaSink()
{
  if (m_pFileOut)
  {
    m_pFileOut->close();
    delete m_pFileOut;
  }
}

void MediaSink::write(const MediaSample& mediaSample)
{
  m_out->write((const char*) mediaSample.getDataBuffer().data(), mediaSample.getDataBuffer().getSize());
}

void MediaSink::writeAu(const std::vector<MediaSample>& mediaSamples)
{
  for (const MediaSample& mediaSample : mediaSamples)
  {
    m_out->write((const char*) mediaSample.getDataBuffer().data(), mediaSample.getDataBuffer().getSize());
  }
}

} // media
} // rtp_plus_plus
