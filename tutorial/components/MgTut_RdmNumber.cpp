#include "MgTut_RdmNumber.h"

// Gaudi
#include "GaudiKernel/RndmGenerators.h"

#include "TH1F.h"

// plugin component
DECLARE_COMPONENT(MgTut_RdmNumber)

MgTut_RdmNumber::MgTut_RdmNumber(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc) {
}
MgTut_RdmNumber::~MgTut_RdmNumber() {}

StatusCode MgTut_RdmNumber::initialize() {
  // this line is mandatory
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;

  // fetch services
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }

  m_randSvc = service("RndmGenSvc");
  if (!m_randSvc) {
    error() << "Unable to locate RndmGen Service" << endmsg;
    return StatusCode::FAILURE;
  }

  // register histograms
  std::unique_ptr<TH1F> h1 = std::make_unique<TH1F>( m_name.value().data(), "Gaussian Distribution", 100, m_mean-10*m_sigma, m_mean+10*m_sigma);
  if ( m_histSvc->regHist( "/tutorial"+m_dir+"/"+m_name, std::move(h1) ).isFailure() ) {
    error() << "Couldn't register " << m_name << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MgTut_RdmNumber::execute() {
  Rndm::Numbers gauss( m_randSvc, Rndm::Gauss( m_mean, m_sigma ) );

  TH1* h( nullptr );
  if ( m_histSvc->getHist( "/tutorial"+m_dir+"/"+m_name, h ).isSuccess() ) {
    h->Fill(gauss());
  } else {
    error() << "Couldn't retrieve " << m_name << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode MgTut_RdmNumber::finalize() { return GaudiAlgorithm::finalize(); }
