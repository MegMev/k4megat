################################################################################
# Package: ReconComponents
################################################################################

file(GLOB sources *.cpp)
gaudi_add_module(ReconComponents
  SOURCES ${sources}
  LINK Gaudi::GaudiAlgLib
       ROOT::MathCore
       ROOT::Smatrix
       k4FWCore::k4FWCore
       EDM4HEP::edm4hep
       k4megat::MegatAnalysis
       Boost::headers
       mlpack::mlpack
       OpenMP::OpenMP_CXX
)
