# DroneLib 2.0 C++ project: Lib package source

PROJECT(${DRONELIB_NAME})

SET(SOURCE_FILES
	utils/Config.cpp
	utils/Logger.cpp
	net/NetTcp.cpp
	net/NetUdp.cpp
	net/NetHelper.cpp
	net/NetSender.cpp
	net/NetReceiver.cpp
	stream/StreamPool.cpp
	stream/StreamReceiver.cpp
)

# Apple specific stuff
if(APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES PREFIX "")

#-----------------------------------------------------
# DELIVERING binaries and header to ${DRONELIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DRONELIB_HOME}/app/bin
    LIBRARY DESTINATION ${DRONELIB_HOME}/app/bin
    ARCHIVE DESTINATION ${DRONELIB_HOME}/app/lib
)

INSTALL(DIRECTORY ../include/ DESTINATION ${DRONELIB_HOME}/app/include/${PROJECT_NAME}
	FILES_MATCHING PATTERN "*.h"
)