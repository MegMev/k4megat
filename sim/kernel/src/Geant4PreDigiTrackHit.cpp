
#include "SimKernel/Geant4PreDigiTrackHit.h"

namespace megat {
namespace sim {

// G4 allocation method
G4ThreadLocal G4Allocator<Geant4PreDigiTrackHit>* Geant4PreDigiTrackHitAllocator = 0;
// Destructor
Geant4PreDigiTrackHit::~Geant4PreDigiTrackHit() {}
// Default Constructor
Geant4PreDigiTrackHit::Geant4PreDigiTrackHit() {}
// Constructor setting some members
Geant4PreDigiTrackHit::Geant4PreDigiTrackHit(unsigned int aTrackId, int aPdgId, double aEnergyDeposit,
                                             double aTime)
    : trackId(aTrackId), pdgId(aPdgId), energyDeposit(aEnergyDeposit), time(aTime) {}

// comparison operator
G4int Geant4PreDigiTrackHit::operator==(const Geant4PreDigiTrackHit& right) const { return (this == &right) ? 1 : 0; }

} // namespace sim
} // namespace megat
