cmake_minimum_required(VERSION 2.6)

PROJECT( rtp++ )

#find_package (glog 0.3.4 REQUIRED)

message("Source Dir:" ${rtp++_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${rtp++_SOURCE_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH ${rtp++_SOURCE_DIR}/../lib)
SET(RUNTIME_OUTPUT_DIRECTORY ${rtp++_SOURCE_DIR}/../bin)

SET(rtp++_BIN ${rtp++_SOURCE_DIR}/../bin)

# for profiling on linux
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

message("rtp++_SOURCE_DIR directories:" ${rtp++_SOURCE_DIR})
message("GLOG_DIR directories:" $ENV{GLOG_DIR})
message("CPP_UTIL_DIR:" $ENV{CPP_UTIL_DIR})
message("BOOST_INCLUDE_DIR:" $ENV{BOOST_INCLUDE_DIR})

IF(WIN32)
IF(MSVC12)
set(ARTIST_CONTRIB_DIR_LIBS $ENV{ARTIST_CONTRIB_DIR}/lib/VC12)
ELSEIF(MSVC11)
set(ARTIST_CONTRIB_DIR_LIBS $ENV{ARTIST_CONTRIB_DIR}/lib/VC11)
ELSEIF(MSVC10)
set(ARTIST_CONTRIB_DIR_LIBS $ENV{ARTIST_CONTRIB_DIR}/lib/VC10)
ELSE(MSVC12)
set(ARTIST_CONTRIB_DIR_LIBS $ENV{ARTIST_CONTRIB_DIR}/lib)
ENDIF(MSVC12)
ENDIF(WIN32)

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

IF(DEFINED ENV{USRSCTP_DIR})
message("USRSCTP_DIR:" $ENV{USRSCTP_DIR})
add_definitions(-DENABLE_SCTP_USERLAND)
ELSE()
message("No SCTP support")
ENDIF()

IF(DEFINED ENV{LIB_SRTP_DIR})
message("LIB_SRTP_DIR:" $ENV{LIB_SRTP_DIR})
add_definitions(-DENABLE_LIB_SRTP)
ELSE()
message("No SRTP support")
ENDIF()

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

message(CMAKE_ROOT: ${CMAKE_ROOT} )
message(CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR} )


IF(WIN32)
SET(rtp++Includes
${rtp++_SOURCE_DIR}
${rtp++_SOURCE_DIR}/lib
${rtp++_SOURCE_DIR}/libs
$ENV{GLOG_DIR}/src/windows
$ENV{BOOST_INCLUDE_DIR}
)
ELSEIF(UNIX)
# Include directories
SET(rtp++Includes
${rtp++_SOURCE_DIR}
${rtp++_SOURCE_DIR}/lib
${rtp++_SOURCE_DIR}/libs
${rtp++_SOURCE_DIR}/../include/rtp++/
${CMAKE_ROOT}/../build/
${CMAKE_CURRENT_BINARY_DIR}/
${rtp++_SOURCE_DIR}/../build/
${rtp++_SOURCE_DIR}/../externals/glog/src/
$ENV{BOOST_INCLUDE_DIR}
/usr/local/include
)
ENDIF(WIN32)

message("rtp++Includes directories:" ${rtp++Includes})

# lib directories
IF(WIN32)
SET(rtp++Link
${rtp++_SOURCE_DIR}/../lib
$ENV{BOOST_LIB_DIR}
)
ELSEIF(UNIX)
SET(rtp++Link
${rtp++_SOURCE_DIR}/../lib
${BOOST_LIB_DIR}
/usr/local/lib
/usr/lib
)
ENDIF(WIN32)

message("rtp++Link directories:" ${rtp++Link})

# libs
IF(WIN32)
# Do windows specific includes
SET(rtp++Libs
rtp++
glog
)
ELSEIF(APPLE)
# Do apple specific includes
SET(rtp++Libs
rtp++
glog
boost_date_time boost_filesystem boost_system boost_thread boost_program_options boost_regex
pthread
c++
)
ELSEIF(UNIX)
# Do linux specific includes
SET(rtp++Libs
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
${rtp++_SOURCE_DIR}/../lib
$ENV{BOOST_LIB_DIR}
)

message("LibDir directories:" ${LibDir})

# Include directories
INCLUDE_DIRECTORIES(
${rtp++_SOURCE_DIR}/../include
)

# build externals first
ADD_SUBDIRECTORY( ../externals/glog ../build)

#ADD_SUBDIRECTORY( Apps) 
ADD_SUBDIRECTORY( lib) 
#ADD_SUBDIRECTORY( libs)
ADD_SUBDIRECTORY( test )
#ADD_SUBDIRECTORY( Tools )

