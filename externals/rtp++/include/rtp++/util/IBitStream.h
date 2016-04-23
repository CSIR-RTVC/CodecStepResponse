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
#include "Buffer.h"

namespace rtp_plus_plus {

/**
 * @brief Class to read from a bitstream
 */
class IBitStream
{
  friend class OBitStream;

public:
  IBitStream(Buffer buffer)
    :m_buffer(buffer),
    m_uiBitsRemaining(buffer.getSize() << 3),
    m_uiBitsInCurrentByte(8),
    m_uiCurrentBytePos(0)
  {

  }

  uint32_t getBitsRemaining() const { return m_uiBitsRemaining; }
  uint32_t getBytesRemaining() const { return (m_uiBitsRemaining >> 3); }

  bool read(uint64_t& uiValue, uint32_t uiBits)
  {
    if (uiBits > m_uiBitsRemaining)
    {
      return false;
    }

    uiValue = 0;
    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining > 0)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      uiValue <<= uiBitsToReadInCurrentByte;
      // bits to shift by
      uint32_t uiBitsToShiftBy = m_uiBitsInCurrentByte - uiBitsToReadInCurrentByte;
      uint8_t uiMask = (2 << (uiBitsToReadInCurrentByte - 1)) - 1;
      uiValue |= ((m_buffer[m_uiCurrentBytePos] >> uiBitsToShiftBy) & uiMask);

      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    // update total
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  bool read(uint32_t& uiValue, uint32_t uiBits)
  {
    if (uiBits > m_uiBitsRemaining)
    {
      return false;
    }

    uiValue = 0;
    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining > 0)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      uiValue <<= uiBitsToReadInCurrentByte;
      // bits to shift by
      uint32_t uiBitsToShiftBy = m_uiBitsInCurrentByte - uiBitsToReadInCurrentByte;
      uint8_t uiMask = (2 << (uiBitsToReadInCurrentByte - 1)) - 1;
      uiValue |= ((m_buffer[m_uiCurrentBytePos] >> uiBitsToShiftBy) & uiMask);

      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    // update total
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  // This method can only read 8 bits at a time
  bool read(uint8_t& uiValue, uint32_t uiBits)
  {
    if (uiBits > 8 || (uiBits > m_uiBitsRemaining))
    {
      return false;
    }

    uiValue = 0;
    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining > 0)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      uiValue <<= uiBitsToReadInCurrentByte;
      // bits to shift by
      uint32_t uiBitsToShiftBy = m_uiBitsInCurrentByte - uiBitsToReadInCurrentByte;
      uint8_t uiMask = (2 << (uiBitsToReadInCurrentByte - 1)) - 1;
      uiValue |= ((m_buffer[m_uiCurrentBytePos] >> uiBitsToShiftBy) & uiMask);

      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    // update total
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  // This method can only read 16 bits at a time
  bool read(uint16_t& uiValue, uint32_t uiBits)
  {
    if (uiBits > 16 || (uiBits > m_uiBitsRemaining))
    {
      return false;
    }

    uiValue = 0;
    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining > 0)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      uiValue <<= uiBitsToReadInCurrentByte;
      // bits to shift by
      uint32_t uiBitsToShiftBy = m_uiBitsInCurrentByte - uiBitsToReadInCurrentByte;
      uint8_t uiMask = (2 << (uiBitsToReadInCurrentByte - 1)) - 1;
      uiValue |= ((m_buffer[m_uiCurrentBytePos] >> uiBitsToShiftBy) & uiMask);

      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    // update total
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  // this method can only be called on byte boundaries
  bool readBytes(uint8_t*& rDestination, uint32_t uiBytes)
  {
    uint32_t uiBits = uiBytes << 3;
    if ((m_uiBitsInCurrentByte != 8) || 
        (uiBits > m_uiBitsRemaining)
       )
    {
      return false;
    }

    memcpy(rDestination, &m_buffer[m_uiCurrentBytePos], uiBytes);
    // update total
    m_uiBitsRemaining -= uiBits;
    m_uiCurrentBytePos += uiBytes;
    return true;
  }

  bool skipBits(uint32_t uiBits)
  {
    if (uiBits > m_uiBitsRemaining)
    {
      return false;
    }

    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  bool skipBytes(uint32_t uiBytes)
  {
    uint32_t uiBits = uiBytes << 3;
    if ((m_uiBitsInCurrentByte != 8) || 
      (uiBits > m_uiBitsRemaining)
      )
    {
      return false;
    }

    m_uiBitsRemaining -= uiBits;
    m_uiCurrentBytePos += uiBytes;
    return true;
  }
   
  uint8_t peekAtCurrentByte() const
  {
    return m_buffer[m_uiCurrentBytePos];
  }
private:
  Buffer m_buffer;
  uint32_t m_uiBitsRemaining;
  uint32_t m_uiBitsInCurrentByte;
  uint32_t m_uiCurrentBytePos;
};

} // rtp_plus_plus
