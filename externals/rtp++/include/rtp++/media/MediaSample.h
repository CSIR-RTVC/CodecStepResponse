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
#include <deque>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <rtp++/util/Buffer.h>

namespace rtp_plus_plus
{
namespace media
{

/**
 * @brief This class represents a media sample.
 */
class MediaSample
{
public:
  /**
   * @brief Constructor
   */
  MediaSample()
   :m_dStartTime(0.0),
     m_bMarker(false),
     m_uiRtpTime(0),
     m_iDecodingOrderNumber(-1),
     m_iFlowIdHint(-1),
     m_iStartCodeLengthHint(-1),
     m_bNaluContainsStartCode(false)
  {

  }
  /**
   * @brief Getter for start time
   */
  double getStartTime() const { return m_dStartTime; }
  /**
   * @brief Setter for start time
   */
  void setStartTime( double dStartTime ) { m_dStartTime = dStartTime; }
  /**
   * @brief Getter for presentation time
   */
  boost::posix_time::ptime getPresentationTime() const { return m_tPresentation; }
  /**
   * @brief Setter for presentation time
   */
  void setPresentationTime(const boost::posix_time::ptime& tPresentation) { m_tPresentation = tPresentation; }
  /**
   * @brief Getter for payload size 
   */
  uint32_t getPayloadSize() const { return m_data.getSize(); }
  /**
   * @brief Getter for data buffer 
   */
  const Buffer& getDataBuffer() const { return m_data;}
  /**
   * @brief Setter for data buffer. NB: takes ownership of data 
   */
  void setData(uint8_t* pData, uint32_t uiSize)
  {
    m_data.setData(pData, uiSize);
  }
  /**
   * @brief Setter for data buffer 
   */
  void setData(Buffer data)
  {
    m_data = data;
  }
  /**
   * @brief Getter for marker
   */
  bool isMarkerSet() const { return m_bMarker; }
  /**
   * @brief Setter for marker
   */
  void setMarker(bool bValue) { m_bMarker = bValue; }
  /**
   * @brief Getter for RTP time
   */
  uint32_t getRtpTime() const { return m_uiRtpTime; }
  /**
   * @brief Setter for RTP time
   */
  void setRtpTime(uint32_t uiRtpTime) { m_uiRtpTime = uiRtpTime; }
  /**
   * @brief Getter for decoding order number
   */
  int16_t getDecodingOrderNumber() const { return m_iDecodingOrderNumber;}
  /**
   * @brief Setter for decoding order number
   */
  void setDecodingOrderNumber(int16_t iDecodingOrderNumber) { m_iDecodingOrderNumber = iDecodingOrderNumber; }
  /**
   * @brief Getter for flow id hint
   */
  int32_t getFlowIdHint() const { return m_iFlowIdHint;}
  /**
   * @brief Setter for flow id hint
   */
  void setFlowIdHint(int32_t iFlowIdHint) { m_iFlowIdHint = iFlowIdHint; }
  /**
   * @brief Getter for start code length hint
   */
  int32_t getStartCodeLengthHint() const { return m_iStartCodeLengthHint;}
  /**
   * @brief Setter for start code length hint
   */
  void setStartCodeLengthHint(int32_t iHint) { m_iStartCodeLengthHint = iHint; }
  /**
   * @brief Getter for NALU contains start code
   */
  bool doesNaluContainsStartCode() const { return m_bNaluContainsStartCode; }
  /**
   * @brief Setter for NALU contains start code 
   */
  void setNaluContainsStartCode(bool bNaluContainsStartCode) { m_bNaluContainsStartCode = bNaluContainsStartCode; }
  
private:  
  /// start time of media sample
  double m_dStartTime;
  /// Presentation time
  boost::posix_time::ptime m_tPresentation;
  /// Buffer to store media data
  Buffer m_data;
  /// marker bit that can be used to show special events in the stream such
  /// as I-Frames, RTCP synchronisation, etc
  bool m_bMarker;
  /// optional RtpTime which can be used to determine new AUs and determine the length of start codes
  uint32_t m_uiRtpTime;
  /// Optional decoding order number, -1 if not set
  int16_t m_iDecodingOrderNumber;
  /// Optional flow ID hint for MPRTP, -1 if not set
  int32_t m_iFlowIdHint;
  /// Optional start code hint for H.264 to write correct start codes
  int32_t m_iStartCodeLengthHint;
  /// to handle encoders that create NAL units with start code
  bool m_bNaluContainsStartCode;
};

} // media
} // rtp_plus_plus
