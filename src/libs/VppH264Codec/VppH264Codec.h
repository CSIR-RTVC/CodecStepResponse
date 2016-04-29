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

#ifdef _WIN32
#ifdef VppH264Codec_EXPORTS
#define VPP_H264_API __declspec(dllexport)
#else
#define VPP_H264_API __declspec(dllimport)
#endif
#else
#define VPP_H264_API 
#endif

// fwd
class ICodecv2;

class VPP_H264_API VppH264Codec : public rtp_plus_plus::media::IVideoCodecTransform
{
public:

  VppH264Codec();
  ~VppH264Codec();
  /**
   * @brief @ITransform
   */
  virtual boost::system::error_code setInputType(const rtp_plus_plus::media::MediaTypeDescriptor& in);
  /**
   * @brief @ITransform
   */
  virtual boost::system::error_code configure(const std::string& sName, const std::string& sValue);
  /**
   * @brief @ITransform
   */
  virtual boost::system::error_code initialise();
  /**
   * @brief @ITransform
   */
  virtual boost::system::error_code getOutputType(rtp_plus_plus::media::MediaTypeDescriptor& out);
  /**
   * @brief @ITransform
   */
  virtual boost::system::error_code transform(const std::vector<rtp_plus_plus::media::MediaSample>& in, std::vector<rtp_plus_plus::media::MediaSample>& out, uint32_t& uiSize);
  /**
   * @brief @ICooperativeCodec
   */
  virtual boost::system::error_code setBitrate(uint32_t uiTargetBitrate);
private:

  rtp_plus_plus::media::MediaTypeDescriptor m_in;
  rtp_plus_plus::media::MediaTypeDescriptor m_out;
  
  ICodecv2* m_pCodec;

  uint32_t m_uiIFramePeriod;
  uint32_t m_uiCurrentFrame;
  uint32_t m_uiFrameBitLimit;
  uint32_t m_uiMode;
  uint32_t m_uiRateControlModelType;
  bool m_bNotifyOnIFrame;

  uint32_t m_uiEncodingBufferSize;
  rtp_plus_plus::Buffer m_encodingBuffer;
};

