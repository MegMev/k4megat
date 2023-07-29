#include "SimSaveCalHits.h"

// FCCSW
#include "SimInterface/IGeoSvc.h"
#include "SimKernel/Geant4CaloHit.h"
#include "SimKernel/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "edm4hep/SimCalorimeterHitCollection.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Segmentations.h"

namespace megat {

  DECLARE_COMPONENT_WITH_ID( SimSaveCalHits, "SimSaveCalHits" )

  SimSaveCalHits::SimSaveCalHits( const std::string& aType, const std::string& aName, const IInterface* aParent )
      : GaudiTool( aType, aName, aParent )
      , m_geoSvc( "MegatGeoSvc", aName )
      , m_eventDataSvc( "EventDataSvc", "SimSaveCalHits" ) {
    declareInterface<ISimSaveOutputTool>( this );
    declareProperty( "CaloHits", m_caloHits, "Handle for calo hits" );
    declareProperty( "GeoSvc", m_geoSvc );
  }

  SimSaveCalHits::~SimSaveCalHits() {}

  StatusCode SimSaveCalHits::initialize() {
    if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }
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

    StatusCode sc  = m_eventDataSvc.retrieve();
    m_podioDataSvc = dynamic_cast<PodioLegacyDataSvc*>( m_eventDataSvc.get() );
    if ( sc == StatusCode::FAILURE ) {
      error() << "Error retrieving Event Data Service" << endmsg;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode SimSaveCalHits::finalize() { return GaudiTool::finalize(); }

  StatusCode SimSaveCalHits::saveOutput( const G4Event& aEvent ) {
    G4HCofThisEvent*    collections = aEvent.GetHCofThisEvent();
    G4VHitsCollection*  collect;
    sim::Geant4CaloHit* hit;
    if ( collections != nullptr ) {
      auto edmHits = m_caloHits.createAndPut();
      for ( int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++ ) {
        collect = collections->GetHC( iter_coll );
        if ( std::find( m_readoutNames.begin(), m_readoutNames.end(), collect->GetName() ) != m_readoutNames.end() ) {
          // Add CellID encoding string to collection metadata
          auto lcdd        = m_geoSvc->lcdd();
          auto allReadouts = lcdd->readouts();
          auto idspec      = lcdd->idSpecification( collect->GetName() );
          auto field_str   = idspec.fieldDescription();
          //          auto& coll_md     = m_podioDataSvc->getProvider().getCollectionMetaData( m_caloHits.get()->getID()
          //          ); coll_md.setValue( "CellIDEncodingString", field_str );

          size_t n_hit = collect->GetSize();
          debug() << "\t" << n_hit << " hits are stored in a collection #" << iter_coll << ": " << collect->GetName()
                  << endmsg;
          for ( size_t iter_hit = 0; iter_hit < n_hit; iter_hit++ ) {
            hit         = dynamic_cast<sim::Geant4CaloHit*>( collect->GetHit( iter_hit ) );
            auto edmHit = edmHits->create();
            edmHit.setCellID( hit->cellID );
            // todo
            // edmHitCore.bits = hit->trackId;
            edmHit.setEnergy( hit->energyDeposit * g42edm::energy );
            edmHit.setPosition( {
                (float)hit->position.x() * (float)g42edm::length,
                (float)hit->position.y() * (float)g42edm::length,
                (float)hit->position.z() * (float)g42edm::length,
            } );
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }
} // namespace megat
