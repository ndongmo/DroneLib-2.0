# DroneLib 2.0 C++ project: Drone package

PROJECT(${DRONE})

SET(SOURCE_FILES
	main.cpp
	DroneController.cpp
	DroneReceiver.cpp
	DroneSender.cpp
	controller/LedController.cpp
	controller/ServoController.cpp
	controller/MotorController.cpp
	controller/BuzzerController.cpp
	controller/BatteryController.cpp
	controller/ComponentController.cpp
	hardware/I2C.cpp
)

IF(APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
ENDIF()

IF(UNIX AND NOT APPLE) # for Linux, BSD, Solaris, Minix
	FIND_LIBRARY(LIB_WS2811 libws2811.a ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_WiringPi libwiringPi.so ${THIRD_PARTY_BIN})
ELSEIF(APPLE)
	FIND_LIBRARY(LIB_WS2811 libws2811.dylib ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_WiringPi libwiringPi.dylib ${THIRD_PARTY_BIN})
ELSE()
	FIND_LIBRARY(LIB_WS2811 libws2811 ${THIRD_PARTY_LIB})
	FIND_LIBRARY(LIB_WiringPi libwiringPi ${THIRD_PARTY_BIN})
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		debug ${LIB}
		debug ${LIB_WS2811}
		debug ${LIB_WiringPi}
		debug i2c
	)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		optimized ${LIB}
		optimized ${LIB_WS2811}
		optimized ${LIB_WiringPi}
		optimized i2c
	)
ENDIF()

add_dependencies(${PROJECT_NAME} ${LIB})

#-----------------------------------------------------
# Create executable
#-----------------------------------------------------

SET(DroneMain Drone-main)

ADD_EXECUTABLE(${DroneMain} ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${DroneMain} PROPERTIES 
	DEBUG_POSTFIX "d"
	OUTPUT_NAME ${PROJECT_NAME}
)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES(${DroneMain} debug ${PROJECT_NAME})
ELSE()
	TARGET_LINK_LIBRARIES(${DroneMain} optimized ${PROJECT_NAME})
ENDIF()

#-----------------------------------------------------
# DELIVERING binaries and header to ${APP_OUTPUT_BIN}
#-----------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${APP_OUTPUT_BIN}
)