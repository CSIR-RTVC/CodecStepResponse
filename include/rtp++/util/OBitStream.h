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
#include <cassert>
#include <cstring>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_array.hpp>
#include "Buffer.h"
#include "IBitStream.h"

#define DEFAULT_BUFFER_SIZE 1024
#define PRE_BUFFER_SIZE 0

/// DEBUG_OBITSTREAM for outputting debug info

namespace rtp_plus_plus {

/**
 * @brief Class to write to a bitstream
 */
class OBitStream
{
public:
  /**
   * @brief OBitStream
   * @param uiSize
   * @param bConservative
   * @param uiPreBufferSize
   */
  explicit OBitStream(const uint32_t uiSize = DEFAULT_BUFFER_SIZE, const uint32_t uiPreBufferSize = PRE_BUFFER_SIZE, bool bConservative = true)
    :m_uiBufferSize(uiSize),
    m_buffer(new uint8_t[uiSize], uiSize, uiPreBufferSize, 0), // allocating 4 bytes of prebuffer
    m_uiBitsLeft(8),
    m_uiCurrentBytePos(0),
    m_bConservative(bConservative)
  {
    memset(m_buffer.getBuffer().get(), 0, m_uiBufferSize);
  }
  /**
   * @brief OBitStream
   * @param buffer
   * @param bConservative
   */
  explicit OBitStream(Buffer buffer, bool bConservative = true)
    :m_uiBufferSize(buffer.getSize()),
    m_buffer(buffer),
    m_uiBitsLeft(8),
    m_uiCurrentBytePos(0),
    m_bConservative(bConservative)
  {
    memset(m_buffer.getBuffer().get(), 0, m_uiBufferSize);
  }
  /**
   * @brief reset resets the write pointers inside the class
   * The allocated memory is not touched or modified.
   * This is useful to avoid memory allocation.
   */
  void reset()
  {
    m_uiBitsLeft = 8;
    m_uiCurrentBytePos = 0;
    memset(m_buffer.getBuffer().get(), 0, m_uiBufferSize);
  }
  /**
   * @brief write8Bits
   * @param uiValue
   */
  void write8Bits(uint8_t uiValue)
  {
    if (totalBitsLeft() < 8)
    {
      increaseBufferSize(m_uiBufferSize << 1);
    }
    // check if we're writing on byte boundary
    if (m_uiBitsLeft == 8)
    {
      m_buffer[m_uiCurrentBytePos++] = uiValue;
    }
    else
    {
      write(uiValue, 8);
    }
  }
  /**
   * @brief write
   * @param uiValue
   * @param uiBits
   */
  void write(uint32_t uiValue, uint32_t uiBits)
  {
    // check if enough memory has been allocated
    if ( totalBitsLeft() < uiBits )
    {
      // reallocate more than enough memory:
      uint32_t uiBytes = uiBits >> 3;
      uint32_t uiNewSize = std::max(m_uiBufferSize << 1, (m_uiBufferSize + uiBytes) << 1 );
      increaseBufferSize(uiNewSize);
    }

    // check if we can do a fast copy: on byte boundaries where uiBits is a multiple of 8
    // is % maybe faster?
    if (m_uiBitsLeft == 8 && (uiBits == 8 ||  uiBits == 16 || uiBits == 24 || uiBits == 32))
    {
      // TODO: check endianness of machine
      bool bLittleEndian = true;
      if (bLittleEndian)
      {
        switch (uiBits)
        {
          case 32:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 32;
#endif
            m_buffer[m_uiCurrentBytePos++] = ((uiValue >> 24) & 0xff); 
          }
          case 24:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 24;
#endif
            m_buffer[m_uiCurrentBytePos++] = ((uiValue >> 16) & 0xff);
          }
          case 16:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 16;
#endif
            m_buffer[m_uiCurrentBytePos++] = ((uiValue >> 8) & 0xff);
          }
          case 8:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 8;
#endif
            m_buffer[m_uiCurrentBytePos++] = uiValue;
            break;
          }
        }
      }
      else
      {
        //memcpy(&m_buffers[m_uiCurrentBufferPos][m_uiCurrentBytePos],  );
        assert(false);
      }
    }
    else
    {
      // calculate mask: (2^bits - 1) is a mask for uiBits number of bits
      // can only write a maximum of m_uiBitsLeft at a time
      uint32_t uiBitsLeftToWrite = uiBits;
      uint32_t uiValueCopy = uiValue;
      while (uiBitsLeftToWrite)
      {
        // write max of m_uiBitsLeft bits
        uint8_t uiBitsToWrite = std::min(uiBitsLeftToWrite, m_uiBitsLeft);
        // calculate mask for bits
        uint8_t uiMask = (2 << (uiBitsToWrite - 1)) -1; 
        // calculate bits to written in next write if current byte has insufficient space
        uint8_t uiOverflowBits = (uiBitsLeftToWrite > m_uiBitsLeft)? uiBitsLeftToWrite - m_uiBitsLeft : 0;
        // align value to the right
        uint8_t uiOffsetByteValue = ((uiValueCopy >> uiOverflowBits) & uiMask);
        // now shift value to correct position within target byte
        uiOffsetByteValue = uiOffsetByteValue << (m_uiBitsLeft - uiBitsToWrite);
        // combine value in buffer with new written bits
        m_buffer[m_uiCurrentBytePos] = m_buffer[m_uiCurrentBytePos] | uiOffsetByteValue;
        
        uiBitsLeftToWrite -= uiBitsToWrite;
        m_uiBitsLeft -= uiBitsToWrite;
        if (m_uiBitsLeft == 0)
        {
          m_uiBitsLeft = 8;
          ++m_uiCurrentBytePos;
        }
        // subtract written bits from value 
        uiValueCopy = uiValueCopy - (uiOffsetByteValue << uiOverflowBits);
      }

#if 0
      //
      //
      uint8_t uiBitsToUse = std::min(uiBits, m_uiBitsLeft);
      uint8_t uiMask = (2 << (uiBits - 1)); 
      uint8_t uiMaskedValue = uiValue & uiMask;
      LOG_INFO(rLogger, LOG_FUNCTION, "Mask: %1% Masked Val: %2%", (int)uiMask, (int)uiMaskedValue);
      // current value at posi
      // check if we have enough bits for all value
      if (uiBits <= m_uiBitsLeft)
      {
        LOG_INFO(rLogger, LOG_FUNCTION, "Before: %1% After shift: %2%", (int)m_buffer[m_uiCurrentBytePos] , (int)(m_buffer[m_uiCurrentBytePos] << uiBits) );
     
        m_buffer[m_uiCurrentBytePos] = (m_buffer[m_uiCurrentBytePos] << uiBits) | uiMaskedValue;
        m_uiBitsLeft -= uiBits;
        LOG_INFO(rLogger, LOG_FUNCTION, "New: %1% Bits left: %2%", (int)m_buffer[m_uiCurrentBytePos] , m_uiBitsLeft);
        if (m_uiBitsLeft == 0)
        {
          m_uiBitsLeft = 8;
          ++m_uiCurrentBytePos;
        }
      }
      else
      {
        uint32_t uiRemainderBits = uiBits - m_uiBitsLeft;
        // fill up current byte
        m_buffer[m_uiCurrentBytePos]  = (m_buffer[m_uiCurrentBytePos]  << m_uiBitsLeft) | (uiMaskedValue >> uiRemainderBits);
        m_uiBitsLeft = 8;
        ++m_uiCurrentBytePos;
        // write remainder
        uiMask = (2 << uiRemainderBits) - 1;
        m_buffer[m_uiCurrentBytePos]  = uiMaskedValue & uiMask; // the new mask will get rid of the other bits
        m_uiBitsLeft = 8 - uiRemainderBits;
      }
#endif
    }
  }
  /**
   * @brief writeBytes
   * @param rSrc
   * @param uiBytes
   * @return
   * this method can only be called on byte boundaries
   */
  bool writeBytes(const uint8_t*& rSrc, uint32_t uiBytes)
  {
    if ((m_uiBitsLeft != 8) || // check byte boundary
        ((m_uiBufferSize - m_uiCurrentBytePos) < uiBytes) // check buffer size
       ) 
         return false;
    memcpy(&m_buffer[m_uiCurrentBytePos], rSrc, uiBytes);
    m_uiCurrentBytePos += uiBytes;
    return true;
  }
  /**
   * @brief write
   * @param in
   * @return
   * this method writes all bytes remaining in the IBitStream to the output stream
   * TODO: make this method handle non-byte boundary data
   */
  bool write(IBitStream& in)
  {
      if (m_uiBitsLeft != 8) return false;
      // get remaining bytes
      if (in.m_uiBitsRemaining % 8 != 0) return false;

      // this code only works if all the pointers are byte aligned!!!
      if (m_uiCurrentBytePos >= m_uiBufferSize)
      {
          LOG(WARNING) << "WARN: Byte pos: " << m_uiCurrentBytePos << " Size: " << m_uiBufferSize;
      }
      assert (m_uiCurrentBytePos <= m_uiBufferSize);
      uint32_t uiBytesLeft = m_uiBufferSize - m_uiCurrentBytePos;

      uint32_t uiBytesToCopy = in.getBytesRemaining();
      if (uiBytesToCopy > uiBytesLeft)
      {
        // conservative for now:
        uint32_t uiNewSize = m_bConservative ? m_uiCurrentBytePos + uiBytesToCopy : m_uiBufferSize * 2;
        increaseBufferSize(uiNewSize);
      }
      // increase buffer size if necessary
      uint8_t* pDestination = const_cast<uint8_t*>(m_buffer.data()) + m_uiCurrentBytePos;
      // bool bRes = in.readBytes(&m_buffer[m_uiCurrentBytePos], uiBytesToCopy);
      bool bRes = in.readBytes(pDestination, uiBytesToCopy);
      assert (bRes);
      m_uiCurrentBytePos += uiBytesToCopy;
      return bRes;
  }
  /**
   * @brief write
   * @param in
   * @param uiBytesToCopy
   * @return
   * this method writes all bytes remaining in the IBitStream to the output stream
   * TODO: make this method handle non-byte boundary data
   */
  bool write(IBitStream& in, uint32_t uiBytesToCopy)
  {
#if 0
      VLOG(5) << "bits left: " << m_uiBitsLeft << " bits remaining: " << in.m_uiBitsRemaining << " Bytes: " << in.getBytesRemaining() << " To copy: " << uiBytesToCopy;
#endif
      if (m_uiBitsLeft != 8) return false;
      // get remaining bytes
      if (in.m_uiBitsRemaining % 8 != 0) return false;
      if (in.getBytesRemaining() < uiBytesToCopy) return false;

      // this code only works if all the pointers are byte aligned!!!
      if (m_uiCurrentBytePos >= m_uiBufferSize)
      {
          LOG(WARNING) << "WARN: Byte pos: " << m_uiCurrentBytePos << " Size: " << m_uiBufferSize;
      }
      assert (m_uiCurrentBytePos <= m_uiBufferSize);
      uint32_t uiBytesLeft = m_uiBufferSize - m_uiCurrentBytePos;

      if (uiBytesToCopy > uiBytesLeft)
      {
        // conservative for now:
        uint32_t uiNewSize = m_uiCurrentBytePos + uiBytesToCopy;
        increaseBufferSize(uiNewSize);
      }
      // increase buffer size if necessary
      uint8_t* pDestination = const_cast<uint8_t*>(m_buffer.data()) + m_uiCurrentBytePos;
      // bool bRes = in.readBytes(&m_buffer[m_uiCurrentBytePos], uiBytesToCopy);
      bool bRes = in.readBytes(pDestination, uiBytesToCopy);
      assert (bRes);
      m_uiCurrentBytePos += uiBytesToCopy;
      return bRes;
  }
  /**
   * @brief bytesUsed
   * @return
   */
  uint32_t bytesUsed() const 
  {
    return m_uiCurrentBytePos + (m_uiBitsLeft == 8 ? 0 : 1);
  }
  /**
   * @brief totalBitsLeft
   * @return
   */
  uint32_t totalBitsLeft() const
  {
    return  m_uiBitsLeft + 8 * (m_uiBufferSize - m_uiCurrentBytePos - 1);
  }
  /**
   * @brief str
   * @return
   */
  Buffer str() const
  {
    // copy all bits to a buffer
    Buffer buffer;
    // first calculate size of buffer required
    uint32_t uiSize = bytesUsed(); 
    if (uiSize)
    {
      buffer.setData(new uint8_t[uiSize], uiSize);
      memcpy(&buffer[0], &m_buffer[0], uiSize); 
    }
    return buffer;
  }
  /**
   * @brief data
   * @return
   */
  Buffer data() const
  {
    return str();
  }

