#include "TpcTestDigi.h"
#include "SimInterface/IGeoSvc.h"

// Gaudi

// edm4hep
#include "edm4hep/SimTrackerHitCollection.h"

// DD4hep
#include "DD4hep/BitFieldCoder.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"
#include "DD4hep/detail/ObjectsInterna.h"

DECLARE_COMPONENT( TpcTestDigi )

TpcTestDigi::TpcTestDigi( const std::string& aName, ISvcLocator* aSvcLoc )
    : GaudiAlgorithm( aName, aSvcLoc ), m_geoSvc( "MegatGeoSvc", aName ) {
  declareProperty( "inHits", m_inHits, "Simulated Hit Collection of Tpc" );
  declareProperty( "outHits", m_outHits, "Segmented Hit Collection of Tpc" );
}
TpcTestDigi::~TpcTestDigi() {}

StatusCode TpcTestDigi::initialize() {
  /// mandatory
  if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

  /// services
  if ( !m_geoSvc ) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  // cast is needed to use some un-interface methods
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>( evtSvc().get() );
  if ( nullptr == m_podioDataSvc ) {
    error() << "Error retrieving Event Data Service" << endmsg;
    return StatusCode::FAILURE;
  }

  /// segmentation
  auto lcdd        = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  if ( allReadouts.find( m_readoutName ) == allReadouts.end() ) {
    error() << "Readout " << m_readoutName << " not found! Please check configuration." << endmsg;
    return StatusCode::FAILURE;
  } else {
    debug() << "Hits will be saved to EDM from the collection " << m_readoutName << endmsg;
  }
  m_segmentation = lcdd->readout( m_readoutName ).segmentation();
  if ( m_segmentation.type() == "MultiSegmentation" ) is_multiSeg = true;
  info() << is_multiSeg << ", " << m_segmentation.type() << ", " << m_segmentation->fieldDescription() << endmsg;

  // // [optional support] seperate hit collection for sub-segmentation
  // auto& hit_cols = m_geoSvc->lcdd()->readout( m_readoutName )->hits;
  // for ( const auto& col : hit_cols ) {
  //   info() << col.name << ",  " << col.key << ", " << col.key_min << ", " << col.key_max << endmsg;
  // }

  // default out collection name is readoutName
  if ( m_outHits.objKey().empty() ) m_outHits.updateKey( m_readoutName );

  return StatusCode::SUCCESS;
}

StatusCode TpcTestDigi::execute() {
  /// fetch the hit collection and fill
  const auto in_hits = m_inHits.get();
  // const auto                            cellIDstr = m_inHits.getCollMetadataCellID( in_hits->getID() );
  // dd4hep::DDSegmentation::BitFieldCoder decoder( cellIDstr );

  auto out_hits = m_outHits.createAndPut();
  for ( const auto& hit : *in_hits ) {
    dd4hep::DDSegmentation::CellID volId    = hit.getCellID();
    auto                           position = hit.getPosition();
    dd4hep::Position               pos{ position.x, position.y, position.z };

    if ( is_multiSeg ) {
      m_segmentation.decoder()->set( volId, m_xyField, 0 );
      auto newCellId = m_segmentation.cellID( pos, pos, volId );
      auto x_hit     = hit.clone();
      x_hit.setCellID( newCellId );
      out_hits->push_back( x_hit );

      m_segmentation.decoder()->set( volId, m_xyField, 1 );
      newCellId  = m_segmentation.cellID( pos, pos, volId );
      auto y_hit = hit.clone();
      y_hit.setCellID( newCellId );
      out_hits->push_back( y_hit );
    } else {
      auto newCellId = m_segmentation.cellID( pos, pos, volId );
      auto new_hit   = hit.clone();
      new_hit.setCellID( newCellId );
      out_hits->push_back( new_hit );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TpcTestDigi::finalize() { return GaudiAlgorithm::finalize(); }
