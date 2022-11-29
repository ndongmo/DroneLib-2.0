# DroneLib 2.0 C++ project: PC controller package

PROJECT(PC)

SET(SOURCE_FILES
	main.cpp
	EventHandler.cpp
	PCController.cpp
	PCReceiver.cpp
	PCSender.cpp
	PCWindow.cpp
)

# Apple specific stuff
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

FIND_PACKAGE(Threads)

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_SDL libSDL2.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf.so ${THIRD_PARTY_BIN})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_SDL libSDL2.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf.dylib ${THIRD_PARTY_BIN})
ELSE()
	FIND_LIBRARY(LIB_SDL libSDL2 ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf ${THIRD_PARTY_LIB})
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
ADD_LIBRARY(${PCLIB_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
	DEBUG_POSTFIX "d"
)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
	POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../res/arial.ttf
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${DRONELIB_NAME}
		debug ${LIB_SDL}
		debug ${LIB_TTF}
		debug Threads::Threads
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${DRONELIB_NAME}
		optimized ${LIB_SDL}
		optimized ${LIB_TTF}
		optimized Threads::Threads
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${DRONELIB_NAME})

#-----------------------------------------------------
# DELIVERING binaries and header to ${DRONELIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)