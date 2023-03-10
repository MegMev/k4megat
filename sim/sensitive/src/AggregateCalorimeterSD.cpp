#include "SimSensitive/AggregateCalorimeterSD.h"

// FCCSW
#include "SimSensitive/Utils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

namespace megat {
  namespace sim {
    AggregateCalorimeterSD::AggregateCalorimeterSD( const std::string& aDetectorName, const std::string& aReadoutName,
                                                    const dd4hep::Segmentation& aSeg )
        : G4VSensitiveDetector( aDetectorName ), m_calorimeterCollection( nullptr ), m_seg( aSeg ) {
      // name of the collection of hits is determined byt the readout name (from XML)
      collectionName.insert( aReadoutName );
    }

    AggregateCalorimeterSD::~AggregateCalorimeterSD() {}

    void AggregateCalorimeterSD::Initialize( G4HCofThisEvent* aHitsCollections ) {
      // create a collection of hits and add it to G4HCofThisEvent
      // deleted in ~G4Event
      m_calorimeterCollection = new G4THitsCollection<Geant4CaloHit>( SensitiveDetectorName, collectionName[0] );
      aHitsCollections->AddHitsCollection( G4SDManager::GetSDMpointer()->GetCollectionID( m_calorimeterCollection ),
                                           m_calorimeterCollection );
    }

    bool AggregateCalorimeterSD::ProcessHits( G4Step* aStep, G4TouchableHistory* ) {
      // check if energy was deposited
      G4double edep = aStep->GetTotalEnergyDeposit();
      if ( edep == 0. ) return false;

      // as in dd4hep::sim::Geant4GenericSD<Calorimeter>
      CLHEP::Hep3Vector prePos  = aStep->GetPreStepPoint()->GetPosition();
      CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
      CLHEP::Hep3Vector midPos  = 0.5 * ( postPos + prePos );
      // check the cell ID
      uint64_t       id       = utils::cellID( m_seg, *aStep, false );
      Geant4CaloHit* hit      = nullptr;
      Geant4CaloHit* hitMatch = nullptr;
      // Check if there is already some energy deposit in that cell
      for ( size_t i = 0; i < m_calorimeterCollection->entries(); i++ ) {
        hit = dynamic_cast<Geant4CaloHit*>( m_calorimeterCollection->GetHit( i ) );
        if ( hit->cellID == id ) {
          hitMatch = hit;
          hitMatch->energyDeposit += edep;
          return true;
        }
      }
      // if not, create a new hit
      // deleted in ~G4Event
      hitMatch = new Geant4CaloHit( 0, // track->GetTrackID()
                                    0, // track->GetDefinition()->GetPDGEncoding()
                                    edep,
                                    0 // track ->GetGlobalTime()
      );

      hitMatch->position = midPos;
      hitMatch->cellID   = id;
      m_calorimeterCollection->insert( hitMatch );
      return true;
    }

  } // namespace sim
} // namespace megat
