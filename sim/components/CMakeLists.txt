################################################################################
# Package: SimComponents
################################################################################

set(sources)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
# set(SUBDIRS actions core fast field generator output physics studies tools regions)
foreach(subdir ${SUBDIRS})
  file(GLOB _lib_sources ${subdir}/*.cpp)
  list(APPEND sources "${_lib_sources}")
  message(STATUS "${subdir}")
endforeach()

gaudi_add_module(SimComponents
                 SOURCES ${sources}
                 LINK Gaudi::GaudiAlgLib
                      DD4hep::DDCore
                      DD4hep::DDG4
                      ${Geant4_LIBRARIES}
                      CLHEP::CLHEP
                      EDM4HEP::edm4hep
                      k4FWCore::k4FWCore
                      SimInterface
                      SimKernel
                      SimSensitive
                    )
