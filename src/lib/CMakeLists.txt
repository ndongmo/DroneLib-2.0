# DroneLib 2.0 C++ project: Lib package

add_subdirectory(src)
add_subdirectory(test)
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    add_subdirectory(docs)
endif()