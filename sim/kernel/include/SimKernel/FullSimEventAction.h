#pragma once

#include "G4UserEventAction.hh"

namespace megat {

  namespace sim {
    class FullSimEventAction : public G4UserEventAction {
    public:
      FullSimEventAction();
      virtual ~FullSimEventAction() = default;

      virtual void BeginOfEventAction( const G4Event* anEvent );
      virtual void EndOfEventAction( const G4Event* anEvent );
    };
  } // namespace sim
} // namespace megat
