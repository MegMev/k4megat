#include "SimSaveTrackerHits.h"

// FCCSW
#include "SimInterface/IGeoSvc.h"
#include "SimKernel/Geant4PreDigiTrackHit.h"
#include "SimKernel/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "edm4hep/SimTrackerHitCollection.h"

// DD4hep
#include "DD4hep/Detector.h"

namespace megat {

  DECLARE_COMPONENT_WITH_ID( SimSaveTrackerHits, "SimSaveTrackerHits" )

  SimSaveTrackerHits::SimSaveTrackerHits( const std::string& aType, const std::string& aName,
                                          const IInterface* aParent )
      : GaudiTool( aType, aName, aParent )
      , m_geoSvc( "MegatGeoSvc", aName )
      , m_eventDataSvc( "EventDataSvc", "SimSaveTrackerHits" ) {
    declareInterface<ISimSaveOutputTool>( this );
    declareProperty( "SimTrackHits", m_trackHits, "Handle for tracker hits" );
    declareProperty( "MegatGeoSvc", m_geoSvc );
  }

  SimSaveTrackerHits::~SimSaveTrackerHits() {}

  StatusCode SimSaveTrackerHits::initialize() {
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
    m_podioDataSvc = dynamic_cast<PodioDataSvc*>( m_eventDataSvc.get() );
    if ( sc == StatusCode::FAILURE ) {
      error() << "Error retrieving Event Data Service" << endmsg;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode SimSaveTrackerHits::finalize() { return GaudiTool::finalize(); }

  StatusCode SimSaveTrackerHits::saveOutput( const G4Event& aEvent ) {
    G4HCofThisEvent*              collections = aEvent.GetHCofThisEvent();
    G4VHitsCollection*            collect;
    megat::Geant4PreDigiTrackHit* hit;
    if ( collections != nullptr ) {
      edm4hep::SimTrackerHitCollection* edmHits = m_trackHits.createAndPut();
      for ( int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++ ) {
        collect = collections->GetHC( iter_coll );
        if ( std::find( m_readoutNames.begin(), m_readoutNames.end(), collect->GetName() ) != m_readoutNames.end() ) {

          // Add CellID encoding string to collection metadata
          auto  lcdd        = m_geoSvc->lcdd();
          auto  allReadouts = lcdd->readouts();
          auto  idspec      = lcdd->idSpecification( collect->GetName() );
          auto  field_str   = idspec.fieldDescription();
          auto& coll_md     = m_podioDataSvc->getProvider().getCollectionMetaData( m_trackHits.get()->getID() );
          coll_md.setValue( "CellIDEncodingString", field_str );

          size_t n_hit = collect->GetSize();
          verbose() << "\t" << n_hit << " hits are stored in a tracker collection #" << iter_coll << ": "
                    << collect->GetName() << endmsg;
          for ( size_t iter_hit = 0; iter_hit < n_hit; iter_hit++ ) {
            hit         = dynamic_cast<megat::Geant4PreDigiTrackHit*>( collect->GetHit( iter_hit ) );
            auto edmHit = edmHits->create();
            edmHit.setCellID( hit->cellID );
            edmHit.setEDep( hit->energyDeposit * sim::g42edm::energy );
            /// workaround, store trackid in an unrelated field
            edmHit.setQuality( hit->trackId );
            edmHit.setTime( hit->time );
            edmHit.setPosition( {
                hit->prePos.x() * sim::g42edm::length,
                hit->prePos.y() * sim::g42edm::length,
                hit->prePos.z() * sim::g42edm::length,
            } );
            CLHEP::Hep3Vector diff = hit->postPos - hit->prePos;
            edmHit.setMomentum( {
                (float)( diff.x() * sim::g42edm::length ),
                (float)( diff.y() * sim::g42edm::length ),
                (float)( diff.z() * sim::g42edm::length ),
            } );
            edmHit.setPathLength( diff.mag() );
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }
} // namespace megat
