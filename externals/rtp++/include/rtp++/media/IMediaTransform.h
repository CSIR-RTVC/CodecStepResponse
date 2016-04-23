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
#include <boost/system/error_code.hpp>
#include <rtp++/media/MediaDescriptor.h>
#include <rtp++/media/MediaSample.h>

namespace rtp_plus_plus {
namespace media {

class IMediaTransform
{
public:
  virtual ~IMediaTransform()
  {

  }
  /**
   * @brief sets input type of transform
   */
  virtual boost::system::error_code setInputType(const MediaTypeDescriptor& in) = 0;
  /**
   * @brief configures transform: should be called before initialise()
   */
  virtual boost::system::error_code configure(const std::string& sName, const std::string& sValue) = 0;
  /**
   * @brief configures transform: should be called after setInputType() and configure()
   */
  virtual boost::system::error_code initialise() = 0;
  /**
   * @brief gets output media type: should be called after initialise()
   */
  virtual boost::system::error_code getOutputType(MediaTypeDescriptor& out) = 0;
  /**
   * @brief executes transform
   */
  virtual boost::system::error_code transform(const std::vector<MediaSample>& in, std::vector<MediaSample>& out, uint32_t& uiSize) = 0;

protected:

private:
  MediaTypeDescriptor m_in;
  MediaTypeDescriptor m_out;
};

} // media
} // rtp_plus_plus
