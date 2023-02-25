# DroneLib 2.0 C++ project: PC Tests package

PROJECT(PC-tests)

SET(SOURCE_FILES
	main.cpp
	EventHandlerTest.cpp
	PCReceiverTest.cpp
	PCSenderTest.cpp
	PCWindowTest.cpp
	PCControllerTest.cpp
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
	FIND_LIBRARY(LIB_SDL libSDL2.so ${THIRD_PARTY_BIN})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf.so ${THIRD_PARTY_BIN})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_GTEST libgtest.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK libgmock.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_SDL libSDL2.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf.dylib ${THIRD_PARTY_BIN})
ELSE()
	IF(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		FIND_LIBRARY(LIB_GTEST libgtest.a ${THIRD_PARTY_LIB})
		FIND_LIBRARY(LIB_GMOCK libgmock.a ${THIRD_PARTY_LIB})
		FIND_LIBRARY(LIB_SDL libSDL2.a ${THIRD_PARTY_LIB})
		FIND_LIBRARY(LIB_TTF libSDL2_ttf.a ${THIRD_PARTY_LIB})
	ELSE()
		FIND_LIBRARY(LIB_GTEST gtest ${THIRD_PARTY_LIB})
		FIND_LIBRARY(LIB_GMOCK gmock ${THIRD_PARTY_LIB})
		FIND_LIBRARY(LIB_SDL SDL2 ${THIRD_PARTY_LIB})
		FIND_LIBRARY(LIB_TTF SDL2_ttf ${THIRD_PARTY_LIB})
	ENDIF()
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
	POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${PC_RSC}/arial.ttf
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${LIB}
		debug ${PCLIB}
		debug ${LIB_GTEST}
		debug ${LIB_GMOCK}
		debug ${LIB_SDL}
		debug ${LIB_TTF}
		debug Threads::Threads
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${LIB}
		optimized ${PCLIB}
		optimized ${LIB_GTEST}
		optimized ${LIB_GMOCK}
		optimized ${LIB_SDL}
		optimized ${LIB_TTF}
		optimized Threads::Threads
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${PCLIB})
add_dependencies(${PROJECT_NAME} ${LIB})

INCLUDE(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} EXTRA_ARGS ARGS=-V)

#-----------------------------------------------------
# DELIVERING binaries and header to ${LIB_HOME}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)