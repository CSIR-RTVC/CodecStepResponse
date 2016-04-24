# source files
SET(NALU_SRCS
main.cpp
)

INCLUDE_DIRECTORIES(
${Includes}
)

ADD_EXECUTABLE(NaluInfo ${NALU_SRCS} ${NALU_HEADERS})

TARGET_LINK_LIBRARIES (
NaluInfo
${rtp++Libs}
)

install(TARGETS NaluInfo
            RUNTIME DESTINATION ${rtp++_BIN}
            LIBRARY DESTINATION ${rtp++_BIN}
            ARCHIVE DESTINATION ${rtp++_SOURCE_DIR}/../lib)

