#pragma once

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

// DD4hep
#include "DD4hep/Segmentations.h"
#include "DD4hep/VolumeManager.h"
#include "DDRec/Surface.h"

//
#include "edm4hep/MutableTrackerHit.h"
#include <DD4hep/Objects.h>
#include <Parsers/Primitives.h>
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/Vector3d.h>
#include <map>

/** @class TpcSegmentAlg
 *
 *  Simulate the primary ionization and electron diffusion process.
 *  The output is a collection of electrons with smeared [x, y, z]
 *  vertex caused by diffusion.
 *
 *  The algorithm is fully parameterized, no Garfield component needed.
 *
 *  Adapted from REST TRestDetectorElectronDiffusionProcess
 *
 *  @author Yong Zhou
 */

class IGeoSvc;

namespace edm4hep {
  class SimTrackerHitCollection;
  class TrackerHitCollection;
  class MutableTrackerHit;
  class Vector3d;
} // namespace edm4hep

class TpcSegmentAlg : public GaudiAlgorithm {
public:
  explicit TpcSegmentAlg( const std::string&, ISvcLocator* );
  virtual ~TpcSegmentAlg();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  void add_hit( edm4hep::SimTrackerHit hit, dd4hep::rec::ISurface* pcb, dd4hep::Position gpos,
                dd4hep::DDSegmentation::CellID volId );

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Data service: needed to register objects and get collection IDs. Just an observing pointer.
  PodioDataSvc* m_podioDataSvc;
  /// volume manager
  dd4hep::VolumeManager m_volMgr;

  struct HitCache {
    edm4hep::MutableTrackerHit hit;
    float                      energy;
    float                      time;
    dd4hep::Position           position;
  };
  std::map<dd4hep::DDSegmentation::CellID, HitCache> m_hitCache;

  /// Segmentation
  bool                 is_stripSeg{ false };
  dd4hep::Segmentation m_segmentation;

  /// Handle to EDM collection
  Gaudi::Property<std::string> m_newField{
      this, "newField", "layer",
      "The name of newly inserted field: for strip readout, it indicates the layer of the "
      "two plane; for pixel "
      "readout, it may indicate the pcb index if multiple pcbs are used, otherwise this "
      "field has no effect. The "
      "name "
      "should match the id specification in compact xml." };
  DataHandle<edm4hep::SimTrackerHitCollection> m_inHits{ "TpcDriftHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::TrackerHitCollection>    m_outHits{ "", Gaudi::DataHandle::Writer, this };

  /// Properties
  Gaudi::Property<std::string> m_readoutName{ this, "readoutName", "TpcPixelHits",
                                              "The readout name defined in compact xml" };
};
