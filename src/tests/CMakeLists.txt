# DroneLib 2.0 C++ project: Integration tests package

PROJECT(Integration-tests)

INCLUDE_DIRECTORIES(
	include
	${PC_INC}
	${DRONE_INC}
	${LIB_INC}
	${THIRD_PARTY_INC}
)

SET(SOURCE_FILES
	main.cpp
	IntegrationTest.cpp
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
	FIND_LIBRARY(LIB_GTEST libgtest ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_GMOCK libgmock ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_SDL libSDL2 ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_TTF libSDL2_ttf ${THIRD_PARTY_LIB})
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
		debug ${DRONE_LIB}
		debug ${LIB_GTEST}
		debug ${LIB_GMOCK}
		debug ${LIB_SDL}
		debug ${LIB_TTF}
		debug ${LIB_WS2811}
		debug i2c
		debug Threads::Threads
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${LIB}
		optimized ${PCLIB}
		optimized ${DRONE_LIB}
		optimized ${LIB_GTEST}
		optimized ${LIB_GMOCK}
		optimized ${LIB_SDL}
		optimized ${LIB_TTF}
		optimized ${LIB_WS2811}
		optimized i2c
		optimized Threads::Threads
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${LIB})
add_dependencies(${PROJECT_NAME} ${PCLIB})
add_dependencies(${PROJECT_NAME} ${DRONE_LIB})

INCLUDE(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} EXTRA_ARGS ARGS=-V)

#-----------------------------------------------------
# DELIVERING binaries and header to ${APP_OUTPUT_BIN}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)