CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

project(k4megat)

#---------------------------------------------------------------
# Find external dependencies

find_package(Gaudi)
find_package(DD4hep)
find_package(ROOT)
find_package(Geant4)
find_package(CLHEP)
find_package(podio 0.17.2)
find_package(EDM4HEP 0.10.1)
find_package(k4FWCore)

set(CMAKE_CXX_STANDARD "17" CACHE STRING "")
if(NOT CMAKE_CXX_STANDARD MATCHES "17")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/InstallArea/ CACHE PATH
    "Install path prefix, prepended onto install directories." FORCE )
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#---------------------------------------------------------------
# Include cmake modules
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )
include(MegatMacro)
include(GitHooks)

include(GNUInstallDirs)
include(CTest)

#---------------------------------------------------------------
# Codebase build

add_subdirectory(detector)
add_subdirectory(sim)
add_subdirectory(digi)
add_subdirectory(analysis)

OPTION(BUILD_DOCS "Build documentation" ON)
if(BUILD_DOCS)
  add_subdirectory(doc)
endif()


#---------------------------------------------------------------
# Configure and install

configure_file(cmake/thismegat.sh bin/thismegat.sh @ONLY)
configure_file(cmake/thismegat_only.sh bin/thismegat_only.sh @ONLY)
configure_file(cmake/thismegat_env.sh bin/thismegat_env.sh @ONLY)
configure_file(cmake/thismegat_internal.sh bin/thismegat_internal.sh @ONLY)
file(GLOB files ${CMAKE_BINARY_DIR}/bin/thismegat*.sh)
install(FILES
  ${files}
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
