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

TARGET_LINK_LIBRARIES (
EvalCodecStepResponse
${CodecStepResponseLibs}
OpenH264Codec
X264Codec
X265Codec
)

install(TARGETS EvalCodecStepResponse
            RUNTIME DESTINATION ${CodecStepResponse_BIN}
            LIBRARY DESTINATION ${CodecStepResponse_BIN}
            ARCHIVE DESTINATION ${CodecStepResponse_SOURCE_DIR}/../lib)

