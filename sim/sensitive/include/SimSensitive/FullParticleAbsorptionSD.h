#ifndef MEGAT_SIM_FULLPARTICLEABSORPTIONSD_H
#define MEGAT_SIM_FULLPARTICLEABSORPTIONSD_H

// DD4hep
#include "DD4hep/Segmentations.h"
#include "DDSegmentation/Segmentation.h"

// Geant
#include "G4THitsCollection.hh"
#include "G4VSensitiveDetector.hh"

// megat
#include "SimG4Common/Geant4CaloHit.h"

/** FullParticleAbsD DetectorDescription/SimSensitive/src/FullParticleAbsSD.h
 * FullParticleAbsSD.h
 *
 *  Sensitive detector to fully stop the incoming particles.
 *  The position of the hit is set to G4Step::GetPreStepPoint() position.
 *  New hit is created for each incoming particle, the energy is stored and the
 * track is removed.
 *
 *  @author    Coralie Neubueser
 */

namespace megat {
namespace sim {

class FullParticleAbsorptionSD : public G4VSensitiveDetector {
public:
  /** Constructor.
   *  @param aDetectorName Name of the detector
   *  @param aReadoutName Name of the readout (used to name the collection)
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  FullParticleAbsorptionSD(const std::string& aDetectorName,
                           const std::string& aReadoutName,
                           const dd4hep::Segmentation& aSeg);
  /// Destructor
  virtual ~FullParticleAbsorptionSD();
  /** Initialization.
   *  Creates the hit collection with the name passed in the constructor.
   *  The hit collection is registered in Geant.
   *  @param aHitsCollections Geant hits collection.
   */
  virtual void Initialize(G4HCofThisEvent* aHitsCollections) final;
  /** Process hit once the particle hit the sensitive volume.
   *  Gets the kinetic energy from the particle track, calculates the position and cellID,
   *  saves that into the hit collection.
   *  New hit is created for first track of each particle within sensitive detector.
   *  @param aStep Step in which particle deposited the energy.
   */
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory*) final;

private:
  /// Collection of calorimeter hits
  G4THitsCollection<Geant4CaloHit>* m_calorimeterCollection;
  /// Segmentation of the detector used to retrieve the cell Ids
  dd4hep::Segmentation m_seg;
};

} // namespace sim
} // namespace megat
#endif /* MEGAT_SIM_FULLPARTICLEPABSORPTIONSD_H */

