#include "SimDD4hepDetector.h"

// FCCSW
#include "SimInterface/IGeoSvc.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_COMPONENT(SimDD4hepDetector)

SimDD4hepDetector::SimDD4hepDetector(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : GaudiTool(aType, aName, aParent), m_geoSvc("GeoSvc", aName) {
  declareInterface<ISimDetectorConstruction>(this);
  declareProperty("GeoSvc", m_geoSvc);
}

SimDD4hepDetector::~SimDD4hepDetector() {}

StatusCode SimDD4hepDetector::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimDD4hepDetector::finalize() { return GaudiTool::finalize(); }

G4VUserDetectorConstruction* SimDD4hepDetector::detectorConstruction() { return m_geoSvc->getGeant4Geo(); }
