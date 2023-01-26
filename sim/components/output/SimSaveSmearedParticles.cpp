#include "SimSaveSmearedParticles.h"

// FCCSW
#include "SimKernel/ParticleInformation.h"
#include "SimKernel/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/MCRecoParticleAssociationCollection.h"

// DD4hep
#include "DD4hep/Segmentations.h"

DECLARE_COMPONENT(SimSaveSmearedParticles)

SimSaveSmearedParticles::SimSaveSmearedParticles(const std::string& aType, const std::string& aName,
                                                     const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimSaveOutputTool>(this);
  declareProperty("RecParticles", m_particles, "Handle for the particles to be written");
  declareProperty("MCRecoParticleAssoc", m_particlesMCparticles,
                  "Handle for the associations between particles and MC particles to be written");
}

SimSaveSmearedParticles::~SimSaveSmearedParticles() {}

StatusCode SimSaveSmearedParticles::initialize() { return GaudiTool::initialize(); }

StatusCode SimSaveSmearedParticles::finalize() { return GaudiTool::finalize(); }

StatusCode SimSaveSmearedParticles::saveOutput(const G4Event& aEvent) {
  auto particles = m_particles.createAndPut();
  auto associations = m_particlesMCparticles.createAndPut();
  int n_part = 0;
  for (int i = 0; i < aEvent.GetNumberOfPrimaryVertex(); i++) {
    for (int j = 0; j < aEvent.GetPrimaryVertex(i)->GetNumberOfParticle(); j++) {
      const G4PrimaryParticle* g4particle = aEvent.GetPrimaryVertex(i)->GetPrimary(j);
      sim::ParticleInformation* info = dynamic_cast<sim::ParticleInformation*>(g4particle->GetUserInformation());
      if (info->smeared()) {
        auto & MCparticle = info->mcParticle();
        auto particle = particles->create();
        auto association = associations->create();
        association.setRec(particle);
        association.setSim(MCparticle);
        particle.setCharge(g4particle->GetCharge());
        particle.setMomentum({
          (float) (info->endMomentum().x() * sim::g42edm::energy),
          (float) (info->endMomentum().y() * sim::g42edm::energy),
          (float) (info->endMomentum().z() * sim::g42edm::energy),
        });
        particle.setMass(g4particle->GetMass() * sim::g42edm::energy);
        particle.setReferencePoint({
            (float) (info->vertexPosition().x() * sim::g42edm::length),
            (float) (info->vertexPosition().y() * sim::g42edm::length),
            (float) (info->vertexPosition().z() * sim::g42edm::length),
        });
        n_part++;
      }
    }
  }
  debug() << "\t" << n_part << " particles are stored in smeared particles collection" << endmsg;
  return StatusCode::SUCCESS;
}
