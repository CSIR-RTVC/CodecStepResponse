#include "stdafx.h"
#include <sstream>
#include <vector>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <rtp++/media/YuvMediaSource.h>

using namespace rtp_plus_plus;
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
    uint32_t uiWidth = 0;
    uint32_t uiHeight = 0;
    double dFps = 0.0;
    uint32_t uiLoopCount = 1;
    std::string sLogfile, sLogDir;
    options_description cmdline_options;
    cmdline_options.add_options()
        ("help,?", "produce help message")
        ("log,l", value<std::string>(&sLogfile)->default_value((boost::filesystem::path(argv[0]).leaf().string()), "Output log file."))
        ("log-dir,L", value<std::string>(&sLogDir)->default_value(".", "Output log dir."))
        ("input,i", value<std::string>(&sYuvFile)->required()->notifier(validateYuvInput), "YUV input file")
        ("width,w", value<uint32_t>(&uiWidth)->required()->notifier(validateWidth), "Width")
        ("height,h", value<uint32_t>(&uiHeight)->required()->notifier(validateHeight), "Height")
        ("fps,f", value<double>(&dFps)->required()->notifier(validateFps), "FPS")
        ("loop-count,c", value<uint32_t>(&uiLoopCount)->default_value(1), "Loop count. 0 = infinite.")
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

    media::YuvMediaSource yuvMediaSource(sYuvFile, uiWidth, uiHeight, uiLoopCount > 1, uiLoopCount);

    int iCount = 0;
    while (yuvMediaSource.isGood())
    {
      std::vector<media::MediaSample> frame = yuvMediaSource.getNextAccessUnit();
      if (!frame.empty())
      {
        ++iCount;
      }
    }
    LOG(INFO) << "Read " << iCount << " frames in " << sYuvFile;
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
