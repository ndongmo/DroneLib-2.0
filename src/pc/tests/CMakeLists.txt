# DroneLib 2.0 C++ project: PC Tests package

PROJECT(PC-tests)

SET(SOURCE_FILES
	main.cpp
	EventHandlerTest.cpp
	PCReceiverTest.cpp
	PCSenderTest.cpp
	PCWindowTest.cpp
	PCControllerTest.cpp
)

# Apple specific stuff
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

FIND_PACKAGE(Threads)

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_TEST libgtest.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_MOCK libgmock.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_SDL libSDL2.so ${THIRD_PARTY_BIN})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_TEST libgtest.dylib ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_MOCK libgmock.dylib ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_SDL libSDL2.dylib ${THIRD_PARTY_BIN})
ELSE()
	FIND_LIBRARY(LIB_TEST libgtest ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_MOCK libgmock ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_SDL libSDL2 ${THIRD_PARTY_BIN})
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
#ADD_TEST(NAME LibTests COMMAND ${PROJECT_NAME})

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${PCLIB_NAME}
		debug ${DRONELIB_NAME}
		debug ${LIB_TEST}
		debug ${LIB_MOCK}
		debug ${LIB_SDL}
		debug Threads::Threads
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${PCLIB_NAME}
		optimized ${DRONELIB_NAME}
		optimized ${LIB_TEST}
		optimized ${LIB_MOCK}
		optimized ${LIB_SDL}
		optimized Threads::Threads
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${PCLIB_NAME})
add_dependencies(${PROJECT_NAME} ${DRONELIB_NAME})

INCLUDE(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} EXTRA_ARGS ARGS=-V)

#-----------------------------------------------------
# DELIVERING binaries and header to ${DRONELIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DRONELIB_HOME}/app/bin
)