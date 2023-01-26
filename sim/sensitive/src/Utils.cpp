#include "SimSensitive/Utils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// Geant
#include "G4NavigationHistory.hh"

namespace megat {
namespace utils {
uint64_t cellID(const dd4hep::Segmentation &aSeg, const G4Step &aStep,
                bool aPreStepPoint) {
  dd4hep::sim::Geant4VolumeManager volMgr =
      dd4hep::sim::Geant4Mapping::instance().volumeManager();
  dd4hep::VolumeID volID =
      volMgr.volumeID(aStep.GetPreStepPoint()->GetTouchable());
  if (aSeg.isValid()) {
    G4ThreeVector global;
    if (aPreStepPoint) {
      global = aStep.GetPreStepPoint()->GetPosition();
    } else {
      global = 0.5 * (aStep.GetPreStepPoint()->GetPosition() +
                      aStep.GetPostStepPoint()->GetPosition());
    }
    G4ThreeVector local = aStep.GetPreStepPoint()
                              ->GetTouchable()
                              ->GetHistory()
                              ->GetTopTransform()
                              .TransformPoint(global);
    dd4hep::Position loc(local.x() * MM_2_CM, local.y() * MM_2_CM,
                         local.z() * MM_2_CM);
    dd4hep::Position glob(global.x() * MM_2_CM, global.y() * MM_2_CM,
                          global.z() * MM_2_CM);
    dd4hep::VolumeID cID = aSeg.cellID(loc, glob, volID);
    return cID;
  }
  return volID;
}

} // namespace megat
} // namespace utils
