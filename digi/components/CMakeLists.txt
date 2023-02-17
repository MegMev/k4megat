################################################################################
# Package: DigiComponents
################################################################################

set(sources)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
# set(SUBDIRS actions core fast field generator output physics studies tools regions)
foreach(subdir ${SUBDIRS})
  file(GLOB _lib_sources ${subdir}/*.cpp)
  list(APPEND sources "${_lib_sources}")
endforeach()

gaudi_add_module(DigiComponents
                 SOURCES ${sources}
                 LINK Gaudi::GaudiAlgLib
                      Gaudi::GaudiKernel
                      DD4hep::DDCore
                      DD4hep::DDRec
                      ROOT::Core
                      ROOT::Hist
                      EDM4HEP::edm4hep
                      k4FWCore::k4FWCore
                      SimInterface
                    )
