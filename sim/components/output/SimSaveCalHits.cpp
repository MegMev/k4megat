#include "GaudiAlg/GaudiTool.h"

#include "SimInterface/IGeoSvc.h"
#include "SimInterface/ISimSaveOutputTool.h"
#include "SimKernel/Geant4CaloHit.h"
#include "SimKernel/Units.h"

#include "edm4hep/Constants.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "k4FWCore/DataHandle.h"
#include "k4FWCore/MetaDataHandle.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Segmentations.h"

#include "G4Event.hh"

namespace megat {

  class SimSaveCalHits : public GaudiTool, virtual public ISimSaveOutputTool {

  public:
    explicit SimSaveCalHits( const std::string& aType, const std::string& aName, const IInterface* aParent )
        : GaudiTool( aType, aName, aParent ) {
      declareInterface<ISimSaveOutputTool>( this );
      declareProperty( "Hits", m_caloHits, "Collection Name of Calorimeter Hits" );
    }
    ~SimSaveCalHits() = default;

    StatusCode initialize() final {
      if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

      ServiceHandle<IGeoSvc> m_geoSvc( "GeoSvc", "SimSaveCalHits" );
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

      MetaDataHandle<std::string> m_cellIDHandle{ m_caloHits, edm4hep::CellIDEncoding, Gaudi::DataHandle::Writer };
      auto                        idspec = lcdd->idSpecification( m_readoutName );
      m_cellIDHandle.put( idspec.fieldDescription() );

      return StatusCode::SUCCESS;
    }

    StatusCode saveOutput( const G4Event& aEvent ) final {
      G4HCofThisEvent*    collections = aEvent.GetHCofThisEvent();
      G4VHitsCollection*  collect;
      sim::Geant4CaloHit* hit;
      if ( collections != nullptr ) {
        auto edmHits = m_caloHits.createAndPut();
        for ( int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++ ) {
          collect = collections->GetHC( iter_coll );
          if ( m_readoutName == collect->GetName() ) {
            size_t n_hit = collect->GetSize();
            debug() << "\t" << n_hit << " hits are stored in a collection #" << iter_coll << ": " << collect->GetName()
                    << endmsg;
            for ( size_t iter_hit = 0; iter_hit < n_hit; iter_hit++ ) {
              hit         = dynamic_cast<sim::Geant4CaloHit*>( collect->GetHit( iter_hit ) );
              auto edmHit = edmHits->create();
              edmHit.setCellID( hit->cellID );
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

  private:
    /// edm
    DataHandle<edm4hep::SimCalorimeterHitCollection> m_caloHits{ "CaloSimHits", Gaudi::DataHandle::Writer, this };
    /// properties
    Gaudi::Property<std::string> m_readoutName{ this, "readoutName", "CaloHits",
                                                "Name of the readout (i.e. hits collection) to save" };
  };

  DECLARE_COMPONENT_WITH_ID( SimSaveCalHits, "SimSaveCalHits" )

} // namespace megat
