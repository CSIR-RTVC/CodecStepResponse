# source files
SET(PSNR_SRCS
main.cpp
)

INCLUDE_DIRECTORIES(
${Includes}
)

ADD_EXECUTABLE(GeneratePSNR ${PSNR_SRCS} ${PSNR_HEADERS})

TARGET_LINK_LIBRARIES (
GeneratePSNR
)

install(TARGETS GeneratePSNR
            RUNTIME DESTINATION ${CodecStepResponse_BIN}
            LIBRARY DESTINATION ${CodecStepResponse_BIN}
            ARCHIVE DESTINATION ${CodecStepResponse_SOURCE_DIR}/../lib)

