# DroneLib 2.0 C++ project: PC package

INCLUDE_DIRECTORIES(
	include
	${LIB_INC}
	${CLIENT_INC}
	${THIRD_PARTY_INC}
)

add_subdirectory(src)
add_subdirectory(tests)