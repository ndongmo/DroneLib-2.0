# DroneLib 2.0 C++ project

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

SET(LIB_HOME ${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB Lib)
SET(CLIENT Client)
SET(PC Pc)
SET(DRONE Drone)

SET(PC_RSC ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/res)
SET(PC_INC ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/include)
SET(CLIENT_INC ${CMAKE_CURRENT_SOURCE_DIR}/src/client/include)
SET(LIB_INC ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/include)
SET(DRONE_INC ${CMAKE_CURRENT_SOURCE_DIR}/src/drone/include)

SET(THREADS_PREFER_PTHREAD_FLAG ON)

SET(APP_OUTPUT_BIN ${LIB_HOME}/app/bin)
SET(APP_OUTPUT_LIB ${LIB_HOME}/app/lib)
SET(APP_OUTPUT_INC ${LIB_HOME}/app/include)
SET(APP_OUTPUT_DOC ${LIB_HOME}/app/docs)
SET(THIRD_PARTY_INC ${LIB_HOME}/third-party/include)

IF(UNIX) # for Linux, BSD, Solaris, Minix
  IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    SET(RASPBERRY_PI_SYS 1)
    add_definitions(-DRASPBERRY_PI_SYS)
  ENDIF()
	IF (CMAKE_SIZEOF_VOID_P EQUAL 8) 	#x64
    SET(THIRD_PARTY_BIN ${LIB_HOME}/third-party/linux/x64/bin)
    SET(THIRD_PARTY_LIB ${LIB_HOME}/third-party/linux/x64/lib)
	ELSE() 	#x86
    SET(THIRD_PARTY_BIN ${LIB_HOME}/third-party/linux/x86/bin)
    SET(THIRD_PARTY_LIB ${LIB_HOME}/third-party/linux/x86/lib)
	ENDIF()
ELSEIF(APPLE)
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8) 	#x64
    SET(THIRD_PARTY_BIN ${LIB_HOME}/third-party/apple/x64/bin)
    SET(THIRD_PARTY_LIB ${LIB_HOME}/third-party/apple/x64/lib)
  ELSE() 	#x86
    SET(THIRD_PARTY_BIN ${LIB_HOME}/third-party/apple/x86/bin)
    SET(THIRD_PARTY_LIB ${LIB_HOME}/third-party/apple/x86/lib)
  ENDIF()
ELSE()
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8) 	#x64
    SET(THIRD_PARTY_BIN ${LIB_HOME}/third-party/win/x64/bin)
    SET(THIRD_PARTY_LIB ${LIB_HOME}/third-party/win/x64/lib)
  ELSE() 	#x86
    SET(THIRD_PARTY_BIN ${LIB_HOME}/third-party/win/x86/bin)
    SET(THIRD_PARTY_LIB ${LIB_HOME}/third-party/win/x86/lib)
  ENDIF()
ENDIF()

enable_testing() # needed on top-level CMakeLists.txt

IF(NOT CMAKE_BUILD_TYPE) 
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_DEBUG_POSTFIX d)

SET(CMAKE_CXX_STANDARD 11)

IF(("$ENV{DEBUG_MESSAGE_DRONELIB}" EQUAL "1") AND (CMAKE_BUILD_TYPE MATCHES Debug))
	SET(ENABLED_DEBUG_MESSAGE " ")
ELSE()
	SET(ENABLED_DEBUG_MESSAGE " -DNDEBUG")
ENDIF()

IF(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  SET(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -Wno-overloaded-virtual -Wdeprecated-declarations -fmax-errors=5")
  SET(CMAKE_CXX_FLAGS_DEBUG " -Og -ggdb ${ENABLED_DEBUG_MESSAGE}")
  SET(CMAKE_CXX_FLAGS_RELEASE " -O3  ${ENABLED_DEBUG_MESSAGE}")
ENDIF()

IF(APPLE)
  string(APPEND CMAKE_CXX_FLAGS " -Qunused-arguments")
ENDIF(APPLE)

add_subdirectory(src/lib)

IF(UNIX)
  add_subdirectory(src/drone)
ENDIF()

IF(NOT RASPBERRY_PI_SYS)
  add_subdirectory(src/client)
  add_subdirectory(src/pc)
ENDIF()

IF(UNIX AND NOT RASPBERRY_PI_SYS)
  add_subdirectory(src/tests)
ENDIF()

IF(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
  add_subdirectory(src/docs)
ENDIF()