#include "AlgTemplate.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// edm4hep
#include "edm4hep/SimCalorimeterHitCollection.h"

// others
#include "TH1F.h"

DECLARE_COMPONENT(AlgTemplate)

AlgTemplate::AlgTemplate(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("CaloSimHits", m_caloHits,
                  "Simulated Hit Collection of Calorimeter");
}
AlgTemplate::~AlgTemplate() {}

StatusCode AlgTemplate::initialize() {
  /// mandatory
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;

  /// services
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }

  // m_randSvc = service("RndmGenSvc");
  // if (!m_randSvc) {
  //   error() << "Unable to locate RndmGen Service" << endmsg;
  //   return StatusCode::FAILURE;
  // }

  /// histograms
  std::unique_ptr<TH1F> h1 = std::make_unique<TH1F>("hCaloEdep", "Cell Eenergy Deposit (MeV)", 300, 0, 30);
  if (m_histSvc->regHist("/tutorial/hCaloEdep", std::move(h1)).isFailure()) {
    error() << "Couldn't register histogram hCaloEdep" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode AlgTemplate::execute() {
  /// fetch the histogram
  TH1* h( nullptr );
  if ( !m_histSvc->getHist( "/tutorial/hCaloEdep", h ).isSuccess() ) {
    error() << "Couldn't retrieve hCaloEdep" << endmsg;
  }

  /// fetch the hit collection
  const auto calo_hits = m_caloHits.get();
  for (const auto& hit : *calo_hits) {
    h->Fill(1000*hit.getEnergy());
  }

  return StatusCode::SUCCESS;
}

StatusCode AlgTemplate::finalize() { return GaudiAlgorithm::finalize(); }
