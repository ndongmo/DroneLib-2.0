# DroneLib 2.0 C++ project

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

set(DRONELIB_HOME ${CMAKE_CURRENT_SOURCE_DIR})
set(DRONELIB_NAME Lib)
set(DRONELIB_INC ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(THIRD_PARTY_INC ${DRONELIB_HOME}/third-party/include)

IF(UNIX) # for Linux, BSD, Solaris, Minix
	IF (CMAKE_SIZEOF_VOID_P EQUAL 8) 	#x64
    set(THIRD_PARTY_BIN ${DRONELIB_HOME}/third-party/linux/x64/bin)
    set(THIRD_PARTY_LIB ${DRONELIB_HOME}/third-party/linux/x64/lib)
	ELSE() 	#x86
    set(THIRD_PARTY_BIN ${DRONELIB_HOME}/third-party/linux/x86/bin)
    set(THIRD_PARTY_LIB ${DRONELIB_HOME}/third-party/linux/x86/lib)
	ENDIF()
ELSEIF(APPLE)
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8) 	#x64
    set(THIRD_PARTY_BIN ${DRONELIB_HOME}/third-party/apple/x64/bin)
    set(THIRD_PARTY_LIB ${DRONELIB_HOME}/third-party/apple/x64/lib)
  ELSE() 	#x86
    set(THIRD_PARTY_BIN ${DRONELIB_HOME}/third-party/apple/x86/bin)
    set(THIRD_PARTY_LIB ${DRONELIB_HOME}/third-party/apple/x86/lib)
  ENDIF()
ELSE()
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8) 	#x64
    set(THIRD_PARTY_BIN ${DRONELIB_HOME}/third-party/win/x64/bin)
    set(THIRD_PARTY_LIB ${DRONELIB_HOME}/third-party/win/x64/lib)
  ELSE() 	#x86
    set(THIRD_PARTY_BIN ${DRONELIB_HOME}/third-party/win/x86/bin)
    set(THIRD_PARTY_LIB ${DRONELIB_HOME}/third-party/win/x86/lib)
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

SET(CMAKE_CXX_FLAGS "-Wall -Wno-overloaded-virtual -Wdeprecated-declarations -fmax-errors=5")
IF(APPLE)
  string(APPEND CMAKE_CXX_FLAGS " -Qunused-arguments")
ENDIF(APPLE)
SET(CMAKE_CXX_FLAGS_DEBUG " -Og -ggdb ${ENABLED_DEBUG_MESSAGE}")
SET(CMAKE_CXX_FLAGS_RELEASE " -O3  ${ENABLED_DEBUG_MESSAGE}")

add_subdirectory(src/lib)
add_subdirectory(src/drone)
add_subdirectory(src/pc)
#add_subdirectory(src/docs)
#if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
#    add_subdirectory(docs)
#endif()