#include "SimSensitive/BirksLawCalorimeterSD.h"

// FCCSW
#include "SimSensitive/Utils.h"
#include "SimKernel/Geant4CaloHit.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"
#include "DDG4/Defs.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"



namespace megat {
namespace sim {

BirksLawCalorimeterSD::BirksLawCalorimeterSD(const std::string& aDetectorName,
                                             const std::string& aReadoutName,
                                             const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName),
      m_calorimeterCollection(nullptr),
      m_seg(aSeg),
      // variables for birks law
      m_material("Polystyrene"),
      m_birk1(0.0130 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2)),
      m_birk2(9.6e-6 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2) * CLHEP::g / (CLHEP::MeV * CLHEP::cm2)) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

BirksLawCalorimeterSD::~BirksLawCalorimeterSD() {}

void BirksLawCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection =
      new G4THitsCollection<Geant4CaloHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection),
                                      m_calorimeterCollection);
}

bool BirksLawCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  G4double response = 0.;

  G4Material* material = aStep->GetPreStepPoint()->GetMaterial();
  G4double charge = aStep->GetPreStepPoint()->GetCharge();

  if ((charge != 0.) && (m_material.compare(material->GetName()) == 0)) {
    G4double rkb = m_birk1;
    // --- correction for particles with more than 1 charge unit ---
    // --- based on alpha particle data ---
    if (std::fabs(charge) > 1.0) rkb *= 7.2 / 12.6;

    if (aStep->GetStepLength() != 0) {
      G4double dedx = edep / (aStep->GetStepLength()) / (material->GetDensity());
      response = edep / (1. + rkb * dedx + m_birk2 * dedx * dedx);
    } else {
      response = edep;
    }
  } else {
    response = edep;
  }
  edep = response;


  const G4Track* track = aStep->GetTrack();
  // as in dd4hep::sim::Geant4GenericSD<Calorimeter>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  auto hit = new Geant4CaloHit(
      track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(), edep, track->GetGlobalTime());
  hit->cellID = utils::cellID(m_seg, *aStep, false);
  hit->position = prePos;
  hit->energyDeposit = edep;
  m_calorimeterCollection->insert(hit);
  return true;
}

} // namespace sim
} // namespace megat
