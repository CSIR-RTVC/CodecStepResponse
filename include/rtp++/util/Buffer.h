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
#include <stdexcept>
#include <boost/cstdint.hpp>
#include <boost/shared_array.hpp>

namespace rtp_plus_plus {

/**
 * @brief Wrapper class around boost::shared array
 */
class Buffer
{
public:
  typedef boost::shared_array<uint8_t> DataBuffer_t;
  /**
   * @brief Buffer Default constructor
   */
  explicit Buffer()
    :m_buffer( DataBuffer_t() ),
    m_uiSize( 0 ),
    m_uiPrebuffer( 0 ),
    m_uiPostbuffer( 0 )
  {

  }
  /**
   * @brief Buffer Constructor that takes ownership of the passed in buffer
   * @param ptr The buffer that will be managed by this class
   * @param size The size of the buffer to be managed
   */
  explicit Buffer(uint8_t* ptr, size_t size)
    :m_buffer( DataBuffer_t(ptr) ),
    m_uiSize(size),
    m_uiPrebuffer(0),
    m_uiPostbuffer(0)
  {
 
  }
  /**
   * @brief Buffer Constructor that takes ownership of the passed in buffer.
   * The buffer contains prebuffer space to write data ahead of the data
   * already written. This can be useful for prepending data after the
   * original data has already been written.
   *
   * @param ptr The buffer that will be managed by this class
   * @param size The size of the buffer to be managed
   */
  explicit Buffer(uint8_t* ptr, size_t size, size_t prebuffer, size_t postbuffer)
    :m_buffer( DataBuffer_t(ptr) ),
    m_uiSize(size),
    m_uiPrebuffer(prebuffer),
    m_uiPostbuffer(postbuffer)
  {
    if (size < prebuffer + postbuffer)
      throw std::runtime_error("Invalid parameters");
  }
  /**
   * @brief Destructor
   */
  ~Buffer()
  {

  }

  uint8_t& operator[](std::ptrdiff_t i) const
  {
    return m_buffer[i + m_uiPrebuffer];
  }

  DataBuffer_t& getBuffer() { return m_buffer; }
  size_t getSize() const { return m_uiSize - m_uiPrebuffer - m_uiPostbuffer; }
  size_t getTotalSize() const { return m_uiSize + m_uiPrebuffer + m_uiPostbuffer; }
  size_t getPrebufferSize() const { return m_uiPrebuffer; }
  size_t getPostbufferSize() const { return m_uiPostbuffer; }

  void setData(uint8_t* ptr, size_t size)
  {
    m_buffer.reset(ptr);
    m_uiSize = size;
    m_uiPrebuffer = 0;
    m_uiPostbuffer = 0;
  }

  void setData(uint8_t* ptr, size_t size, size_t prebuffer, size_t postbuffer)
  {
    if (size < prebuffer + postbuffer)
      throw std::runtime_error("Invalid parameters");
    m_buffer.reset(ptr);
    m_uiSize = size - prebuffer - postbuffer;
    m_uiPrebuffer = prebuffer;
    m_uiPostbuffer = postbuffer;
  }

  bool prependData(uint8_t* pData, size_t size)
  {
    if (size > m_uiPrebuffer) return false;
    memcpy(m_buffer.get() + m_uiPrebuffer - size, pData, size);
    m_uiPrebuffer -= size;
    return true;
  }

  /**
   * @brief consumePostBuffer Consumes the post buffer which can be accessed directly
   * @param size Size in bytes of post buffer that are to be consumed
   * @return true if the requested size could be consumed in the post buffer
   */
  bool consumePostBuffer(size_t size)
  {
    if (size > m_uiPostbuffer) return false;
    m_uiPostbuffer -= size;
    return true;
  }

  void reset()
  {
    m_buffer.reset();
    m_uiSize = 0;
    m_uiPrebuffer = 0;
    m_uiPostbuffer = 0;
  }

  const uint8_t* data() const
  {
    return m_buffer.get() + m_uiPrebuffer;
  }

  const uint8_t* getPrebufferData() const
  {
    return m_buffer.get();
  }

  std::string toStdString() const
  {
    return std::string( (char*)m_buffer.get() + m_uiPrebuffer, m_uiSize - m_uiPrebuffer - m_uiPostbuffer);
  }

  Buffer clone() const
  {
    if (m_uiSize > 0)
    {
      Buffer copy(new uint8_t[m_uiSize], m_uiSize, m_uiPrebuffer, m_uiPostbuffer);
      memcpy(&m_buffer[0], &copy.m_buffer[0], m_uiSize + m_uiPrebuffer + m_uiPostbuffer);
      return copy;
    }
    else
    {
      return Buffer();
    }
  }

private:
  DataBuffer_t m_buffer;
  size_t m_uiSize;
  size_t m_uiPrebuffer;
  size_t m_uiPostbuffer;
};

} // rtp_plus_plus
