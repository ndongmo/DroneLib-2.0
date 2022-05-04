# DroneLib 2.0 C++ project: Lib package documentation

PROJECT(DroneLib-docs)

#-----------------------------------------------------
# DELIVERING binaries and header to ${DRONELIB_HOME}
#-----------------------------------------------------

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/config.dox)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/config)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Lib documentation with Doxygen"
        VERBATIM )
endif (DOXYGEN_FOUND)

INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc_doxygen/ DESTINATION ${DRONELIB_HOME}/app/docs/)