#include "stdafx.h"
#include <chrono>
#include <sstream>
#include <vector>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <rtp++/media/IVideoCodecTransform.h>
#include <rtp++/media/YuvMediaSource.h>
#include <rtp++/media/h264/H264AnnexBStreamWriter.h>
#include <rtp++/media/h265/H265AnnexBStreamWriter.h>
#include <rtp++/util/Conversion.h>
#include <rtp++/util/StringTokenizer.h>
#include <OpenH264Codec/OpenH264Codec.h>
#include <X264Codec/X264Codec.h>
#include <X265Codec/X265Codec.h>

using namespace rtp_plus_plus;
using namespace rtp_plus_plus::media;
using namespace boost::program_options;

void validateYuvInput(const std::string& sYuvFile)
{
  if (!boost::filesystem::exists(sYuvFile))
  {
    LOG(ERROR) << "YUV input file " << sYuvFile << " does not exist";
    throw validation_error(validation_error::invalid_option_value);
  }
}

void validateWidth(const uint32_t uiWidth)
{
  if (uiWidth == 0)
  {
    LOG(ERROR) << "Invalid width: " << uiWidth;
    throw validation_error(validation_error::invalid_option_value);
  }
}

void validateHeight(const uint32_t uiHeight)
{
  if (uiHeight == 0)
  {
    LOG(ERROR) << "Invalid width: " << uiHeight;
    throw validation_error(validation_error::invalid_option_value);
  }
}

void validateFps(const double dFps)
{
  if (dFps == 0.0)
  {
    LOG(ERROR) << "Invalid FPS: " << dFps;
    throw validation_error(validation_error::invalid_option_value);
  }
}

struct RateDescriptor
{
  RateDescriptor()
    :Rate(0.0),
      Duration(0.0)
  {

  }
  RateDescriptor(double dRate, double dDuration)
    :Rate(dRate),
      Duration(dDuration)
  {

  }
  // rate in kbps OR bpp
  double Rate;
  // duration in seconds OR number of frames
  double Duration;
};

/**
 * @brief parseRateDescriptor parses a rate descriptor string which has the form:
 * rate_descriptor = <<rate>>[:<<duration>>[|<<rate_descriptor>>]]
 * @param sRatesDescriptor The rate descriptor to be parsed
 * @return vector of rates
 */
std::vector<RateDescriptor> parseRateDescriptor(const std::string& sRatesDescriptor)
{
  std::vector<RateDescriptor> rates;
  std::vector<std::string> vSegments = StringTokenizer::tokenize(sRatesDescriptor, ",", true, true);
  // only the last segment can not have a duration
  for (const std::string& sSegment : vSegments)
  {
    std::vector<std::string> vSegmentInfo = StringTokenizer::tokenize(sSegment, ":", true, true);
    bool bDummy;
    if (vSegmentInfo.size() == 2)
    {
      double dRate = convert<double>(vSegmentInfo[0], bDummy);
      assert(bDummy);
      double dDuration = convert<double>(vSegmentInfo[1], bDummy);
      assert(bDummy);
      rates.push_back(RateDescriptor(dRate, dDuration));
    }
    else if (vSegmentInfo.size() == 1)
    {
      double dRate = convert<double>(vSegmentInfo[0], bDummy);
      assert(bDummy);
      rates.push_back(RateDescriptor(dRate, -1.0));
    }
    else
    {
      return std::vector<RateDescriptor>();
    }
  }
  return rates;
}

void validateRateDescriptor(const std::string& sRateDescriptor)
{
  std::vector<RateDescriptor> rates = parseRateDescriptor(sRateDescriptor);
  if (rates.empty())
  {
    LOG(ERROR) << "Invalid rate descriptor: " << sRateDescriptor;
    throw validation_error(validation_error::invalid_option_value);
  }
}

void validateVideoCodec(const std::string& sVideoCodec)
{
  std::vector<std::string> codecs = {"H264", "H264-SVC", "H265"};
  auto it = std::find_if(codecs.begin(), codecs.end(), [sVideoCodec](const std::string& sCodec)
  {
    return boost::to_upper_copy(sVideoCodec) == sCodec;
  });
  if (it == codecs.end())
  {
    LOG(ERROR) << "Invalid video codec: " << sVideoCodec;
    throw validation_error(validation_error::invalid_option_value);
  }
}