private:
  void increaseBufferSize(uint32_t uiNewSize)
  {
    // respect old pre buffer
    uint32_t uiOldPreBuffer = m_buffer.getPrebufferSize();
    uint32_t uiOldPostBuffer = m_buffer.getPostbufferSize();
    Buffer buffer = Buffer(new uint8_t[uiNewSize], uiNewSize, uiOldPreBuffer, uiOldPostBuffer);
    memset(&buffer[0], 0, uiNewSize);
    memcpy(&buffer[0], &m_buffer[0], m_uiBufferSize );
    m_buffer = buffer;
    m_uiBufferSize = uiNewSize;
  }
  void doWrite(uint32_t uiValue, uint32_t uiBits)
  {
    // calculate mask: (2^bits - 1) is a mask for uiBits number of bits
    uint8_t uiMask = (2 << (uiBits - 1)) - 1;
    uint8_t uiMaskedValue = uiValue & uiMask;
#if 0
    LOG_INFO(rLogger, LOG_FUNCTION, "Mask: %1% Masked Val: %2%", (int)uiMask, (int)uiMaskedValue);
#endif
    // current value at posi
    // check if we have enough bits for all value
    if (uiBits <= m_uiBitsLeft)
    {
#if 0
      LOG_INFO(rLogger, LOG_FUNCTION, "Before: %1% After shift: %2%", (int)m_buffer[m_uiCurrentBytePos] , (int)(m_buffer[m_uiCurrentBytePos] << uiBits) );
#endif

      m_buffer[m_uiCurrentBytePos] = (m_buffer[m_uiCurrentBytePos] << uiBits) | uiMaskedValue;
      m_uiBitsLeft -= uiBits;
#if 0
      LOG_INFO(rLogger, LOG_FUNCTION, "New: %1% Bits left: %2%", (int)m_buffer[m_uiCurrentBytePos] , m_uiBitsLeft);
#endif
      if (m_uiBitsLeft == 0)
      {
        m_uiBitsLeft = 8;
        ++m_uiCurrentBytePos;
      }
    }
    else
    {
      uint32_t uiRemainderBits = uiBits - m_uiBitsLeft;
      // fill up current byte
      m_buffer[m_uiCurrentBytePos]  = (m_buffer[m_uiCurrentBytePos]  << m_uiBitsLeft) & (uiMaskedValue >> uiRemainderBits);
      m_uiBitsLeft = 8;
      ++m_uiCurrentBytePos;
      // write remainder
      uiMask = (2 << uiRemainderBits) - 1;
      m_buffer[m_uiCurrentBytePos]  = uiMaskedValue & uiMask; // the new mask will get rid of the other bits
      m_uiBitsLeft = 8 - uiRemainderBits;
    }
  }

  uint32_t m_uiBufferSize;
  Buffer m_buffer;

  ///< Bits left in current byte
  uint32_t m_uiBitsLeft;

  ///< Current position in the buffer  
  uint32_t m_uiCurrentBytePos;

  bool m_bConservative;
};

} // rtp_plus_plus
