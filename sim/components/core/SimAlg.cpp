#include "SimAlg.h"

// FCCSW
#include "SimInterface/ISimSvc.h"

// Geant
#include "G4Event.hh"

DECLARE_COMPONENT(SimAlg)

SimAlg::SimAlg(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc),
m_geantSvc("SimSvc", aName) {
  declareProperty("eventProvider", m_eventTool, "Handle for tool that creates the G4Event");
}
SimAlg::~SimAlg() {}

StatusCode SimAlg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  if (!m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }
  for (auto& toolname : m_saveToolNames) {
    m_saveTools.push_back(tool<ISimSaveOutputTool>(toolname));
    // FIXME: check StatusCode once the m_saveTools is a ToolHandleArray
    // if (!) {
    //   error() << "Unable to retrieve the output saving tool." << endmsg;
    //   return StatusCode::FAILURE;
    // }
  }
  if (!m_eventTool.retrieve()) {
    error() << "Unable to retrieve the G4Event provider " << m_eventTool << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimAlg::execute() {
  // first translate the event
  G4Event* event = m_eventTool->g4Event();

  if (!event) {
    error() << "Unable to retrieve G4Event from " << m_eventTool << endmsg;
    return StatusCode::FAILURE;
  }
  m_geantSvc->processEvent(*event).ignore();
  G4Event* constevent;
  m_geantSvc->retrieveEvent(constevent).ignore();
  for (auto& tool : m_saveTools) {
    tool->saveOutput(*constevent).ignore();
  }
  m_geantSvc->terminateEvent().ignore();
  return StatusCode::SUCCESS;
}

StatusCode SimAlg::finalize() { return GaudiAlgorithm::finalize(); }
