################################################################################
# Package: SimKernel
################################################################################

file(GLOB _lib_sources src/*.cpp)
gaudi_add_library(SimKernel
                 SOURCES ${_lib_sources}
		 LINK Gaudi::GaudiAlgLib
                      DD4hep::DDCore
                      DD4hep::DDG4
                      ${Geant4_LIBRARIES}
                      EDM4HEP::edm4hep
                      SimInterface
                 )
