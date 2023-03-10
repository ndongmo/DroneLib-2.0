# DroneLib 2.0 C++ project: Lib package source

PROJECT(${LIB})

SET(SOURCE_FILES
	utils/Config.cpp
	utils/Logger.cpp
	net/NetTcp.cpp
	net/NetUdp.cpp
	net/NetHelper.cpp
	net/NetSender.cpp
	net/NetReceiver.cpp
	stream/StreamPool.cpp
	stream/StreamSender.cpp
	stream/VideoSender.cpp
	stream/AudioSender.cpp
	stream/StreamReceiver.cpp
	stream/VideoReceiver.cpp
	stream/AudioReceiver.cpp
	IController.cpp
	Controller.cpp
	Service.cpp
)

IF(APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
ENDIF()

FIND_PACKAGE(Threads)

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_AV_UTIL libavutil.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_AV_CODEC libavcodec.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_AV_FILTER libavfilter.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_AV_FORMAT libavformat.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_AV_DEVICE libavdevice.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_SW_SCALE libswscale.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_SW_RESAMPLE libswresample.so ${THIRD_PARTY_BIN})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_AV_CODEC libavcodec.dylib ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_AV_FORMAT libavformat.dylib ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_SW_SCALE libswscale.dylib ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_SW_RESAMPLE libswresample.dylib ${THIRD_PARTY_BIN})
ELSE()
	FIND_LIBRARY(LIB_AV_UTIL avutil ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_AV_CODEC avcodec ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_AV_FILTER avfilter ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_AV_FORMAT avformat ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_AV_DEVICE avdevice ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_SW_SCALE swscale ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_SW_RESAMPLE swresample ${THIRD_PARTY_LIB})
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES PREFIX "")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${LIB_AV_UTIL}
		debug ${LIB_AV_CODEC}
		debug ${LIB_AV_FILTER}
		debug ${LIB_AV_FORMAT}
		debug ${LIB_AV_DEVICE}
		debug ${LIB_SW_SCALE}
		debug ${LIB_SW_RESAMPLE}
		debug Threads::Threads
		$<$<BOOL:${WIN32}>:wsock32>
		$<$<BOOL:${WIN32}>:ws2_32>
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${LIB_AV_UTIL}
		optimized ${LIB_AV_CODEC}
		optimized ${LIB_AV_FILTER}
		optimized ${LIB_AV_FORMAT}
		optimized ${LIB_AV_DEVICE}
		optimized ${LIB_SW_SCALE}
		optimized ${LIB_SW_RESAMPLE}
		optimized Threads::Threads
		$<$<BOOL:${WIN32}>:wsock32>
		$<$<BOOL:${WIN32}>:ws2_32>
	)
ENDIF()

#-----------------------------------------------------
# DELIVERING binaries and header to ${LIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
    LIBRARY DESTINATION ${APP_OUTPUT_BIN}
    ARCHIVE DESTINATION ${APP_OUTPUT_LIB}
)

INSTALL(DIRECTORY ../include/ DESTINATION ${APP_OUTPUT_INC}/${PROJECT_NAME}
	FILES_MATCHING PATTERN "*.h"
)