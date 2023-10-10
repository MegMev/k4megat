#include "SimKernel/EventInformation.h"

#include "G4Track.hh"

#include "SimKernel/Units.h"

#include "edm4hep/MCParticleCollection.h"

namespace megat {

  namespace sim {
    EventInformation::EventInformation() { m_mcParticles = new edm4hep::MCParticleCollection(); }

    void EventInformation::setCollections( edm4hep::MCParticleCollection*& aMCParticleCollection ) {
      // ownership is transferred here - to SaveTool which is supposed to put it in the event store
      aMCParticleCollection = m_mcParticles;
    }

    void EventInformation::addParticle( const G4Track* aSecondary ) {
      auto  edmParticle = m_mcParticles->create();
      auto  g4mom       = aSecondary->GetMomentum();
      auto  g4energy    = aSecondary->GetTotalEnergy();
      float mass        = g4energy * g4energy - g4mom.mag2();
      mass              = sqrt( fabs( mass ) );
      size_t g4ID       = aSecondary->GetTrackID();

      edmParticle.setMomentum( {
          (float)g4mom.x() * (float)g42edm::energy,
          (float)g4mom.y() * (float)g42edm::energy,
          (float)g4mom.z() * (float)g42edm::energy,
      } );
      edmParticle.setMass( mass * g42edm::energy );
      edmParticle.setSimulatorStatus( g4ID );
      edmParticle.setPDG( aSecondary->GetDynamicParticle()->GetDefinition()->GetPDGEncoding() );

      auto g4EndPos = aSecondary->GetPosition();
      edmParticle.setEndpoint( {
          g4EndPos.x() * g42edm::length,
          g4EndPos.y() * g42edm::length,
          g4EndPos.z() * g42edm::length,
      } );
      edmParticle.setTime( aSecondary->GetGlobalTime() * g42edm::length );

      auto g4StartPos = aSecondary->GetVertexPosition();
      edmParticle.setVertex( {
          g4StartPos.x() * g42edm::length,
          g4StartPos.y() * g42edm::length,
          g4StartPos.z() * g42edm::length,
      } );

      // TODO: no time at endpoint, no parents/daughter relations
    }

  } // namespace sim
} // namespace megat
