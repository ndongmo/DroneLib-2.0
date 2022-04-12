# DroneLib 2.0 C++ project: Lib package tests

PROJECT(Lib-test)

SET(SOURCE_FILES
	main.cpp
)

# Apple specific stuff
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

#-----------------------------------------------------
# DELIVERING binaries and header to ${DRONELIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DRONELIB_HOME}/app/bin
)