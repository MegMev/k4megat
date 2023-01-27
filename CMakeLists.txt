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
set(GAUDI_GENCONF_DIR "genConfDir")
if (${Gaudi_VERSION} VERSION_LESS 35.1)
  set(GAUDI_GENCONF_DIR "genConf")
endif()

find_package(DD4hep)
dd4hep_set_compiler_flags()
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR DD4HEP_BUILD_TEST) AND TEST)
  dd4hep_enable_tests()
endif()

find_package(podio)
find_package(EDM4HEP)
find_package(CLHEP)

set(MEGAT_CORE k4FWCore)
option(USE_EXTERNAL_K4CORE "Use external k4FWCore, make it exists" ON)
mark_as_advanced(USE_EXTERNAL_K4CORE)
if(USE_EXTERNAL_K4CORE)
  find_package(k4FWCore)
  find_package(k4SimGeant4)
  set(MEGAT_CORE k4FWCore::k4FWCore)
endif()

#---------------------------------------------------------------
include(GNUInstallDirs)
include(CTest)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/InstallArea/ CACHE PATH
    "Install path prefix, prepended onto install directories." FORCE )
endif()

# Set up C++ Standard
# ``-DCMAKE_CXX_STANDARD=<standard>`` when invoking CMake
set(CMAKE_CXX_STANDARD "17" CACHE STRING "")

if(NOT CMAKE_CXX_STANDARD MATCHES "14|17")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
endif()

# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(cmake/MegatMacro.cmake)

add_subdirectory(k4libs)
add_subdirectory(detector)
add_subdirectory(sim)
add_subdirectory(mypackage)

#########################
# Configure and install #
#########################
configure_file(cmake/thismegat.sh       bin/thismegat.sh       @ONLY)
install(FILES    ${CMAKE_BINARY_DIR}/bin/thismegat.sh DESTINATION bin )
configure_file(cmake/thismegat_only.sh       bin/thismegat_only.sh       @ONLY)
install(FILES    ${CMAKE_BINARY_DIR}/bin/thismegat_only.sh DESTINATION bin )


# install(EXPORT ${PROJECT_NAME}Targets
#   NAMESPACE ${PROJECT_NAME}::
#   FILE "${PROJECT_NAME}Targets.cmake"
#   DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/"
#   )

# gaudi_install(CMAKE cmake/${PROJECT_NAME}Config.cmake)

#########################
# Test #
#########################
# if(BUILD_TESTING)
#   add_subdirectory(test)
# endif()

#########################
# Packing #
#########################
# option(ENABLE_CPACK "Whether or not to use cpack config" OFF)
# if(ENABLE_CPACK)
#   include(cmake/${PROJECT_NAME}CPack.cmake)
# endif()
