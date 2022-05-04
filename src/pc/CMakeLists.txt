# DroneLib 2.0 C++ project: PC package

INCLUDE_DIRECTORIES(
	include
	${DRONELIB_INC}
	${THIRD_PARTY_INC}
)

SET(PCLIB_NAME PCLib)

add_subdirectory(src)
add_subdirectory(tests)