# DroneLib 2.0 C++ project: Lib Tests package

PROJECT(Lib-tests)

SET(SOURCE_FILES
	main.cpp
	utils/ConfigTest.cpp
	utils/LoggerTest.cpp
	net/NetUdpTest.cpp
	net/NetTcpTest.cpp
	net/NetHelperTest.cpp
	stream/StreamPoolTest.cpp
	stream/MediaSenderTest.cpp
	stream/StreamSenderTest.cpp
	stream/StreamReceiverTest.cpp
)

IF(APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
ENDIF()

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_GTEST libgtest.a ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK libgmock.a ${THIRD_PARTY_LIB})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_GTEST libgtest.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK libgmock.dylib ${THIRD_PARTY_LIB})
ELSE()
	FIND_LIBRARY(LIB_GTEST gtest ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK gmock ${THIRD_PARTY_LIB})
ENDIF()

IF (WIN32)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${THIRD_PARTY_BIN})
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${LIB}
		debug ${LIB_GTEST}
		debug ${LIB_GMOCK}
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${LIB}
		optimized ${LIB_GTEST}
		optimized ${LIB_GMOCK}
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${LIB})

INCLUDE(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} EXTRA_ARGS ARGS=-V)

#-----------------------------------------------------
# DELIVERING binaries and header to ${LIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)