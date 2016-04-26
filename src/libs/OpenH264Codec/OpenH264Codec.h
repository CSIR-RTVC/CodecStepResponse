/**********
    This file is part of CodecStepResponse.

    CodecStepResponse is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CodecStepResponse is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CodecStepResponse.  If not, see <http://www.gnu.org/licenses/>.
**********/
// "CSIR"
// Copyright (c) 2016 CSIR.  All rights reserved.
#pragma once
#include <rtp++/media/IVideoCodecTransform.h>
#include <rtp++/util/Buffer.h>

#ifdef _WIN32
#ifdef OpenH264Codec_EXPORTS
#define Open_H264_API __declspec(dllexport)
#else
#define Open_H264_API __declspec(dllimport)
#endif
#else
#define Open_H264_API 
#endif

class ISVCEncoder;

class Open_H264_API OpenH264Codec : public rtp_plus_plus::media::IVideoCodecTransform
{
public:
  /**
   * @brief OpenH264Codec
   */
  OpenH264Codec();
  /**
   * @brief OpenH264Codec
   */
  ~OpenH264Codec();
  /**
   * @brief @IMediaTransform
   */
  virtual boost::system::error_code setInputType(const rtp_plus_plus::media::MediaTypeDescriptor& in);
  /**
   * @brief @IMediaTransform
   */
  virtual boost::system::error_code configure(const std::string& sName, const std::string& sValue);
  /**
   * @brief @IMediaTransform
   */
  virtual boost::system::error_code initialise();
  /**
   * @brief @IMediaTransform
   */
  virtual boost::system::error_code getOutputType(rtp_plus_plus::media::MediaTypeDescriptor& out);
  /**
   * @brief @IMediaTransform
   */
  virtual boost::system::error_code transform(const std::vector<rtp_plus_plus::media::MediaSample>& in,
                                              std::vector<rtp_plus_plus::media::MediaSample>& out,
                                              uint32_t& uiSize);
  /**
   * @brief @INetworkCodecCooperation
   */
  virtual boost::system::error_code setBitrate(uint32_t uiTargetBitrate);

private:

  rtp_plus_plus::media::MediaTypeDescriptor m_in;
  rtp_plus_plus::media::MediaTypeDescriptor m_out;
  
  ISVCEncoder* m_pCodec;
  uint32_t m_uiTargetBitrate;
  bool m_bInitialised;
  uint32_t m_uiEncodingBufferSize;
  rtp_plus_plus::Buffer m_encodingBuffer;
};
