#include "SimFastSimPhysicsList.h"

// FCCSW
#include "SimKernel/FastSimPhysics.h"
#include "SimInterface/ISimPhysicsList.h"

// Geant4
#include "G4VModularPhysicsList.hh"

DECLARE_COMPONENT(SimFastSimPhysicsList)

SimFastSimPhysicsList::SimFastSimPhysicsList(const std::string& aType, const std::string& aName,
                                                 const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimPhysicsList>(this);
  declareProperty("fullphysics", m_physicsListTool, "Handle for the full physics list tool");
}

SimFastSimPhysicsList::~SimFastSimPhysicsList() {}

StatusCode SimFastSimPhysicsList::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_physicsListTool.retrieve().ignore();
  return StatusCode::SUCCESS;
}

StatusCode SimFastSimPhysicsList::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimFastSimPhysicsList::physicsList() {
  // ownership passed to SimSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  G4VModularPhysicsList* physicsList = m_physicsListTool->physicsList();
  // Coupled transportation enables calculation of particle trajectory in envelopes with fast sim models attached
  G4PhysicsListHelper::GetPhysicsListHelper()->UseCoupledTransportation();
  // Attach Fast Simulation Process (will take over normal transportation if FastSimModel triggered
  physicsList->RegisterPhysics(new sim::FastSimPhysics);
  return physicsList;
}
