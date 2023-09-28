# DroneLib 2.0 C++ project: PC controller package

PROJECT(${PC})

SET(SOURCE_FILES
	main.cpp
	EventHandler.cpp
	PCController.cpp
	PCSpeaker.cpp
	PCWindow.cpp
)

IF(APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
ENDIF()

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_SDL libSDL2.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf.so ${THIRD_PARTY_BIN})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_SDL libSDL2.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf.dylib ${THIRD_PARTY_LIB})
ELSE()
	FIND_LIBRARY(LIB_SDL SDL2 ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_TTF SDL2_ttf ${THIRD_PARTY_LIB})
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
	DEBUG_POSTFIX "d"
)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
	POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${PC_RSC}/arial.ttf
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${CLIENT}
		debug ${LIB_SDL}
		debug ${LIB_TTF}
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${CLIENT}
		optimized ${LIB_SDL}
		optimized ${LIB_TTF}
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${CLIENT})

#-----------------------------------------------------
# Create executable
#-----------------------------------------------------

SET(PCMain Pc-main)

IF (WIN32)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${THIRD_PARTY_BIN})
ENDIF()

ADD_EXECUTABLE(${PCMain} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PCMain} PROPERTIES 
	DEBUG_POSTFIX "d"
	OUTPUT_NAME ${PROJECT_NAME}
)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PCMain} debug ${PROJECT_NAME})
ELSE()
	TARGET_LINK_LIBRARIES(${PCMain} optimized ${PROJECT_NAME})
ENDIF()

#-----------------------------------------------------
# DELIVERING binaries and header to ${LIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)