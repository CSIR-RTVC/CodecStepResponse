cmake_minimum_required(VERSION 2.6)

PROJECT( CodecStepResponse )

message("Source Dir:" ${CodecStepResponse_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${CodecStepResponse_SOURCE_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH ${CodecStepResponse_SOURCE_DIR}/../bin)
SET(RUNTIME_OUTPUT_DIRECTORY ${CodecStepResponse_SOURCE_DIR}/../bin)

SET(CodecStepResponse_BIN ${CodecStepResponse_SOURCE_DIR}/../bin)

message("CodecStepResponse_SOURCE_DIR directories:" ${CodecStepResponse_SOURCE_DIR})
message("GLOG_DIR directories:" $ENV{GLOG_DIR})
message("CPP_UTIL_DIR:" $ENV{CPP_UTIL_DIR})
message("BOOST_INCLUDE_DIR:" $ENV{BOOST_INCLUDE_DIR})

IF(WIN32)
IF(MSVC12)
message("Detected MSVC12 compiler")
set(MSVC_VER VC12)
ELSEIF(MSVC11)
message("Detected MSVC11 compiler")
set(MSVC_VER VC11)
ELSEIF(MSVC10)
message("Detected MSVC10 compiler")
set(MSVC_VER VC10)
ELSE(MSVC12)
message("WARNING: Unknown/unsupported MSVC version")
ENDIF(MSVC12)
ENDIF(WIN32)

IF (UNIX)
message("Adding c++11 support to compiler")
add_definitions(-std=c++11)
ENDIF(UNIX)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
# 64bit
  message("Detected 64-bit build - compiling with -fPIC")
  SET(CMAKE_CXX_FLAGS "-fPIC")
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
# 32 bit
message("Detected 32-bit build")
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

SET(BUILD_VPP true)
IF(BUILD_VPP)
add_definitions(-DENABLE_VPP)
ENDIF(BUILD_VPP)

IF(DEFINED ENV{OPEN_H264_DIR})
message("OPEN_H264_DIR:" $ENV{OPEN_H264_DIR})
add_definitions(-DENABLE_OPEN_H264)
SET(BUILD_OPEN_H264 true)
ELSE()
message("No OpenH264 support")
SET(BUILD_OPEN_H264 false)
ENDIF()

IF(DEFINED ENV{X264_DIR})
message("X264_DIR:" $ENV{X264_DIR})
add_definitions(-DENABLE_X264)
SET(BUILD_X264 true)
ELSE()
message("No x264 support")
SET(BUILD_X264 false)
ENDIF()

IF(DEFINED ENV{X265_DIR})
message("X265_DIR:" $ENV{X265_DIR})
add_definitions(-DENABLE_X265)
SET(BUILD_X265 true)
ELSE()
message("No x265 support")
SET(BUILD_X2645 false)
ENDIF()

IF(WIN32)
# Assuming the Foo-root dir is defined in FOO_DIR
add_library( glog UNKNOWN IMPORTED )
# You can define two import-locations: one for debug and one for release.
set_target_properties( glog PROPERTIES IMPORTED_LOCATION_DEBUG $ENV{GLOG_DIR}/lib/Debug/libglog.lib )
set_target_properties( glog PROPERTIES IMPORTED_LOCATION_RELEASE $ENV{GLOG_DIR}/lib/Release/libglog.lib )
ENDIF(WIN32)

# DCCP definitions: dccp code only works on linux
IF(UNIX)
IF(APPLE)
ELSE(APPLE)
IF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  # No DCCP on FreeBSD
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
#add_definitions(-DENABLE_ASIO_DCCP)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
ENDIF(APPLE)
ENDIF(UNIX)

IF(WIN32)
SET(CodecStepResponseIncludes
${CodecStepResponse_SOURCE_DIR}
${CodecStepResponse_SOURCE_DIR}/lib
${CodecStepResponse_SOURCE_DIR}/libs
${CodecStepResponse_SOURCE_DIR}/../externals/rtp++/include
$ENV{GLOG_DIR}/src/windows
$ENV{BOOST_INCLUDE_DIR}
)
ELSEIF(UNIX)
# Include directories
SET(CodecStepResponseIncludes
${CodecStepResponse_SOURCE_DIR}
${CodecStepResponse_SOURCE_DIR}/lib
${CodecStepResponse_SOURCE_DIR}/libs
${CodecStepResponse_SOURCE_DIR}/../externals/rtp++/include
${CodecStepResponse_SOURCE_DIR}/../externals/rtp++/build/
${CodecStepResponse_SOURCE_DIR}/../externals/rtp++/externals/glog/src/
#${CodecStepResponse_SOURCE_DIR}/../include/CodecStepResponse/
$ENV{BOOST_INCLUDE_DIR}
/usr/local/include
)
ENDIF(WIN32)

message("CodecStepResponseIncludes directories:" ${CodecStepResponseIncludes})

# lib directories
IF(WIN32)
SET(CodecStepResponseLink
${CodecStepResponse_SOURCE_DIR}/../lib
${CodecStepResponse_SOURCE_DIR}/../externals/rtp++/lib
$ENV{BOOST_LIB_DIR}
)
ELSEIF(UNIX)
SET(CodecStepResponseLink
${CodecStepResponse_SOURCE_DIR}/../lib
${CodecStepResponse_SOURCE_DIR}/../externals/rtp++/lib
${BOOST_LIB_DIR}
/usr/local/lib
/usr/lib
)
ENDIF(WIN32)

message("CodecStepResponseLink directories:" ${CodecStepResponseLink})

# libs
IF(WIN32)
# Do windows specific includes
SET(CodecStepResponseLibs
CodecStepResponse
rtp++
glog
)
ELSEIF(APPLE)
# Do apple specific includes
SET(CodecStepResponseLibs
rtp++
glog
boost_date_time boost_filesystem boost_system boost_thread boost_program_options boost_regex
pthread
c++
)
ELSEIF(UNIX)
# Do linux specific includes
SET(CodecStepResponseLibs
rtp++
#OpenH264Codec
#VppH264Codec
#X264Codec
#X265Codec
glog
boost_date_time boost_filesystem boost_system boost_thread boost_program_options boost_regex
pthread
)
ENDIF(WIN32) 


SET(LibDir
${CodecStepResponse_SOURCE_DIR}/../lib
$ENV{BOOST_LIB_DIR}
)

message("LibDir directories:" ${LibDir})

# Include directories
INCLUDE_DIRECTORIES(
${CodecStepResponse_SOURCE_DIR}/../include
)

ADD_SUBDIRECTORY( ../externals/rtp++/src ../build)

ADD_SUBDIRECTORY( apps ) 
#ADD_SUBDIRECTORY( lib ) 
ADD_SUBDIRECTORY( libs )
#ADD_SUBDIRECTORY( test )
ADD_SUBDIRECTORY( tools )


