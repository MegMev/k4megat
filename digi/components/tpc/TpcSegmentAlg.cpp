#include "GaudiAlg/GaudiAlgorithm.h"

#include "SimInterface/IGeoSvc.h"
#include "SimKernel/Units.h"

#include "edm4hep/Constants.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackerHitCollection.h"
#include "edm4hep/Vector3d.h"
#include "edm4hep/utils/vector_utils.h"

#include "k4FWCore/DataHandle.h"
#include "k4FWCore/MetaDataHandle.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/VolumeManager.h"
#include "DDRec/Surface.h"

#include <limits>
#include <map>

/** @class TpcSegmentAlg
 *
 *  Process:
 *  1. specify the readout segmentation (from =compact= xml): pixel or strip
 *  2. collect the drifted electrons to each readout cell
 *  3. save sum energy and e-weighted position for each cell
 *
 *  @author Yong Zhou
 */

using namespace megat;
using namespace dd4hep::rec;

static constexpr double max_boundary = std::numeric_limits<double>::max();

class TpcSegmentAlg : public GaudiAlgorithm {

public:
  explicit TpcSegmentAlg( const std::string& aName, ISvcLocator* aSvcLoc )
      : GaudiAlgorithm( aName, aSvcLoc ), m_geoSvc( "GeoSvc", aName ) {
    declareProperty( "inHits", m_inHits, "Simulated Hit Collection of Tpc" );
    declareProperty( "outHits", m_outHits, "Segmented Hit Collection of Tpc" );
  }

  StatusCode initialize() final {
    if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

    /// services
    if ( !m_geoSvc ) {
      error() << "Unable to locate Geometry Service. "
              << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
      return StatusCode::FAILURE;
    }

    /// segmentation
    auto lcdd        = m_geoSvc->lcdd();
    auto allReadouts = lcdd->readouts();
    if ( allReadouts.find( m_readoutName ) == allReadouts.end() ) {
      error() << "Readout " << m_readoutName << " not found! Please check configuration." << endmsg;
      return StatusCode::FAILURE;
    }

    m_segmentation = lcdd->readout( m_readoutName ).segmentation();
    if ( m_segmentation.type() == "MultiSegmentation" ) is_stripSeg = true;
    m_decoder = m_segmentation.decoder();
    debug() << is_stripSeg << ", " << m_segmentation.type() << ", " << m_segmentation->fieldDescription() << endmsg;

    /// update id specification
    MetaDataHandle<std::string> m_cellIDHandle{ m_outHits, edm4hep::CellIDEncoding, Gaudi::DataHandle::Writer };
    m_cellIDHandle.put( m_segmentation->fieldDescription() );

    /// default out collection name is readoutName
    if ( m_outHits.objKey().empty() ) m_outHits.updateKey( m_readoutName );

    // /// [optional todo:] seperate hit collection for sub-segmentation
    // auto& hit_cols = m_geoSvc->lcdd()->readout( m_readoutName )->hits;
    // for ( const auto& col : hit_cols ) {
    //   info() << col.name << ",  " << col.key << ", " << col.key_min << ", " << col.key_max << endmsg;
    // }

    return StatusCode::SUCCESS;
  }

  StatusCode execute() final {
    // get input hits
    const auto in_hits = m_inHits.get();

    // segmented hits: dE is accumulated, drift hits are collected
    auto out_hits = m_outHits.createAndPut();
    for ( const auto& hit : *in_hits ) {
      // cellid is volumeID for TPC sim hits
      auto volId = hit.getCellID();
      auto sL    = m_geoSvc->getSensitiveSurfList( volId );
      auto gpos  = hit.getPosition();

      // [optional todo: key_value from xml?]
      if ( is_stripSeg ) {
        if ( sL.size() > 1 ) {
          error() << "Can't segment multi PCBs with strip readout" << endmsg;
          return StatusCode::FAILURE;
        }
        auto pcb = sL.front();

        // x-layer (layer Nr hardcoded)
        m_decoder->set( volId, m_newField, 0 );
        add_hit( hit, pcb, gpos, volId );

        // y-layer
        m_decoder->set( volId, m_newField, 1 );
        add_hit( hit, pcb, gpos, volId );
      } else if ( sL.size() == 1 ) {
        auto pcb = sL.front();
        add_hit( hit, pcb, gpos, volId );
      } else {
        for ( auto pcb : sL ) {
          if ( pcb->insideBounds( { gpos.x, gpos.y, gpos.z }, max_boundary ) ) {
            m_decoder->set( volId, m_newField, pcb->id() );
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

    return StatusCode::SUCCESS;
  }

private:
  struct HitCache {
    edm4hep::MutableTrackerHit hit;
    float                      energy;
    float                      time;
    edm4hep::Vector3d          position;
  };
  std::map<dd4hep::DDSegmentation::CellID, HitCache> m_hitCache;

  inline void add_hit( edm4hep::SimTrackerHit hit, dd4hep::rec::ISurface* pcb, edm4hep::Vector3d gpos,
                       dd4hep::DDSegmentation::CellID volId ) {
    auto gpos_dd   = gpos * edm2dd::length;
    auto lpos      = pcb->globalToLocal( { gpos_dd.x, gpos_dd.y, gpos_dd.z } );
    auto newCellId = m_segmentation.cellID( { lpos.u(), lpos.v(), 0 }, {}, volId );

    if ( auto it = m_hitCache.find( newCellId ); it != m_hitCache.end() ) {
      auto& cache = it->second;
      cache.hit.addToRawHits( hit.getObjectID() );
      cache.energy += hit.getEDep();
      cache.position = cache.position + hit.getEDep() * gpos;
      if ( cache.time > hit.getTime() ) cache.time = hit.getTime();
    } else {
      auto& new_hit = m_hitCache[newCellId];
      new_hit.hit.setCellID( newCellId );
      new_hit.hit.addToRawHits( hit.getObjectID() );
      new_hit.energy = hit.getEDep();
      new_hit.time   = hit.getTime();
      // pseudo truth position
      new_hit.position = hit.getEDep() * gpos;
    }
  }

private:
  /// services
  ServiceHandle<IGeoSvc> m_geoSvc;

  /// edms
  DataHandle<edm4hep::SimTrackerHitCollection> m_inHits{ "TpcDriftHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::TrackerHitCollection>    m_outHits{ "", Gaudi::DataHandle::Writer, this };

  /// properties
  Gaudi::Property<std::string> m_newField{
      this, "newField", "layer",
      "The name of newly inserted field: for strip readout, it indicates the layer of the "
      "two plane; for pixel "
      "readout, it may indicate the pcb index if multiple pcbs are used, otherwise this "
      "field has no effect. The "
      "name "
      "should match the id specification in compact xml." };
  Gaudi::Property<std::string> m_readoutName{ this, "readoutName", "TpcPixelHits",
                                              "The readout name defined in compact xml" };

  /// others
  bool                         is_stripSeg{ false };
  const dd4hep::BitFieldCoder* m_decoder;
  dd4hep::Segmentation         m_segmentation;
};

DECLARE_COMPONENT( TpcSegmentAlg )
