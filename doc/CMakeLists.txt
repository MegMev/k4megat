find_package(Doxygen REQUIRED)
if(DOXYGEN_FOUND)
  message(STATUS "Build doc")
  configure_file(Doxyfile.in Doxyfile @ONLY)
  add_custom_target(doc
                    ${DOXYGEN_EXECUTABLE} Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen
    DESTINATION ${CMAKE_INSTALL_PREFIX}/
    )
endif()
