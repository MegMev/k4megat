#include "GaudiAlg/GaudiTool.h"
#include "SimInterface/IGeoSvc.h"
#include "SimInterface/ISimSaveOutputTool.h"
#include "SimKernel/Geant4PreDigiTrackHit.h"
#include "SimKernel/Units.h"
#include "k4FWCore/DataHandle.h"
#include "k4FWCore/MetaDataHandle.h"

#include "G4Event.hh"

#include "DD4hep/Detector.h"
#include "edm4hep/Constants.h"
#include "edm4hep/SimTrackerHitCollection.h"

namespace megat {

  class SimSaveTrackerHits : public GaudiTool, virtual public ISimSaveOutputTool {

  public:
    explicit SimSaveTrackerHits( const std::string& aType, const std::string& aName, const IInterface* aParent )
        : GaudiTool( aType, aName, aParent ) {
      declareInterface<ISimSaveOutputTool>( this );
      declareProperty( "Hits", m_trackHits, "Collection Name of Tracker Hits" );
    }
    ~SimSaveTrackerHits() = default;

    StatusCode initialize() final {
      if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

      ServiceHandle<IGeoSvc> m_geoSvc( "GeoSvc", "SimSaveTrackerHits" );
      if ( !m_geoSvc ) {
        error() << "Unable to locate Geometry Service. "
                << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
        return StatusCode::FAILURE;
      }
      auto lcdd        = m_geoSvc->lcdd();
      auto allReadouts = lcdd->readouts();
      if ( allReadouts.find( m_readoutName ) == allReadouts.end() ) {
        error() << "Readout " << m_readoutName << " not found! Please check tool configuration." << endmsg;
        return StatusCode::FAILURE;
      }

      MetaDataHandle<std::string> m_cellIDHandle{ m_trackHits, edm4hep::CellIDEncoding, Gaudi::DataHandle::Writer };
      auto                        idspec = lcdd->idSpecification( m_readoutName );
      m_cellIDHandle.put( idspec.fieldDescription() );

      return StatusCode::SUCCESS;
    }

    StatusCode saveOutput( const G4Event& aEvent ) final {
      G4HCofThisEvent*            collections = aEvent.GetHCofThisEvent();
      G4VHitsCollection*          collect;
      sim::Geant4PreDigiTrackHit* hit;
      if ( collections != nullptr ) {
        edm4hep::SimTrackerHitCollection* edmHits = m_trackHits.createAndPut();
        for ( int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++ ) {
          collect = collections->GetHC( iter_coll );
          if ( m_readoutName == collect->GetName() ) {
            size_t n_hit = collect->GetSize();
            debug() << "\t" << n_hit << " hits are stored in a tracker collection #" << iter_coll << ": "
                    << collect->GetName() << endmsg;
            for ( size_t iter_hit = 0; iter_hit < n_hit; iter_hit++ ) {
              hit         = dynamic_cast<sim::Geant4PreDigiTrackHit*>( collect->GetHit( iter_hit ) );
              auto edmHit = edmHits->create();
              edmHit.setCellID( hit->cellID );
              edmHit.setEDep( hit->energyDeposit * g42edm::energy );
              /// workaround, store trackid in an unrelated field
              edmHit.setQuality( hit->trackId );
              edmHit.setTime( hit->time );
              edmHit.setPosition( {
                  hit->prePos.x() * g42edm::length,
                  hit->prePos.y() * g42edm::length,
                  hit->prePos.z() * g42edm::length,
              } );
              CLHEP::Hep3Vector diff = hit->postPos - hit->prePos;
              edmHit.setMomentum( {
                  (float)( diff.x() * g42edm::length ),
                  (float)( diff.y() * g42edm::length ),
                  (float)( diff.z() * g42edm::length ),
              } );
              edmHit.setPathLength( diff.mag() );
            }
          }
        }
      }
      return StatusCode::SUCCESS;
    }

  private:
    DataHandle<edm4hep::SimTrackerHitCollection> m_trackHits{ "TrackerSimHits", Gaudi::DataHandle::Writer, this };
    Gaudi::Property<std::string>                 m_readoutName{ this, "readoutName", "TrakerHits",
                                                "Name of the readout (hits collection) to save" };
  };

  DECLARE_COMPONENT_WITH_ID( SimSaveTrackerHits, "SimSaveTrackerHits" )
} // namespace megat
