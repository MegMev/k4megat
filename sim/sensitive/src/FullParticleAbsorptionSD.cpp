#include "SimSensitive/FullParticleAbsorptionSD.h"

// FCCSW
#include "SimSensitive/Utils.h"
#include "SimKernel/Geant4CaloHit.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

namespace megat {
namespace sim {

FullParticleAbsorptionSD::FullParticleAbsorptionSD(const std::string& aDetectorName,
                                                   const std::string& aReadoutName,
                                                   const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_calorimeterCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

FullParticleAbsorptionSD::~FullParticleAbsorptionSD() {}

void FullParticleAbsorptionSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection = new G4THitsCollection<Geant4CaloHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection),
                                      m_calorimeterCollection);
}

bool FullParticleAbsorptionSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  G4Track* aTrack = aStep->GetTrack();
  G4double kineticEnergy = aTrack->GetKineticEnergy();
  auto hit = new Geant4CaloHit(
      aTrack->GetTrackID(), aTrack->GetDefinition()->GetPDGEncoding(), kineticEnergy, aTrack->GetGlobalTime());
  hit->cellID = utils::cellID(m_seg, *aStep, false);
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  hit->position = prePos;
  m_calorimeterCollection->insert(hit);
  // kill the track to ensure no double counting
  aTrack->SetTrackStatus(fStopAndKill);
  return true;
}

} // namespace sim
} // namespace megat
