# CMakeLists.txt in Lib dir

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledBinary}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  message("Clang detected: setting warning level")
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-unused-function")
#  ADD_DEFINITIONS (-Werror )
#  ADD_DEFINITIONS (-Wall )
#  ADD_DEFINITIONS (/W4 )
#  set_property( GLOBAL APPEND_STRING PROPERTY COMPILE_FLAGS -Werror )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

SET(CORE_SRCS
stdafx.cpp
)
SET(EXPERIMENTAL_SRCS
)
SET(MEDIA_SRCS
media/YuvMediaSource.cpp
)
SET(UTIL_SRCS
util/Base64.cpp
)
SET(CORE_HEADERS
stdafx.h
../../include/rtp++/Version.h
)
SET(EXPERIMENTAL_HEADERS
../../include/rtp++/experimental/INetworkCodecCooperation.h
)
SET(MEDIA_HEADERS
../../include/rtp++/media/IMediaTransform.h
../../include/rtp++/media/IVideoCodecTransform.h
../../include/rtp++/media/MediaDescriptor.h
../../include/rtp++/media/MediaSample.h
../../include/rtp++/media/MediaSource.h
../../include/rtp++/media/YuvMediaSource.h
)
SET(UTIL_HEADERS
../../include/rtp++/util/Base64.h
../../include/rtp++/util/Buffer.h
../../include/rtp++/util/Conversion.h
)
SET(RTP_SRCS
${CORE_SRCS} 
${EXPERIMENTAL_SRCS} 
${MEDIA_SRCS}
${UTIL_SRCS}
)

SOURCE_GROUP("Source Files\\core" FILES ${CORE_SRCS})
SOURCE_GROUP("Source Files\\experimental" FILES ${EXPERIMENTAL_SRCS})
SOURCE_GROUP("Source Files\\media" FILES ${MEDIA_SRCS})
SOURCE_GROUP("Source Files\\util" FILES ${UTIL_SRCS})


SOURCE_GROUP("Header Files\\core" FILES ${CORE_HEADERS})
SOURCE_GROUP("Header Files\\experimental" FILES ${EXPERIMENTAL_HEADERS})
SOURCE_GROUP("Header Files\\media" FILES ${MEDIA_HEADERS})
SOURCE_GROUP("Header Files\\util" FILES ${UTIL_HEADERS})


SET(RTP_HEADERS 
${CORE_HEADERS}
${EXPERIMENTAL_HEADERS} 
${MEDIA_HEADERS}
${UTIL_HEADERS}
)

INCLUDE_DIRECTORIES(
${rtp++Includes}
#${rtp++_SOURCE_DIR}/Libs/OpenH264Codec
#${rtp++_SOURCE_DIR}/Libs/VppH264Codec
#${rtp++_SOURCE_DIR}/Libs/X264Codec
#${rtp++_SOURCE_DIR}/Libs/X265Codec
)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
# 64bit
  SET(CMAKE_CXX_FLAGS "-fPIC")
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
# 32 bit
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

# ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" CORE_SRCS)

ADD_LIBRARY( rtp++ STATIC ${RTP_SRCS} ${RTP_HEADERS})


# adding precompiled header support on windows
if (MSVC)
   set_target_properties(rtp++ PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