void validateVideoCodecImpl(const std::string& sVideoCodecImpl)
{
  // TODO: instead of using hard-coded string, load all DLLs in directory meeting requirements
  std::vector<std::string> codecs = {"openh264", "x264", "x265", "vpp"};
  auto it = std::find_if(codecs.begin(), codecs.end(), [sVideoCodecImpl](const std::string& sCodecImpl)
  { return boost::to_upper_copy(sVideoCodecImpl) == boost::to_upper_copy(sCodecImpl);});
  if (it == codecs.end())
  {
    LOG(ERROR) << "Invalid video codec impl: " << sVideoCodecImpl;
    throw validation_error(validation_error::invalid_option_value);
  }
}

std::unique_ptr<IVideoCodecTransform> createAndInitialiseCodec(const std::string& sVideoCodec, const std::string& sVideoCodecImpl, uint32_t uiWidth, uint32_t uiHeight, double dFps, const std::vector<std::string>& videoCodecParams, uint32_t uiInitialBitrateKbps)
{
  std::unique_ptr<IVideoCodecTransform> pCodec;
  if (sVideoCodec == "H264")
  {
    if (sVideoCodecImpl == "OPENH264")
    {
      pCodec = std::unique_ptr<IVideoCodecTransform>(new OpenH264Codec());
    }
    else if (sVideoCodecImpl == "X264")
    {
      pCodec = std::unique_ptr<IVideoCodecTransform>(new X264Codec());
    }
  }
  else if (sVideoCodec == "H265")
  {
#ifdef ENABLE_X265
    if (sVideoCodecImpl == "X265")
    {
      pCodec = std::unique_ptr<IVideoCodecTransform>(new X265Codec());
    }
#endif
  }

  boost::system::error_code ec;
  if (pCodec)
  {
    MediaTypeDescriptor mediaIn(MediaTypeDescriptor::MT_VIDEO, MediaTypeDescriptor::MST_YUV_420P, uiWidth, uiHeight, dFps);
    ec = pCodec->setInputType(mediaIn);
    if (ec)
    {
      LOG(ERROR) << "Error initialising transform!: " << ec.message();
      // TODO: should exit app here
    }

    std::map<std::string, std::string> params;
    for (auto& item : videoCodecParams)
    {
      auto pair = StringTokenizer::tokenize(item, "=", true, true);
      if (pair.size() == 1)
      {
        params[pair[0]] = "";
      }
      else if (pair.size() == 2)
      {
        params[pair[0]] = pair[1];
      }
      else
      {
        LOG(WARNING) << "Invalid parameter: " << item;
      }
    }

    for (auto param : params)
    {
      VLOG(2) << "Calling configure: Name: " << param.first << " value: " << param.second;
      ec = pCodec->configure(param.first, param.second);
      if (ec)
      {
        LOG(WARNING) << "Failed to set codec parameter: " << param.first << " value: " << param.second;
      }
    }
  }

  if (pCodec)
  {
    VLOG(2) << "Setting initial bitrate to " << uiInitialBitrateKbps << " kbps";
    pCodec->setBitrate(uiInitialBitrateKbps);
    ec = pCodec->initialise();
    if (ec)
    {
      LOG(ERROR) << "Failed to initialise codec";
      return std::unique_ptr<IVideoCodecTransform>();
    }
  }

  return pCodec;
}

std::unique_ptr<MediaSink> createMediaSink(const std::string& sVideoCodec, const std::string& sOutputBaseName)
{
  std::unique_ptr<MediaSink> pMediaSink;
  std::ostringstream out;
  out << sOutputBaseName;

  if ( sVideoCodec == "H264" )
  {
    out << ".264";
    pMediaSink = std::unique_ptr<MediaSink>(new h264::H264AnnexBStreamWriter(out.str(), false,  true));
  }
  else if ( sVideoCodec == "H265" )
  {
    out << ".265";
    pMediaSink = std::unique_ptr<MediaSink>(new MediaSink(out.str()));
  }
  return pMediaSink;
}

