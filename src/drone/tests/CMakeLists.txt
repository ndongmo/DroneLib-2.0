# DroneLib 2.0 C++ project: Drone Tests package

PROJECT(Drone-tests)

SET(SOURCE_FILES
	main.cpp
	DroneReceiverTest.cpp
	DroneSenderTest.cpp
	DroneControllerTest.cpp
	controller/LedControllerTest.cpp
	controller/MotorControllerTest.cpp
)

# Apple specific stuff
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

FIND_PACKAGE(Threads)

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_GTEST libgtest.a ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK libgmock.a ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_WS2811 libws2811.a ${THIRD_PARTY_LIB})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_GTEST libgtest.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK libgmock.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_WS2811 libws2811.dylib ${THIRD_PARTY_LIB})
ELSE()
	FIND_LIBRARY(LIB_GTEST libgtest ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK libgmock ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_WS2811 libws2811 ${THIRD_PARTY_LIB})
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${LIB}
		debug ${DRONE_LIB}
		debug ${LIB_GTEST}
		debug ${LIB_GMOCK}
		debug ${LIB_WS2811}
		debug i2c
		debug Threads::Threads
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${LIB}
		optimized ${DRONE_LIB}
		optimized ${LIB_GTEST}
		optimized ${LIB_GMOCK}
		optimized ${LIB_WS2811}
		optimized i2c
		optimized Threads::Threads
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${DRONE_LIB})
add_dependencies(${PROJECT_NAME} ${LIB})

INCLUDE(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} EXTRA_ARGS ARGS=-V)

#-----------------------------------------------------
# DELIVERING binaries and header to ${APP_OUTPUT_BIN}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)