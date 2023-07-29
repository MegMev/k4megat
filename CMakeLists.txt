CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

project(k4megat)


#---------------------------------------------------------------
OPTION(BUILD_DOCS "Build documentation" ON)
if(BUILD_DOCS)
  add_subdirectory(doc)
endif()

#---------------------------------------------------------------
# external dependencies
# Load macros and functions for Gaudi-based projects
find_package(Gaudi)
set(GAUDI_GENCONF_DIR "genConfDir")
if (${Gaudi_VERSION} VERSION_LESS 35.1)
  set(GAUDI_GENCONF_DIR "genConf")
endif()

find_package(DD4hep)
find_package(ROOT)
find_package(EDM4HEP)
find_package(Geant4)
find_package(CLHEP)

# core package maybe built as subproject
option(USE_EXTERNAL_K4CORE "Use external k4FWCore, make it exists" ON)
mark_as_advanced(USE_EXTERNAL_K4CORE)
if(USE_EXTERNAL_K4CORE)
  find_package(k4FWCore)
else()
  find_path(CPPGSL_INCLUDE_DIR
    NAMES gsl/gsl
    HINTS $ENV{CPPGSL_ROOT_DIR} ${CPPGSL_ROOT_DIR})
endif()

# dd4hep provides some useful compiling flags (esp. rpath)
dd4hep_set_compiler_flags()

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

# This mandatory
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# cmake modules
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )
include(MegatMacro)
# Includes githooks, now using clang-format
include(GitHooks)

#---------------------------------------------------------------
add_subdirectory(k4libs)
add_subdirectory(detector)
add_subdirectory(sim)
add_subdirectory(digi)
add_subdirectory(analysis)

#########################
# Configure and install #
#########################
configure_file(cmake/thismegat.sh bin/thismegat.sh @ONLY)
install(FILES
  ${CMAKE_BINARY_DIR}/bin/thismegat.sh
  DESTINATION bin
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
)

configure_file(cmake/thismegat_only.sh bin/thismegat_only.sh @ONLY)
install(FILES
  ${CMAKE_BINARY_DIR}/bin/thismegat_only.sh
  DESTINATION bin
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
)


install(EXPORT ${PROJECT_NAME}
  NAMESPACE ${PROJECT_NAME}::
  FILE "${PROJECT_NAME}Targets.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/"
  )

gaudi_install(CMAKE cmake/${PROJECT_NAME}Config.cmake)

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
