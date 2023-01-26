#include "SimFastSimActions.h"

// FCCSW
#include "SimKernel/FastSimActions.h"

DECLARE_COMPONENT(SimFastSimActions)

SimFastSimActions::SimFastSimActions(const std::string& type, const std::string& name, const IInterface* parent)
    : AlgTool(type, name, parent) {
  declareInterface<ISimActionTool>(this);
}

SimFastSimActions::~SimFastSimActions() {}

StatusCode SimFastSimActions::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimFastSimActions::finalize() { return AlgTool::finalize(); }

G4VUserActionInitialization* SimFastSimActions::userActionInitialization() { return new sim::FastSimActions(); }
