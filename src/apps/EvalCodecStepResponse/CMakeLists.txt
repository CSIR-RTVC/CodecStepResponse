# source files for EvalCodecStepResponse 
SET(CSR_SRCS
main.cpp
)

SET(CSR_HEADERS
stdafx.h
)

INCLUDE_DIRECTORIES(
${CodecStepResponseIncludes}
)

LINK_DIRECTORIES(
${CodecStepResponseLink}
)

ADD_EXECUTABLE(EvalCodecStepResponse ${CSR_SRCS} ${CSR_HEADERS})

SET(CodecLibs
${CodecStepResponseLibs}
OpenH264Codec
X264Codec
X265Codec
)

IF(BUILD_VPP)
SET(CodecLibs
${CodecLibs}
VppH264Codec
)
#ELSE(BUILD_VPP)
#SET(CodecLibs
#${CodecStepResponseLibs}
#OpenH264Codec
#X264Codec
#X265Codec
#)
ENDIF(BUILD_VPP)

TARGET_LINK_LIBRARIES (
EvalCodecStepResponse
${CodecStepResponseLibs}
${CodecLibs}
)

install(TARGETS EvalCodecStepResponse
            RUNTIME DESTINATION ${CodecStepResponse_BIN}
            LIBRARY DESTINATION ${CodecStepResponse_BIN}
            ARCHIVE DESTINATION ${CodecStepResponse_SOURCE_DIR}/../lib)

