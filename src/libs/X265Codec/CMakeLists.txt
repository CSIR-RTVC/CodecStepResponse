# CMakeLists.txt in x265Codec dir

IF(WIN32)
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_BUILD_FOR_SHORT")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
# get rid of some compiler warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
ELSE(WIN32)
add_definitions("-Wwrite-strings")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
ENDIF(WIN32)

INCLUDE_DIRECTORIES(
${CodecStepResponseIncludes}
$ENV{X265_DIR}/source
)

# Lib directories
LINK_DIRECTORIES(
${CodecStepResponseLink}
)

SET(H265_LIB_HDRS
stdafx.h
X265Codec.h
)	

SET(H265_LIB_SRCS 
stdafx.cpp
X265Codec.cpp
)

ADD_LIBRARY( X265Codec SHARED ${H265_LIB_SRCS} ${H265_LIB_HDRS})

TARGET_LINK_LIBRARIES(
X265Codec
x265
glog
) 

