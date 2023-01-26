#include "SimGflashHomoCalo.h"

// Geant
#include "G4NistManager.hh"
#include "GFlashHomoShowerParameterisation.hh"

DECLARE_COMPONENT(SimGflashHomoCalo)

SimGflashHomoCalo::SimGflashHomoCalo(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {}

SimGflashHomoCalo::~SimGflashHomoCalo() {}

StatusCode SimGflashHomoCalo::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_material.empty()) {
    error() << "Material of the calorimeter is not defined." << endmsg;
    return StatusCode::FAILURE;
  }
  G4NistManager* nist = G4NistManager::Instance();
  if (nist->FindOrBuildMaterial(m_material.value()) == nullptr) {
    error() << "Material <" << m_material << "> is not found by G4NistManager." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimGflashHomoCalo::finalize() { return GaudiTool::finalize(); }

std::unique_ptr<GVFlashShowerParameterisation> SimGflashHomoCalo::parametrisation() {
  G4NistManager* nist = G4NistManager::Instance();
  std::unique_ptr<GVFlashShowerParameterisation> parametrisation = std::unique_ptr<GFlashHomoShowerParameterisation>(
      new GFlashHomoShowerParameterisation(nist->FindOrBuildMaterial(m_material.value())));

  // TODO move to sampling tool
  // G4NistManager* nist = G4NistManager::Instance();
  // m_parametrisation = std::unique_ptr<GFlashSamplingShowerParameterisation>(new GFlashSamplingShowerParameterisation(
  //     nist->FindOrBuildMaterial("G4_lAr"),  nist->FindOrBuildMaterial("G4_Pb"), 6, 2));

  return std::move(parametrisation);
}
