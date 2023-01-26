#include "SimGdmlDetector.h"

// FCCSW
#include "SimKernel/GdmlDetectorConstruction.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_COMPONENT(SimGdmlDetector)

SimGdmlDetector::SimGdmlDetector(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimDetectorConstruction>(this);
}

SimGdmlDetector::~SimGdmlDetector() {}

StatusCode SimGdmlDetector::initialize() { return AlgTool::initialize(); }
StatusCode SimGdmlDetector::finalize() { return AlgTool::finalize(); }

G4VUserDetectorConstruction* SimGdmlDetector::detectorConstruction() {
  return new sim::GdmlDetectorConstruction(m_gdmlFile, m_validate);
}
