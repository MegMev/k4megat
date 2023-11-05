#include "SimKernel/FullSimActions.h"
#include "SimKernel/FullSimEventAction.h"
#include "SimKernel/FullSimTrackingAction.h"
#include <iostream>

namespace megat {

  namespace sim {
    FullSimActions::FullSimActions( bool enableHistory, double aEnergyCut )
        : G4VUserActionInitialization(), m_enableHistory( enableHistory ), m_energyCut( aEnergyCut ) {}

    FullSimActions::~FullSimActions() {}

    void FullSimActions::Build() const {
      if ( m_enableHistory ) {
        SetUserAction( new FullSimEventAction() );
        SetUserAction( new FullSimTrackingAction( m_energyCut ) );
      }
    }
  } // namespace sim
} // namespace megat
