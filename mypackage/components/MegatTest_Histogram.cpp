#include "MegatTest_Histogram.h"

// FCCSW
#include "GaudiKernel/ITHistSvc.h"

// datamodel
#include "MegatTest_Histogram.h"
#include "edm4hep/SimCalorimeterHitCollection.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "TH1F.h"

DECLARE_COMPONENT(MegatTest_Histogram)

MegatTest_Histogram::MegatTest_Histogram(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("caloSimHits", m_calorHits,
                  "Simulated Hit Collection of Calorimeter");
}
MegatTest_Histogram::~MegatTest_Histogram() {}

StatusCode MegatTest_Histogram::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }

  m_edep = new TH1F("CaloEdep", "Cell Eenergy Deposit (MeV)", 300, 0, 30);
  if (m_histSvc->regHist("/sim/CaloEdep", m_edep).isFailure()) {
    error() << "Couldn't register CaloEdep histogram" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode MegatTest_Histogram::execute() {
  const auto calo_hits = m_calorHits.get();
  for (const auto& hit : *calo_hits) {
    // auto mom_edm = hit.getRec().getMomentum();
    // CLHEP::Hep3Vector mom(mom_edm.x, mom_edm.y, mom_edm.z);
    // m_eta->Fill(mom.eta());
    m_edep->Fill(1000*hit.getEnergy());
  }
  return StatusCode::SUCCESS;
}

StatusCode MegatTest_Histogram::finalize() { return GaudiAlgorithm::finalize(); }
