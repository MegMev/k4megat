#pragma once

#include "G4UserTrackingAction.hh"

namespace megat {

  namespace sim {
    class FullSimTrackingAction : public G4UserTrackingAction {
    public:
      FullSimTrackingAction( double energyCut );
      virtual ~FullSimTrackingAction() = default;

      void PreUserTrackingAction( const G4Track* aTrack );
      void PostUserTrackingAction( const G4Track* aTrack );

      /** Simple filter for particles to be saved, based on their energy.
       * @param[in] aTrack track of the particle to be saved
       * @param[in] aEnergyCut energy threshold above which particles are saved
       */
      bool selectSecondary( const G4Track& aTrack, double aEnergyCut );

    private:
      /// energy threshold for secondaries to be saved
      double m_energyCut;
    };
  } // namespace sim
} // namespace megat
