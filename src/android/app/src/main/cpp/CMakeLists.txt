# DroneLib 2.0 C++ project: Android controller package

cmake_minimum_required(VERSION 3.9)

PROJECT(native-lib)

SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)

SET(CLIENT_INC ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../client/include)
SET(LIB_INC ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../lib/include)
SET(THIRD_PARTY_INC ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../third-party/include)

INCLUDE_DIRECTORIES(
	${LIB_INC}
	${CLIENT_INC}
	${THIRD_PARTY_INC}
)

SET(SOURCE_FILES
	EventHandler.cpp
	AndroidController.cpp
	native-lib.cpp
)

FIND_LIBRARY(DRONE_CLIENT libClient.so libs)
FIND_LIBRARY(DRONE_LIB Lib.so libs)
FIND_LIBRARY(LOG_LIB log)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
	DEBUG_POSTFIX "d"
)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${DRONE_LIB}
		debug ${DRONE_CLIENT}
        debug ${LOG_LIB}
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${DRONE_LIB}
		optimized ${DRONE_CLIENT}
		optimized ${LOG_LIB}
	)
ENDIF()