#include "MgTut_EDMIO.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// edm4hep
#include "edm4hep/SimCalorimeterHitCollection.h"

// others
#include "TH1F.h"

DECLARE_COMPONENT(MgTut_EDMIO)

MgTut_EDMIO::MgTut_EDMIO(const std::string &aName, ISvcLocator *aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("CaloSimHits", m_caloHits,
                  "Simulated Hit Collection of Calorimeter");
  declareProperty("OffsetCaloSimHits", m_offsetCaloHits,
                  "Offset Hit Collection of Calorimeter");
}
MgTut_EDMIO::~MgTut_EDMIO() {}

StatusCode MgTut_EDMIO::initialize() {
  /// mandatory
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;

  /// services
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }

  /// histograms
  std::unique_ptr<TH1F> h1 = std::make_unique<TH1F>(
      "hOffsetCaloEdep", "Cell Eenergy Deposit + Offset (MeV)", 300, 0,
      30 + m_offset);
  if (m_histSvc->regHist("/tutorial/hOffsetCaloEdep", std::move(h1))
          .isFailure()) {
    error() << "Couldn't register histogram hOffsetCaloEdep" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode MgTut_EDMIO::execute() {
  /// fetch the histogram
  TH1 *h(nullptr);
  if (!m_histSvc->getHist("/tutorial/hOffsetCaloEdep", h).isSuccess()) {
    error() << "Couldn't retrieve hOffsetCaloEdep" << endmsg;
  }

  /// fetch the hit collection and fill
  auto offset_hits = m_offsetCaloHits.createAndPut();
  const auto calo_hits = m_caloHits.get();
  for (const auto &hit : *calo_hits) {
    // CAVEAT: the following code is incorrect
    // HINT: Ownership design & Value Semantics of edm4hep & Seperate I/O from
    // Memory Representation
    {
      // auto new_hit = offset_hits->create(); // new_hit is owned by
      // offset_hits when created
      // new_hit = hit.clone();                // it points to a cloned
      // (temporary) hit object after assignment
      // new_hit.setEnergy(m_offset*0.001 + hit.getEnergy()); // the temporary
      // hit object gets the new value
    }
    // the hit object owned by offset_hits keep the same value, i.e., the
    // default value

    // the correct way: start life as standalone, then pushed to the owning
    // collection
    auto new_hit = hit.clone();
    new_hit.setEnergy(m_offset * 0.001 + hit.getEnergy());
    offset_hits->push_back(new_hit); // offset_hits now owns the new_hit

    // fill the hist
    h->Fill(m_offset + 1000 * hit.getEnergy());
  }

  return StatusCode::SUCCESS;
}

StatusCode MgTut_EDMIO::finalize() { return GaudiAlgorithm::finalize(); }
