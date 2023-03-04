#include "SimInterface/IGeoSvc.h"

// Gaudi

// edm4hep
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

using namespace dd4hep;
using namespace dd4hep::rec;

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
  if ( m_segmentation.type() == "MultiSegmentation" ) is_stripSeg = true;
  info() << is_stripSeg << ", " << m_segmentation.type() << ", " << m_segmentation->fieldDescription() << endmsg;

  // // [todo: optional] seperate hit collection for sub-segmentation
  // auto& hit_cols = m_geoSvc->lcdd()->readout( m_readoutName )->hits;
  // for ( const auto& col : hit_cols ) {
  //   info() << col.name << ",  " << col.key << ", " << col.key_min << ", " << col.key_max << endmsg;
  // }

  // default out collection name is readoutName
  if ( m_outHits.objKey().empty() ) m_outHits.updateKey( m_readoutName );

  // get the list of pcb surfaces of a subdetector
  // method1: using type string (not suitable for multiple TPC subdetector)
  // SurfaceManager&   surfMan = *m_geoSvc->lcdd()->extension<SurfaceManager>();
  // const SurfaceMap& surfMap = *surfMan.map( "tracker" );
  // for ( auto& surf : surfMap ) {
  //   auto type = surf.second->type();
  //   if ( type.isSensitive() ) info() << surf.first << "\t" << *( surf.second ) << endmsg;
  // }

  // method2: using volumeid (most general)
  DDSegmentation::BitFieldCoder bc( "system:6,pcb:6,strip:16:48" );
  long64                        tracker_id{ 222135545665454 };
  bc.set( tracker_id, "system", 1 );
  bc.set( tracker_id, "pcb", 3 );
  bc.set( tracker_id, "strip", 10 );
  // get top-level detector
  auto track_mgr = m_volMgr.subdetector( tracker_id );
  // auto tpc_de    = track_mgr.detector();
  info() << track_mgr << endmsg;

  // lower-level volumes
  auto vcon = m_volMgr.lookupContext( tracker_id );
  auto de   = vcon->element;
  auto vpv  = vcon->volumePlacement();
  auto epv  = vcon->elementPlacement();

  if ( vcon ) {
    info() << "find " << vcon->identifier << " vpv: " << vpv.toString() << " epv: " << epv.toString() << endmsg;
  }

  // if ( tpc_de.isValid() ) {
  //   SurfaceHelper surfMan( tpc_de );
  //   const auto&   sL = surfMan.surfaceList();
  //   for ( auto& surf : sL ) {
  //     auto type = surf->type();
  //     if ( type.isSensitive() ) info() << *( surf ) << endmsg;
  //   }
  // }

  // method3: using geosvc
  // auto sL = m_geoSvc->getSensitiveSurfList( tracker_id );
  // // auto sL = m_geoSvc->getHelperSurfList( tracker_id );
  // for ( auto& surf : sL ) {
  //   auto type = surf->type();
  //   info() << *( surf ) << endmsg;
  // }

  return StatusCode::SUCCESS;
}

StatusCode TpcSegmentAlg::execute() {
  // get input hits
  const auto in_hits = m_inHits.get();

  // create segmented hits: dE is accumulated, drift hits are collected
  auto out_hits = m_outHits.createAndPut();
  for ( const auto& hit : *in_hits ) {
    DDSegmentation::CellID volId = hit.getCellID(); // cellid is volumeID for TPC sim hits
    auto                   sL    = m_geoSvc->getSensitiveSurfList( volId );
    auto                   gpos  = hit.getPosition();

    // [todo: key_value from xml]
    if ( is_stripSeg ) {
      if ( sL.size() > 1 ) {
        error() << "Can't segment multi PCBs with strip readout" << endmsg;
        return StatusCode::FAILURE;
      }

      const auto& pcb = sL.front();
      // x-layer
      m_segmentation.decoder()->set( volId, m_newField, 0 );
      add_hit( hit, pcb, gpos, volId );

      // y-layer
      m_segmentation.decoder()->set( volId, m_newField, 1 );
      add_hit( hit, pcb, gpos, volId );
    } else if ( sL.size() == 1 ) {
      const auto& pcb = sL.front();
      add_hit( hit, pcb, gpos, volId );
    } else {
      for ( auto pcb : sL ) {
        if ( pcb->insideBounds( { gpos.x, gpos.y, gpos.z }, 9999 ) ) { // do not care about on surface or not
          m_segmentation.decoder()->set( volId, m_newField, pcb->id() );
          add_hit( hit, pcb, gpos, volId );
        }
      }
    }
  }

  // update the sum edep and arrive time
  for ( auto& [key, value] : m_hitCache ) {
    value.hit.setEDep( value.energy );
    value.hit.setTime( value.time );
    out_hits->push_back( value.hit );
  }
  m_hitCache.clear();

  // update the id specification string
  auto& collmd = m_podioDataSvc->getProvider().getCollectionMetaData( out_hits->getID() );
  collmd.setValue( "CellIDEncodingString", m_segmentation->fieldDescription() );

  return StatusCode::SUCCESS;
}

StatusCode TpcSegmentAlg::finalize() { return GaudiAlgorithm::finalize(); }

inline void TpcSegmentAlg::add_hit( edm4hep::SimTrackerHit hit, ISurface* pcb, edm4hep::Vector3d gpos,
                                    DDSegmentation::CellID volId ) {
  auto lpos      = pcb->globalToLocal( { gpos.x, gpos.y, gpos.z } );
  auto newCellId = m_segmentation.cellID( { lpos.u(), lpos.v(), 0 }, {}, volId );

  if ( auto it = m_hitCache.find( newCellId ); it != m_hitCache.end() ) {
    auto& cache = it->second;
    cache.hit.addToRawHits( hit.getObjectID() );
    cache.energy += hit.getEDep();
    if ( cache.time > hit.getTime() ) cache.time = hit.getTime();
  }

  auto& new_hit = m_hitCache[newCellId];
  new_hit.hit.setCellID( newCellId );
  new_hit.hit.setTime( hit.getTime() );
  new_hit.hit.setEDep( hit.getEDep() );
  auto strip_pos = m_segmentation.position( newCellId );
  // [todo: we need global cell position]
  new_hit.hit.setPosition( { strip_pos.x(), strip_pos.y(), strip_pos.z() } );
}
