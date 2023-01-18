CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

project(k4megat)

message(STATUS "$ENV{CPPGSL_ROOT_DIR}")
find_path(CPPGSL_INCLUDE_DIR
  NAMES gsl/gsl
  HINTS $ENV{CPPGSL_ROOT_DIR} ${CPPGSL_ROOT_DIR})
message(STATUS "cpp: ${CPPGSL_INCLUDE_DIR}")
#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(Gaudi)
find_package(DD4hep)
dd4hep_set_compiler_flags()
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR DD4HEP_BUILD_TEST) AND TEST)
  dd4hep_enable_tests()
endif()

find_package(podio)
find_package(EDM4HEP)
#---------------------------------------------------------------
include(GNUInstallDirs)
include(CTest)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/InstallArea/ CACHE PATH
    "Install path prefix, prepended onto install directories." FORCE )
endif()

# Set up C++ Standard
# ``-DCMAKE_CXX_STANDARD=<standard>`` when invoking CMake
set(CMAKE_CXX_STANDARD ${GAUDI_CXX_STANDARD} CACHE STRING "")

if(NOT CMAKE_CXX_STANDARD MATCHES "14|17")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
endif()

include(cmake/MegatMacro.cmake)

add_subdirectory(mypackage)
add_subdirectory(detector)

# if(BUILD_TESTING)
#   add_subdirectory(test)
# endif()

# option(ENABLE_CPACK "Whether or not to use cpack config" OFF)
# if(ENABLE_CPACK)
#   include(cmake/${PROJECT_NAME}CPack.cmake)
# endif()

#########################
# Configure and install #
#########################
configure_file(cmake/thismegat.sh       bin/thismegat.sh       @ONLY)
install(FILES    ${CMAKE_BINARY_DIR}/bin/thismegat.sh DESTINATION bin )


# install(EXPORT ${PROJECT_NAME}Targets
#   NAMESPACE ${PROJECT_NAME}::
#   FILE "${PROJECT_NAME}Targets.cmake"
#   DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/"
#   )

# gaudi_install(CMAKE cmake/${PROJECT_NAME}Config.cmake)

######## geometry files #############
install(DIRECTORY ${CMAKE_SOURCE_DIR}/geometry/
  DESTINATION geometry
  COMPONENT install
)

#########################################################
# message(STATUS ${ROOT_DIR})
# message(STATUS "gaudi_dir: ${Gaudi_DIR}")
# message(STATUS "gaudi_root: ${Gaudi_ROOT}")
# message(STATUS "gaudi bin dir: ${Gaudi_BINARY_DIR}")
# message(STATUS "gaudi inc dir: ${Gaudi_INCLUDE_DIR}")
# message(STATUS "gaudi lib dir: ${Gaudi_LIBRARY_DIR}")
# message(STATUS "gaudi plg dir: ${Gaudi_PLUGINS_DIR}")
# message(STATUS "gaudi pyt dir: ${Gaudi_PYTHON_DIR}")


# message(STATUS "dd4hep_dir: ${DD4hep_DIR}")
# message(STATUS "dd4hep_include_dir: ${DD4hep_INCLUDE_DIRS}")
# message(STATUS "dd4hep lib dir: ${DD4hep_LIBRARIES}")


message(STATUS "podio_dir: ${podio_DIR}")
message(STATUS "podio_include_dir: ${podio_INCLUDE_DIR}")
message(STATUS "podio lib dir: ${podio_LIBRARY_DIR}")
message(STATUS "podio py dir: ${podio_PYTHON_DIR}")
