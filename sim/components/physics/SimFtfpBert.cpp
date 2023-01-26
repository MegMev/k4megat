#include "SimFtfpBert.h"

// Geant4
#include "FTFP_BERT.hh"
#include "G4VModularPhysicsList.hh"

DECLARE_COMPONENT(SimFtfpBert)

SimFtfpBert::SimFtfpBert(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimPhysicsList>(this);
}

SimFtfpBert::~SimFtfpBert() {}

StatusCode SimFtfpBert::initialize() { return AlgTool::initialize(); }

StatusCode SimFtfpBert::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimFtfpBert::physicsList() {
  // ownership passed to SimSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new FTFP_BERT(m_G4PhysicsVerbosityLevel);
}
