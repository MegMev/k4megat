#include "SimFullSimActions.h"

// FCCSW
#include "SimKernel/FullSimActions.h"

DECLARE_COMPONENT(SimFullSimActions)

SimFullSimActions::SimFullSimActions(const std::string& type, const std::string& name, const IInterface* parent)
    : AlgTool(type, name, parent) {
  declareInterface<ISimActionTool>(this);
}

SimFullSimActions::~SimFullSimActions() {}

StatusCode SimFullSimActions::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimFullSimActions::finalize() { return AlgTool::finalize(); }

G4VUserActionInitialization* SimFullSimActions::userActionInitialization() {
  return new sim::FullSimActions(m_enableHistory, m_energyCut);
}
