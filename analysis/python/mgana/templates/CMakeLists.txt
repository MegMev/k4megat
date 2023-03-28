cmake_minimum_required(VERSION 3.16.9)
project(__pkgname__ CXX)
set(lib_name "MegatAnalyzer___pkgname__")

find_package(ROOT COMPONENTS ROOTVecOps ROOTDataFrame REQUIRED)
find_package(EDM4HEP REQUIRED)
find_package(k4megat REQUIRED)
find_package(DD4hep REQUIRED)

#--- Set a better default for installation directory---------------------------
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "__megatpath__" CACHE PATH "default install path" FORCE)
endif()

#--- Offer the user the choice of overriding the installation directories
set(MEGAT_DIR "__megatpath__" CACHE PATH
    "Installation directory for MegatAnalyzer framework")

#--- Find all paths to '__pkgname__' library pieces
file(GLOB headers "include/*.h")
file(GLOB sources "src/*.cc")
file(GLOB classes "src/classes.h")
file(GLOB reflex_sel "src/classes_def.xml")

#--- generate the ROOT dictionary using a REFLEX selection
set(CMAKE_ROOTTEST_NOROOTMAP OFF)
reflex_generate_dictionary(lib${lib_name} ${headers} ${classes}
  SELECTION ${reflex_sel}
)
#--- build the analyzer library
add_library(${lib_name} SHARED ${sources} ${headers})
target_include_directories(${lib_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                              $<INSTALL_INTERFACE:include>)
target_link_libraries(${lib_name} PUBLIC k4megat::MegatAnalysis
                                         EDM4HEP::edm4hep
                                         ROOT::ROOTVecOps)
set_target_properties(${lib_name} PROPERTIES PUBLIC_HEADER "${headers}")

target_sources(${lib_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/lib${lib_name}.cxx)

#--- install the built targets
install(TARGETS ${lib_name}
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" COMPONENT bin
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/__pkgname__"
        COMPONENT analyses)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${lib_name}.rootmap"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        COMPONENT analyses)
if(${ROOT_VERSION} GREATER 6)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${lib_name}_rdict.pcm"
          DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
          COMPONENT analyses)
endif()
message(STATUS "Built standalone analyser package: __pkgname__")
