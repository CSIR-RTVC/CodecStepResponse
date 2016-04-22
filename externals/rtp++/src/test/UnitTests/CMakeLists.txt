# source files for Test

SET(TEST_CORE_HEADERS
Media.h
)

SET(TEST_CORE_SRCS
main.cpp
)

INCLUDE_DIRECTORIES(
${rtp++Includes}
)

LINK_DIRECTORIES(
${rtp++Link}
)

ADD_EXECUTABLE(UnitTests ${TEST_CORE_SRCS} ${TEST_CORE_HEADERS})

IF(WIN32)
# Do windows specific includes
TARGET_LINK_LIBRARIES (
UnitTests
rtp++
glog
)
ELSEIF(UNIX)
# Do linux specific includes
TARGET_LINK_LIBRARIES (
UnitTests
rtp++
glog
# Do linux specific includes
boost_chrono boost_date_time boost_filesystem boost_regex boost_signals boost_system boost_thread boost_unit_test_framework
${rtp++Libs}
)
ENDIF(WIN32)

install(TARGETS UnitTests DESTINATION rtp++_SOURCE_DIR}/../test})

