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
	IController.cpp
	Service.cpp
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
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
    LIBRARY DESTINATION ${APP_OUTPUT_BIN}
    ARCHIVE DESTINATION ${APP_OUTPUT_LIB}
)

INSTALL(DIRECTORY ../include/ DESTINATION ${APP_OUTPUT_INC}/${PROJECT_NAME}
	FILES_MATCHING PATTERN "*.h"
)