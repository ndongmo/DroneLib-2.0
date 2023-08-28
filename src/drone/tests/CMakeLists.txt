# DroneLib 2.0 C++ project: Drone Tests package

PROJECT(Drone-tests)

SET(SOURCE_FILES
	main.cpp
	DroneReceiverTest.cpp
	DroneSenderTest.cpp
	DroneControllerTest.cpp
	controller/LedControllerTest.cpp
	controller/ServoControllerTest.cpp
	controller/MotorControllerTest.cpp
	controller/BuzzerControllerTest.cpp
	controller/BatteryControllerTest.cpp
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

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${LIB}
		debug ${DRONE}
		debug ${LIB_GTEST}
		debug ${LIB_GMOCK}
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${LIB}
		optimized ${DRONE}
		optimized ${LIB_GTEST}
		optimized ${LIB_GMOCK}
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${LIB} ${DRONE})

INCLUDE(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} EXTRA_ARGS ARGS=-V)

#-----------------------------------------------------
# DELIVERING binaries and header to ${APP_OUTPUT_BIN}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)