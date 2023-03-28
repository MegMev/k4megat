# package name
set(_pkg_name MegatAnalysis)

# Build the library
file(GLOB sources src/*.cpp)
gaudi_add_library(${_pkg_name}
  SOURCES ${sources}
  LINK PUBLIC
  ROOT::Physics
  ROOT::MathCore
  ROOT::ROOTVecOps
  ROOT::ROOTDataFrame
  EDM4HEP::edm4hep
  podio::podio
  DD4hep::DDCore
)

# ROOT dictionaries
gaudi_add_dictionary(${_pkg_name}Dict
  HEADERFILES dict/analysis_dict.h
  SELECTION dict/analysis_selection.xml
  LINK ${_pkg_name}
)

# Install python modules
gaudi_install(PYTHON)
gaudi_install(SCRIPTS bin/)
