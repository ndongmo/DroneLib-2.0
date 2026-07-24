# DroneLib 2.0 C++ project: Client package

INCLUDE_DIRECTORIES(
	include
	${LIB_INC}
	${THIRD_PARTY_INC}
)

add_subdirectory(src)

IF(NOT ANDROID)
	add_subdirectory(tests)
ENDIF()