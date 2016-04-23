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
#include <iostream>
#include <string>
#include <rtp++/media/MediaSample.h>

namespace rtp_plus_plus
{
namespace media
{

class MediaSink
{
public:

  MediaSink(const std::string& sDest);
  virtual ~MediaSink();

  virtual void write(const MediaSample& mediaSample);
  virtual void writeAu(const std::vector<MediaSample>& mediaSamples);

protected:

  std::ofstream* m_pFileOut;
  std::ostream* m_out;
};

} // media
} // rtp_plus_plus
