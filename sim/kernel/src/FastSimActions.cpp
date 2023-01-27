#include "SimKernel/FastSimActions.h"
namespace megat {
  namespace sim {
    FastSimActions::FastSimActions() : G4VUserActionInitialization() {}

    FastSimActions::~FastSimActions() {}

    void FastSimActions::Build() const {}
  } // namespace sim
} // namespace megat
