#include "SimInterface/IGeoSvc.h"

// edm4hep
#include "SimKernel/Units.h"
#include "TpcSegmentAlg.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackerHitCollection.h"

// DD4hep
#include "DD4hep/BitFieldCoder.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DDRec/Surface.h"
#include "DDRec/SurfaceHelper.h"
#include "DDRec/SurfaceManager.h"
#include <DD4hep/Objects.h>
#include <DDSegmentation/BitFieldCoder.h>
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/StatusCode.h>
#include <Parsers/Primitives.h>
#include <cerrno>
#include <edm4hep/MutableTrackerHit.h>
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/Vector3d.h>
#include <edm4hep/utils/vector_utils.h>

using namespace megat;
using namespace dd4hep;
using namespace dd4hep::rec;

static constexpr double max_boundary = std::numeric_limits<double>::max();

DECLARE_COMPONENT( TpcSegmentAlg )

TpcSegmentAlg::TpcSegmentAlg( const std::string& aName, ISvcLocator* aSvcLoc )
    : GaudiAlgorithm( aName, aSvcLoc ), m_geoSvc( "MegatGeoSvc", aName ) {
  declareProperty( "inHits", m_inHits, "Simulated Hit Collection of Tpc" );
  declareProperty( "outHits", m_outHits, "Segmented Hit Collection of Tpc" );
}
TpcSegmentAlg::~TpcSegmentAlg() {}

StatusCode TpcSegmentAlg::initialize() {
  /// mandatory
  if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

  /// services
  if ( !m_geoSvc ) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  /// get volume manager
  m_volMgr = m_geoSvc->getVolumeManager();

  // cast is needed to use some un-interface methods
  m_podioDataSvc = dynamic_cast<PodioLegacyDataSvc*>( evtSvc().get() );
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
  if ( m_segmentation.type() == "MultiSegmentation" ) is_stripSeg = true;
  info() << is_stripSeg << ", " << m_segmentation.type() << ", " << m_segmentation->fieldDescription() << endmsg;

  // // [optional todo:] seperate hit collection for sub-segmentation
  // auto& hit_cols = m_geoSvc->lcdd()->readout( m_readoutName )->hits;
  // for ( const auto& col : hit_cols ) {
  //   info() << col.name << ",  " << col.key << ", " << col.key_min << ", " << col.key_max << endmsg;
  // }

  // default out collection name is readoutName
  if ( m_outHits.objKey().empty() ) m_outHits.updateKey( m_readoutName );

  return StatusCode::SUCCESS;
}

StatusCode TpcSegmentAlg::execute() {
  // get input hits
  const auto in_hits = m_inHits.get();

  // segmented hits: dE is accumulated, drift hits are collected
  auto out_hits = m_outHits.createAndPut();
  for ( const auto& hit : *in_hits ) {
    // cellid is volumeID for TPC sim hits
    DDSegmentation::CellID volId = hit.getCellID();
    auto                   sL    = m_geoSvc->getSensitiveSurfList( volId );
    auto                   gpos  = hit.getPosition();

    // [optional todo: key_value from xml?]
    if ( is_stripSeg ) {
      if ( sL.size() > 1 ) {
        error() << "Can't segment multi PCBs with strip readout" << endmsg;
        return StatusCode::FAILURE;
      }
      auto pcb = sL.front();

      // x-layer (layer Nr hardcoded)
      m_segmentation.decoder()->set( volId, m_newField, 0 );
      add_hit( hit, pcb, gpos, volId );

      // y-layer
      m_segmentation.decoder()->set( volId, m_newField, 1 );
      add_hit( hit, pcb, gpos, volId );
    } else if ( sL.size() == 1 ) {
      auto pcb = sL.front();
      add_hit( hit, pcb, gpos, volId );
    } else {
      for ( auto pcb : sL ) {
        if ( pcb->insideBounds( { gpos.x, gpos.y, gpos.z }, max_boundary ) ) {
          m_segmentation.decoder()->set( volId, m_newField, pcb->id() );
          add_hit( hit, pcb, gpos, volId );
        }
      }
    }
  }

  // update: summed edep, earliest arrival time and edep-weighted truth position
  for ( auto& [key, value] : m_hitCache ) {
    value.hit.setEDep( value.energy );
    value.hit.setTime( value.time );
    value.hit.setPosition( value.position / value.energy );
    out_hits->push_back( value.hit );
  }
  m_hitCache.clear();

  // update: id specification string of output collection
  auto& collmd = m_podioDataSvc->getProvider().getCollectionMetaData( out_hits->getID() );
  collmd.setValue( "CellIDEncodingString", m_segmentation->fieldDescription() );

  return StatusCode::SUCCESS;
}

StatusCode TpcSegmentAlg::finalize() { return GaudiAlgorithm::finalize(); }

inline void TpcSegmentAlg::add_hit( edm4hep::SimTrackerHit hit, ISurface* pcb, edm4hep::Vector3d gpos,
                                    DDSegmentation::CellID volId ) {
  auto gpos_dd   = gpos * edm2dd::length;
  auto lpos      = pcb->globalToLocal( { gpos_dd.x, gpos_dd.y, gpos_dd.z } );
  auto newCellId = m_segmentation.cellID( { lpos.u(), lpos.v(), 0 }, {}, volId );

  if ( auto it = m_hitCache.find( newCellId ); it != m_hitCache.end() ) {
    auto& cache = it->second;
    cache.hit.addToRawHits( hit.getObjectID() );
    cache.energy += hit.getEDep();
    cache.position = cache.position + hit.getEDep() * gpos;
    if ( cache.time > hit.getTime() ) cache.time = hit.getTime();
  }

  auto& new_hit = m_hitCache[newCellId];
  new_hit.hit.setCellID( newCellId );
  new_hit.hit.addToRawHits( hit.getObjectID() );
  new_hit.energy = hit.getEDep();
  new_hit.time   = hit.getTime();
  // pseudo truth position
  new_hit.position = hit.getEDep() * gpos;
}
