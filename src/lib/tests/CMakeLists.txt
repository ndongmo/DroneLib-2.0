# DroneLib 2.0 C++ project: Lib Tests package

PROJECT(Lib-tests)

SET(SOURCE_FILES
	main.cpp
	utils/ConfigTest.cpp
	utils/LoggerTest.cpp
	net/NetUdpTest.cpp
	net/NetTcpTest.cpp
	net/NetHelperTest.cpp
)

# Apple specific stuff
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

FIND_PACKAGE(Threads)

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_TEST libgtest.a ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_MOCK libgmock.a ${THIRD_PARTY_LIB})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_TEST libgtest.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_MOCK libgmock.dylib ${THIRD_PARTY_LIB})
ELSE()
	FIND_LIBRARY(LIB_TEST libgtest ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_MOCK libgmock ${THIRD_PARTY_LIB})
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
#ADD_TEST(NAME LibTests COMMAND ${PROJECT_NAME})

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${DRONELIB_NAME}
		debug ${LIB_TEST}
		debug ${LIB_MOCK}
		debug Threads::Threads
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${DRONELIB_NAME}
		optimized ${LIB_TEST}
		optimized ${LIB_MOCK}
		optimized Threads::Threads
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${DRONELIB_NAME})

INCLUDE(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} EXTRA_ARGS ARGS=-V)

#-----------------------------------------------------
# DELIVERING binaries and header to ${DRONELIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)