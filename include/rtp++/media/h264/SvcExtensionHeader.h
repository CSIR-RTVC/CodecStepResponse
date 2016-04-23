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
#include <cstdint>
#include <iterator>
#include <boost/optional.hpp>
#include <rtp++/media/MediaSample.h>
#include <rtp++/media/h264/H264NalUnitTypes.h>
#include <rtp++/util/IBitStream.h>

namespace rtp_plus_plus {
namespace media {
namespace h264 {
namespace svc {

struct SvcExtensionHeader
{
  SvcExtensionHeader()
    :idr_flag(0),
     priority_id(0),
     no_inter_layer_pred_flag(0),
     dependency_id(0),
     quality_id(0),
     temporal_id(0),
     use_ref_base_pic_flag(0),
     discardable_flag(0),
     output_flag(0),
     reserved_three_2bits(0)
  {

  }

  uint32_t idr_flag;
  uint32_t priority_id;
  uint32_t no_inter_layer_pred_flag;
  uint32_t dependency_id;
  uint32_t quality_id;
  uint32_t temporal_id;
  uint32_t use_ref_base_pic_flag;
  uint32_t discardable_flag;
  uint32_t output_flag;
  uint32_t reserved_three_2bits;
};

/**
  * Utility method to extract NAL unit type from media sample
  */
static boost::optional<SvcExtensionHeader> extractSvcExtensionHeader(const MediaSample& mediaSample)
{
  media::h264::NalUnitType eType = media::h264::getNalUnitType(mediaSample);

  if (eType == NUT_PREFIX_NAL_UNIT || eType == NUT_CODED_SLICE_EXT || eType == NUT_RESERVED_21)
  {
    if (mediaSample.getPayloadSize() < 4)
    return boost::optional<SvcExtensionHeader>();

  IBitStream in(mediaSample.getDataBuffer());
  // skip NAL header
  in.skipBytes(1);

#define USING_HHI_SVC_ENCODER
#ifdef USING_HHI_SVC_ENCODER
  SvcExtensionHeader svcHeader;
  if (eType == NUT_RESERVED_21)
    svcHeader.idr_flag = 1;

  // reserved zero 2 bits
  in.skipBits(2);
  in.read(svcHeader.priority_id, 6);
  in.read(svcHeader.temporal_id, 3);
  in.read(svcHeader.dependency_id, 3);
  in.read(svcHeader.quality_id, 2);
  // reserved zero 1 bit
  in.skipBits(1);
  // layer base
  in.skipBits(1);
  // use base prediction
  in.skipBits(1);
  in.read(svcHeader.discardable_flag, 1);
  // fragmented
  in.skipBits(1);
  // last fragment
  in.skipBits(1);
  // fragment order all
  in.read(svcHeader.reserved_three_2bits, 2);
  return boost::optional<SvcExtensionHeader>(svcHeader);
#else
  // latest version of standard
  uint8_t svcExtensionBit;
  bool bRes = in.read(svcExtensionBit, 1);
  assert(bRes);
  if (!svcExtensionBit)
    return boost::optional<SvcExtensionHeader>();
  SvcExtensionHeader svcHeader;
  in.read(svcHeader.idr_flag, 1);
  in.read(svcHeader.priority_id, 6);
  in.read(svcHeader.no_inter_layer_pred_flag, 1);
  in.read(svcHeader.dependency_id, 3);
  in.read(svcHeader.quality_id, 4);
  in.read(svcHeader.temporal_id, 3);
  in.read(svcHeader.use_ref_base_pic_flag, 1);
  in.read(svcHeader.discardable_flag, 1);
  in.read(svcHeader.output_flag, 1);
  in.read(svcHeader.reserved_three_2bits, 2);
  return boost::optional<SvcExtensionHeader>(svcHeader);
#endif

  }
  return boost::optional<SvcExtensionHeader>();
}

static void splitMediaSamplesIntoBaseAndEnhancementLayers(const std::vector<MediaSample>& vMediaSamples,
                                                          std::vector<MediaSample>& vBaseLayerSamples,
                                                          std::vector<MediaSample>& vEnhancementLayerSamples)
{
#if 1
  // for now hack it by searching for the first media::h264::NUT_CODED_SLICE_EXT
  auto it = std::find_if(vMediaSamples.begin(), vMediaSamples.end(), [](const MediaSample& mediaSample)
  {
    return (media::h264::getNalUnitType(mediaSample) == media::h264::NUT_CODED_SLICE_EXT ||
            media::h264::getNalUnitType(mediaSample) == media::h264::NUT_RESERVED_21);
  });

  std::copy(vMediaSamples.begin(), it, std::back_inserter(vBaseLayerSamples));
  if (it != vMediaSamples.end())
    std::copy(it, vMediaSamples.end(), std::back_inserter(vEnhancementLayerSamples));
#endif
}


} // svc
} // h264
} // media
} // rtp_plus_plus
