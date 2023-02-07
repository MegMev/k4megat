find_package(Doxygen REQUIRED)
if(DOXYGEN_FOUND)
  configure_file(Doxyfile.in Doxyfile @ONLY)
  ### make doc explicitly to build documentation
  add_custom_target(doc
                    ${DOXYGEN_EXECUTABLE} Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  # add_custom_target(doc ALL
  #                   ${DOXYGEN_EXECUTABLE} Doxyfile
  #                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  #                   COMMENT "Generating API documentation with Doxygen" VERBATIM)
  # install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen
  #   DESTINATION ${CMAKE_INSTALL_PREFIX}/
  #   )
endif()
