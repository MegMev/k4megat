file(GLOB _module_sources components/*.cpp)
gaudi_add_module(MegatTestPlugins
  SOURCES ${_module_sources}
  LINK Gaudi::GaudiAlgLib EDM4HEP::edm4hep CLHEP::CLHEP
       k4FWCore::k4FWCore)
