# DroneLib 2.0 C++ project: Drone package

PROJECT(Drone)

SET(SOURCE_FILES
	main.cpp
	Controller.cpp
	Network.cpp
)

# Apple specific stuff
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

FIND_PACKAGE(Threads)

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_SDL libSDL2.so ${THIRD_PARTY_BIN})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_SDL libSDL2.dylib ${THIRD_PARTY_LIB})
ELSE()
	FIND_LIBRARY(LIB_SDL libSDL2 ${THIRD_PARTY_LIB})
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
ADD_LIBRARY(${DRONE_TEST_LIB} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${DRONELIB_NAME}
		debug ${LIB_SDL}
		debug Threads::Threads
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${DRONELIB_NAME}
		optimized ${LIB_SDL}
		optimized Threads::Threads
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${DRONELIB_NAME})

#-----------------------------------------------------
# DELIVERING binaries and header to ${DRONELIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DRONELIB_HOME}/app/bin
)