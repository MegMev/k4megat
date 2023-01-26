#include "SimParticleSmearFormula.h"

// Gaudi

#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/IRndmGenSvc.h"

// ROOT
#include "TFormula.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_COMPONENT(SimParticleSmearFormula)

SimParticleSmearFormula::SimParticleSmearFormula(const std::string& type, const std::string& name,
                                                     const IInterface* parent)
    : GaudiTool(type, name, parent), m_resolutionMomentum() {
  declareInterface<ISimParticleSmearTool>(this);
}

SimParticleSmearFormula::~SimParticleSmearFormula() {}

StatusCode SimParticleSmearFormula::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_randSvc = service("RndmGenSvc");
  if (!m_randSvc) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_resolutionMomentumStr.empty()) {
    m_resolutionMomentum = TFormula("pdep", m_resolutionMomentumStr.value().c_str());
    info() << "Momentum-dependent resolutions: " << m_resolutionMomentum.GetExpFormula() << endmsg;
  } else {
    info() << "No momentum-dependent resolutions defined." << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimParticleSmearFormula::finalize() { return GaudiTool::finalize(); }

StatusCode SimParticleSmearFormula::smearMomentum(CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  if (!m_resolutionMomentum.IsValid()) {
    error() << "Unable to smear particle's momentum - no resolution given!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_randSvc->generator(Rndm::Gauss(1, m_resolutionMomentum.Eval(aMom.mag())), m_gauss).ignore();
  double tmp = m_gauss->shoot();
  aMom *= tmp;
  return StatusCode::SUCCESS;
}
