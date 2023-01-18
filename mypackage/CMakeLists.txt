if(NOT TARGET k4FWCore)
  message(FATAL_ERROR "Can't find k4FWCore")
endif()

if(NOT TARGET k4FWCorePlugins)
  message(FATAL_ERROR "Can't find k4FWCorePlugins")
endif()

if(NOT TARGET k4Interface)
  message(FATAL_ERROR "Can't find k4Interface")
endif()


file(GLOB _module_sources components/*.cpp)
gaudi_add_module(MegatTestPlugins
  SOURCES ${_module_sources}
  LINK Gaudi::GaudiAlgLib EDM4HEP::edm4hep k4FWCore)
