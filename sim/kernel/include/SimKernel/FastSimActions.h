#ifndef SIMG4FAST_FASTSIMACTIONS_H
#define SIMG4FAST_FASTSIMACTIONS_H

#include <string>

// Geant4
#include "G4VUserActionInitialization.hh"

namespace megat {

  /** @class FastSimActions SimFast/SimFast/FastSimActions.h FastSimActions.h
   *  User action initialization for fast simulation.
   *  @author    Anna Zaborowska
   */

  namespace sim {
    class FastSimActions : public G4VUserActionInitialization {
    public:
      explicit FastSimActions();
      virtual ~FastSimActions();
      /// Create all user actions.
      virtual void Build() const final;
    };
  } // namespace sim
} // namespace megat

#endif /* SIMG4FAST_FASTSIMACTIONS_H */
