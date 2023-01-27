#include "InspectHitsCollectionsTool.h"

// FCCSW
#include "SimInterface/IGeoSvc.h"
#include "SimKernel/Geant4CaloHit.h"
#include "SimKernel/Geant4PreDigiTrackHit.h"

// Geant
#include "G4Event.hh"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Segmentations.h"

namespace megat {

  DECLARE_COMPONENT_WITH_ID( InspectHitsCollectionsTool, "InspectHitsColTool" )

  InspectHitsCollectionsTool::InspectHitsCollectionsTool( const std::string& aType, const std::string& aName,
                                                          const IInterface* aParent )
      : GaudiTool( aType, aName, aParent ), m_geoSvc( "MegatGeoSvc", aName ) {
    declareInterface<ISimSaveOutputTool>( this );
  }

  InspectHitsCollectionsTool::~InspectHitsCollectionsTool() {}

  StatusCode InspectHitsCollectionsTool::initialize() {
    if ( GaudiTool::initialize().isFailure() ) {
      error() << "Unable to initialize Service()" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( !m_geoSvc ) {
      error() << "Unable to locate Geometry Service. "
              << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
      return StatusCode::FAILURE;
    }
    auto lcdd        = m_geoSvc->lcdd();
    auto allReadouts = lcdd->readouts();
    for ( auto& readoutName : m_readoutNames ) {
      if ( allReadouts.find( readoutName ) == allReadouts.end() ) {
        error() << "Readout " << readoutName << " not found! Please check tool configuration." << endmsg;
        return StatusCode::FAILURE;
      } else {
        debug() << "Hits will be saved to EDM from the collection " << readoutName << endmsg;
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode InspectHitsCollectionsTool::finalize() { return GaudiTool::finalize(); }

  StatusCode InspectHitsCollectionsTool::saveOutput( const G4Event& aEvent ) {
    G4HCofThisEvent*              collections = aEvent.GetHCofThisEvent();
    G4VHitsCollection*            collect;
    sim::Geant4PreDigiTrackHit* hitT;
    sim::Geant4CaloHit*         hitC;
    info() << "Obtaining hits collections that are stored in this event:" << endmsg;
    if ( collections != nullptr ) {
      for ( int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++ ) {
        collect = collections->GetHC( iter_coll );
        if ( std::find( m_readoutNames.begin(), m_readoutNames.end(), collect->GetName() ) != m_readoutNames.end() ) {
          info() << "\tcollection #: " << iter_coll << "\tname: " << collect->GetName()
                 << "\tsize: " << collect->GetSize() << endmsg;
          size_t n_hit   = collect->GetSize();
          auto   decoder = m_geoSvc->lcdd()->readout( collect->GetName() ).idSpec().decoder();
          for ( size_t iter_hit = 0; iter_hit < n_hit; iter_hit++ ) {
            hitT = dynamic_cast<sim::Geant4PreDigiTrackHit*>( collect->GetHit( iter_hit ) );
            if ( hitT ) {
              dd4hep::DDSegmentation::CellID cID = hitT->cellID;
              debug() << "hit Edep: " << hitT->energyDeposit << "\tcellID: " << cID << "\t"
                      << decoder->valueString( cID ) << endmsg;
            } else {
              hitC = dynamic_cast<sim::Geant4CaloHit*>( collect->GetHit( iter_hit ) );
              if ( hitC ) {
                dd4hep::DDSegmentation::CellID cID = hitC->cellID;
                debug() << "hit Edep: " << hitC->energyDeposit << "\tcellID: " << cID << "\t"
                        << decoder->valueString( cID ) << endmsg;
              }
            }
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }
} // namespace megat
