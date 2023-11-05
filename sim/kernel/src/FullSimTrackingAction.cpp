#include "SimKernel/FullSimTrackingAction.h"

#include "SimKernel/EventInformation.h"

#include "G4EventManager.hh"
#include "G4LorentzVector.hh"

namespace megat {

  namespace sim {

    FullSimTrackingAction::FullSimTrackingAction( double aEnergyCut ) : m_energyCut( aEnergyCut ) {}

    void FullSimTrackingAction::PreUserTrackingAction( const G4Track* aTrack ) {
      auto g4EvtMgr = G4EventManager::GetEventManager();
      auto evtinfo  = dynamic_cast<EventInformation*>( g4EvtMgr->GetUserInformation() );
      // Not used: should be handled in addParticle
      // G4LorentzVector prodPos( aTrack->GetGlobalTime() - aTrack->GetLocalTime(), aTrack->GetVertexPosition() );
      // G4LorentzVector endPos( aTrack->GetGlobalTime(), aTrack->GetPosition() );
      if ( selectSecondary( *aTrack, m_energyCut ) ) { evtinfo->addParticle( aTrack ); }
    }

    void FullSimTrackingAction::PostUserTrackingAction( const G4Track* /*aTrack*/ ) {}

    bool FullSimTrackingAction::selectSecondary( const G4Track& aTrack, double aEnergyCut ) {
      G4LorentzVector p4( aTrack.GetMomentum(), aTrack.GetTotalEnergy() );
      if ( p4.e() < aEnergyCut ) { return false; }
      return true;
    }
  } // namespace sim
} // namespace megat
