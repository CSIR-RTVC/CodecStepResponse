#pragma once
#include <rtp++/media/NalUnitMediaSource.h>
#include <rtp++/media/YuvMediaSource.h>

namespace rtp_plus_plus {
namespace test {

BOOST_AUTO_TEST_CASE(tc_test_YuvMediaSource)
{
  const uint32_t uiWidth = 352;
  const uint32_t uiHeight = 288;
  const uint32_t uiFrameSize = static_cast<uint32_t>(uiWidth * uiHeight * 1.5);
  const uint32_t uiFrameCount = 5;
  std::string sRawYuv;
  sRawYuv.resize(uiFrameSize * uiFrameCount, '0');
  std::istringstream istr(sRawYuv);
  media::YuvMediaSource yuvMediaSource(istr, uiWidth, uiHeight, false, 0);

  int iCount = 0;
  while (yuvMediaSource.isGood())
  {
    std::vector<media::MediaSample> frame = yuvMediaSource.getNextAccessUnit();
    if (!frame.empty())
    {
      ++iCount;
      BOOST_CHECK_EQUAL(frame.size(), 1);
    }
  }
  BOOST_CHECK_EQUAL(iCount, uiFrameCount);
}

BOOST_AUTO_TEST_CASE(tc_test_NalUnitMediaSource)
{
  media::NalUnitMediaSource naluMediaSource("../data/352x288p30_Akiyo.264", rfc6184::H264, false, 0);
  int iCount = 0;
  int iNalCount = 0;
  while (naluMediaSource.isGood())
  {
    std::vector<media::MediaSample> frame = naluMediaSource.getNextAccessUnit();
    if (!frame.empty())
    {
      ++iCount;
      iNalCount+= frame.size();
    }
  }
  VLOG(2) << "Total frames: " << iCount << " Total NALUs: " << iNalCount;
  BOOST_CHECK_EQUAL(iCount, 300);
}

} // test
} // rtp_plus_plus
