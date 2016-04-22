#include "stdafx.h"
#include <vector>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <rtp++/media/YuvMediaSource.h>

using namespace rtp_plus_plus;
using namespace boost::program_options;

void onYuvInput(const std::string& sYuvFile)
{
  if (!boost::filesystem::exists(sYuvFile))
  {
    LOG(ERROR) << "YUV input file " << sYuvFile << " does not exist";
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
    options_description cmdline_options;
    cmdline_options.add_options()
        ("help,h", "produce help message")
        ("input,i", value<std::string>(&sYuvFile)->required()->notifier(onYuvInput), "YUV input file");

    variables_map vm;
    store(command_line_parser(argc, argv).
              options(cmdline_options).run(), vm);
    notify(vm);
    if (vm.count("help"))
    {
      std::ostringstream ostr;
      ostr << cmdline_options;
      LOG(ERROR) << ostr.str();
      return 1;
    }

    const uint32_t uiWidth = 352;
    const uint32_t uiHeight = 288;
    media::YuvMediaSource yuvMediaSource(sYuvFile, uiWidth, uiHeight, false, 0);

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
