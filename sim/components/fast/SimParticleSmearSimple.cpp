#include "SimParticleSmearSimple.h"

// Gaudi

#include "GaudiKernel/IRndmGenSvc.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_COMPONENT(SimParticleSmearSimple)

SimParticleSmearSimple::SimParticleSmearSimple(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ISimParticleSmearTool>(this);
}

SimParticleSmearSimple::~SimParticleSmearSimple() {}

StatusCode SimParticleSmearSimple::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (service("RndmGenSvc", m_randSvc).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(1, m_sigma)).ignore();
  info() << "Tool used for smearing particles initialized with constant sigma = " << m_sigma << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode SimParticleSmearSimple::finalize() { return GaudiTool::finalize(); }

StatusCode SimParticleSmearSimple::smearMomentum(CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  double tmp = m_gauss.shoot();
  aMom *= tmp;
  return StatusCode::SUCCESS;
}
