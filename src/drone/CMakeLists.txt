# DroneLib 2.0 C++ project: Drone package

INCLUDE_DIRECTORIES(
	include
	${DRONELIB_INC}
	${THIRD_PARTY_INC}
)

SET(DRONE_TEST_LIB DroneTestLib)

add_subdirectory(src)
add_subdirectory(tests)