# DroneLib 2.0 C++ project: Lib package

INCLUDE_DIRECTORIES(
	include
	${THIRD_PARTY_INC}
)

add_subdirectory(src)

IF(NOT ANDROID)
	add_subdirectory(tests)
ENDIF()