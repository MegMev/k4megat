#ifndef MEGAT_TpcTestDigi_H
#define MEGAT_TpcTestDigi_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

// DD4hep
#include "DD4hep/Segmentations.h"

/** @class TpcTestDigi
 *
 * This algorithm demo shows how to fetch a existing collection object from TES, add an custom offset
 * to the energy value, create and write to a new collection object in TES.
 *
 * See tut_edmio.py for usage.
 *
 *  @author Yong Zhou
 */

class IGeoSvc;

namespace edm4hep {
  class SimTrackerHitCollection;
} // namespace edm4hep

class TpcTestDigi : public GaudiAlgorithm {
public:
  explicit TpcTestDigi( const std::string&, ISvcLocator* );
  virtual ~TpcTestDigi();
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
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Data service: needed to register objects and get collection IDs. Just an observing pointer.
  PodioDataSvc* m_podioDataSvc;

  /// Segmentation
  bool                 is_multiSeg{ false };
  dd4hep::Segmentation m_segmentation;

  /// Handle to EDM collection
  DataHandle<edm4hep::SimTrackerHitCollection> m_inHits{ "TpcHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::SimTrackerHitCollection> m_outHits{ "", Gaudi::DataHandle::Writer, this };

  /// Properties
  Gaudi::Property<std::string> m_xyField{ this, "xyField", "layer",
                                          "The field name used to seperate X/Y plane, defined in compact xml" };
  Gaudi::Property<std::string> m_readoutName{ this, "readoutName", "TpcPixelHits",
                                              "The readout name defined in compact xml" };
};
#endif /* MEGAT_TpcTestDigi_H */
