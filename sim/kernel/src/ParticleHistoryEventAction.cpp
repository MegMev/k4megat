#include "SimKernel/ParticleHistoryEventAction.h"

#include "SimKernel/EventInformation.h"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LorentzVector.hh"

namespace megat {

  namespace sim {

    ParticleHistoryEventAction::ParticleHistoryEventAction() {}

    void ParticleHistoryEventAction::BeginOfEventAction( const G4Event* /*anEvent*/ ) {

      auto eventInfo = new EventInformation();
      G4EventManager::GetEventManager()->SetUserInformation( eventInfo );
    }

    void ParticleHistoryEventAction::EndOfEventAction( const G4Event* /*anEvent*/ ) {}

  } // namespace sim
} // namespace megat
