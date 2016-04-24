// To prevent double inclusion of winsock on windows
#ifdef _WIN32
// To be able to use std::max
#define NOMINMAX
#include <WinSock2.h>
#endif

#ifdef _WIN32
#pragma warning(push)     // disable for this header only
#pragma warning(disable:4251)
// To get around compile error on windows: ERROR macro is defined
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>
#ifdef _WIN32
#pragma warning(pop)     // restore original warning level
#endif

#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <rtp++/media/NalUnitMediaSource.h>

using namespace boost::program_options;
using namespace rtp_plus_plus;

void validateInput(const std::string& sAnnexBFile)
{
  if (!boost::filesystem::exists(sAnnexBFile))
  {
    LOG(ERROR) << "Input file " << sAnnexBFile << " does not exist";
    throw validation_error(validation_error::invalid_option_value);
  }
}

void validateFormat(const std::string& sMediaType)
{
  std::vector<std::string> acceptedFormats{"h264","H264","h265","H265","h264-svc","H264-SVC"};
  auto it = std::find_if(acceptedFormats.begin(), acceptedFormats.end(), [sMediaType](const std::string& sFormat) { return sMediaType != sFormat;});
  if (it == acceptedFormats.end())
  {
    LOG(ERROR) << "Unsupported media format: " << sMediaType;
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

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv)
{
  google::InitGoogleLogging(argv[0]);
  try
  {
    bool bVerbose;
    std::string sInput;
    std::string sMediaType;
    double dFps = 0.0;
    std::string sOutput;
    bool bExtractBaseLayer;
    bool bOutputVideoMetaData;

    options_description desc("Allowed options");
    desc.add_options()
        ("help,?", "produce help message")
        ("vvv,v", bool_switch(&bVerbose)->default_value(false), "Verbose [false]")
        ("input,i", value<std::string>(&sInput)->required()->notifier(validateInput), "input = [<<file_name>>]")
        ("format", value<std::string>(&sMediaType)->notifier(validateFormat), "media format = [h264 | h265]")
        ("fps,f", value<double>(&dFps)->required()->notifier(validateFps), "Frames per second")
        //("limit-rate,l", po::bool_switch(&bLimitRate)->default_value(false), "Limit frame rate to fps")
        ("output,o", value<std::string>(&sOutput)->default_value(""), "Outfile")
        ("extractBl,x", bool_switch(&bExtractBaseLayer)->default_value(false), "Extract base layer only (for SVC streams) to output file")
        ("output-meta-data,m", bool_switch(&bOutputVideoMetaData)->default_value(false), "Output video meta data to files (file source only)")
        ;

    positional_options_description p;
    p.add("input", 0);

    variables_map vm;
    store(command_line_parser(argc, argv).
              options(desc).positional(p).run(), vm);
    if (vm.count("help"))
    {
      std::ostringstream ostr;
      ostr << desc;
      LOG(ERROR) << ostr.str();
      return 1;
    }
    notify(vm);

    if (sMediaType.empty())
    {
      // try infer from file extension
      std::string sExt = boost::filesystem::path(sInput).extension().string();
      if (sExt == ".264")
        sMediaType = rfc6184::H264;
      else if (sExt == ".265")
        sMediaType = rfchevc::H265;
      else
      {
        // TODO: could look at NAL units to try and guess what type of NALUs we're dealing with.
        LOG(ERROR) << "Unsupported media type";
        return -1;
      }
    }
    media::NalUnitMediaSource naluMediaSource(sInput, sMediaType, false, 0);
    int iCount = 0;
    int iNalCount = 0;
    double dAuDuration = 1.0/dFps;

    while (naluMediaSource.isGood())
    {
      std::vector<media::MediaSample> nalus = naluMediaSource.getNextAccessUnit();
      if (!nalus.empty())
      {
        iNalCount+= nalus.size();
        uint32_t uiTotalAuSize = std::accumulate(nalus.begin(), nalus.end(), 0, []
                                                 (int iSum, const media::MediaSample& mediaSample)
        {
          return iSum + mediaSample.getPayloadSize();
        });
        if (bVerbose)
        {
          std::ostringstream sizes;
          for (auto& mediaSample : nalus)
          {
            sizes << mediaSample.getPayloadSize() << " ";
          }
          LOG(INFO) << "AU " << iCount << " Start: " << dAuDuration * iCount << " Size: " << uiTotalAuSize << " (" << sizes.str() << ") ";
        }
        ++iCount;
      }
    }
    VLOG(2) << "Total frames: " << iCount << " Total NALUs: " << iNalCount;

    return 0;
  }
  catch (boost::exception& e)
  {
    LOG(ERROR) << "Exception: " << boost::diagnostic_information(e);
  }
  catch (std::exception& e)
  {
    LOG(ERROR) << "Exception: " << e.what();
  }
  catch (...)
  {
    LOG(ERROR) << "Unknown exception!!!";
  }
  return 1;
}