enum RateMode
{
  RATE_MODE_KBPS = 0,
  RATE_MODE_BPP = 1
};

enum SwitchMode
{
  SWITCH_MODE_FRAME = 0,
  SWITCH_MODE_TIME = 1
};

int main(int argc, char** argv)
{
  // call any code here that needs to be called on application startup
  google::InitGoogleLogging(argv[0]);
#ifndef WIN32
  google::InstallFailureSignalHandler();
#endif

  try
  {
    std::string sYuvFile;
    std::string sOutput;
    uint32_t uiWidth = 0;
    uint32_t uiHeight = 0;
    double dFps = 0.0;
    bool bRepeat = false;
    uint32_t uiLoopCount = 1;
    std::string sLogfile, sLogDir;
    std::string sVideoCodec, sVideoCodecImpl;
    std::vector<std::string> videoCodecParams;
    uint32_t uiRateMode;
    uint32_t uiSwitchMode;
    std::string sRateDescriptor;
    options_description cmdline_options;
    cmdline_options.add_options()
        ("help,?", "produce help message")
        ("log,l", value<std::string>(&sLogfile)->default_value((boost::filesystem::path(argv[0]).leaf().string()), "Output log file."))
        ("log-dir,L", value<std::string>(&sLogDir)->default_value("."), "Output log dir.")
        ("input,i", value<std::string>(&sYuvFile)->required()->notifier(validateYuvInput), "YUV input file")
        ("output,o", value<std::string>(&sOutput)->required(), "Output file base name")
        ("width,w", value<uint32_t>(&uiWidth)->required()->notifier(validateWidth), "Width")
        ("height,h", value<uint32_t>(&uiHeight)->required()->notifier(validateHeight), "Height")
        ("fps,f", value<double>(&dFps)->required()->notifier(validateFps), "FPS")
        ("repeat,r", bool_switch(&bRepeat)->default_value(false), "Repeat source on eof.")
        ("repeat-count,c", value<uint32_t>(&uiLoopCount)->default_value(1), "Number of repetitions. 0 = infinite.")
        ("video-codec", value<std::string>(&sVideoCodec)->required()->notifier(validateVideoCodec), "Codec: [h264,h265]")
        ("vc-impl", value<std::string>(&sVideoCodecImpl)->required()->notifier(validateVideoCodecImpl), "Codec: [openh264,x264,x265,vpp]")
        ("vc-param", value<std::vector<std::string>>(&videoCodecParams), "Video codec parameters")
        ("rate-mode", value<uint32_t>(&uiRateMode)->default_value(0), "Rate mode. 0=kbps,1=bpp.")
        ("switch-mode", value<uint32_t>(&uiSwitchMode)->default_value(0), "Switch mode. 0=frame,1=time(s).")
        ("rate-descriptor", value<std::string>(&sRateDescriptor)->required()->notifier(validateRateDescriptor), "Rate descriptor format: <rate>[:<duration>[_<rate_descriptor>]]")
        ;

    variables_map vm;
    store(command_line_parser(argc, argv).
              options(cmdline_options).run(), vm);

    // deal with help option before mandatory checks
    if (vm.count("help"))
    {
      std::ostringstream ostr;
      ostr << cmdline_options;
      LOG(ERROR) << ostr.str();
      return 1;
    }

    notify(vm);

    std::ostringstream logPath; logPath << sLogDir << "/" << sLogfile;
    // update the log file: we want to be able to parse this file
    google::SetLogDestination(google::GLOG_INFO, (logPath.str() + ".INFO").c_str());
    google::SetLogDestination(google::GLOG_WARNING, (logPath.str() + ".WARNING").c_str());
    google::SetLogDestination(google::GLOG_ERROR, (logPath.str() + ".ERROR").c_str());

    // convert to kbps as this is understood by encoders
    // convert switch point to frame
    std::vector<RateDescriptor> rates = parseRateDescriptor(sRateDescriptor);
    std::vector<double> vKbps;
    std::vector<uint32_t> vSwitchFrames = {0}; // first switch to bitrate is at the beginning
    uint32_t uiPreviousSwitchFrame = 0;
    double dFrameDuration = 1.0/dFps;
    for (RateDescriptor rate : rates)
    {
      switch (uiRateMode)
      {
        case RATE_MODE_KBPS:
        {
          vKbps.push_back(rate.Rate);
          break;
        }
        case RATE_MODE_BPP:
        {
          double dKbps = rate.Rate * uiWidth * uiHeight * dFps;
          vKbps.push_back(dKbps);
          break;
        }
      }
      switch (uiSwitchMode)
      {
        case SWITCH_MODE_FRAME:
        {
          uint32_t uiNextSwitch = uiPreviousSwitchFrame + rate.Duration;
          vSwitchFrames.push_back(uiNextSwitch);
          uiPreviousSwitchFrame = uiNextSwitch;
          break;
        }
        case SWITCH_MODE_TIME:
        {
          if (rate.Duration != -1.0)
          {
            uint32_t uiNextSwitch = uiPreviousSwitchFrame + (rate.Duration/dFrameDuration);
            vSwitchFrames.push_back(uiNextSwitch);
            uiPreviousSwitchFrame = uiNextSwitch;
          }
          break;
        }
      }
    }

    boost::to_upper(sVideoCodec);
    boost::to_upper(sVideoCodecImpl);

    std::unique_ptr<IVideoCodecTransform> pCodec = createAndInitialiseCodec(sVideoCodec, sVideoCodecImpl, uiWidth, uiHeight, dFps, videoCodecParams, vKbps.at(0));
    if (!pCodec)
    {
      LOG(ERROR) << "Failed to create and initialise codec.";
      return -1;
    }

    std::unique_ptr<MediaSink> pMediaSink = createMediaSink(sVideoCodec, sOutput);
    if (!pMediaSink)
    {
      LOG(ERROR) << "Failed to create media sink.";
      return -1;
    }

    media::YuvMediaSource yuvMediaSource(sYuvFile, uiWidth, uiHeight, bRepeat, uiLoopCount);

    int iCurrentFrame = 0;
    uint32_t uiCurrentRateKbpsIndex = 0;
    uint32_t uiCurrentSwitchFrameIndex = 0;
    auto start = std::chrono::steady_clock::now();

    while (yuvMediaSource.isGood())
    {
      std::vector<media::MediaSample> encodedSamples;
      std::vector<media::MediaSample> frame = yuvMediaSource.getNextAccessUnit();
      if (!frame.empty())
      {
        if ((uiCurrentSwitchFrameIndex < vSwitchFrames.size()) &&
            (vSwitchFrames[uiCurrentSwitchFrameIndex] == iCurrentFrame) &&
            (uiCurrentRateKbpsIndex < vKbps.size())
            )
        {
          VLOG(2) << "Setting next bitrate to " << vKbps[uiCurrentRateKbpsIndex] << " kbps Current frame: " << iCurrentFrame;
          boost::system::error_code ec = pCodec->setBitrate(vKbps[uiCurrentRateKbpsIndex++]);
          if (ec)
          {
            LOG(WARNING) << "Failed to update bitrate to " << vKbps[uiCurrentRateKbpsIndex - 1] << "kbps";
          }
          ++uiCurrentSwitchFrameIndex;
        }

        // encode
        uint32_t uiEncodedSize = 0;
        boost::system::error_code ec = pCodec->transform(frame, encodedSamples, uiEncodedSize);
        if (ec)
        {
          LOG(WARNING) << "Error in media encode: " << ec.message();
          return -1;
        }
        else
        {
          // write to sink
          pMediaSink->writeAu(encodedSamples);
        }
        ++iCurrentFrame;
      }
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
    LOG(INFO) << "Read " << iCurrentFrame << " frames in " << sYuvFile << " (" << elapsed_ms.count() << " ms)";
  }
  catch (boost::exception& e)
  {
    LOG(ERROR) << "Exception: %1%" << boost::diagnostic_information(e);
  }
  catch (std::exception& e)
  {
    LOG(ERROR) << "Exception: %1%" << e.what();
  }
  catch (...)
  {
    LOG(ERROR) << "Unknown exception!!!";
  }

  return 0;
}
