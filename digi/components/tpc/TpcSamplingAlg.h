#pragma once

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// k4FWCore
#include "k4FWCore/DataHandle.h"

//
#include "SimKernel/Units.h"
#include <cmath>
#include <edm4hep/SimTrackerHitCollection.h>

/** @class TpcSamplingAlg
 *
 *  Demo sampling ADC:
 *  - time delay (param: m_timeDelay)
 *  - sample interval (param: m_sampleInterval)
 *  - sample points (param: m_nrPts)
 *  - shaping time (param: m_shapeTime)
 *  - gain factor for adapting to ADC range (param: m_gain)
 *
 *  Input: TrackerHit from TpcSegmentAlg
 *
 *  Output: TPCHit with an array of data words for each hit
 *
 *  Note:
 *  - only support 16-bit ADC currently
 *
 *  @author Yong Zhou
 */

namespace edm4hep {
  class SimTrackerHitCollection;
  class TrackerHitCollection;
  class TPCHitCollection;
} // namespace edm4hep

class TpcSamplingAlg : public GaudiAlgorithm {
public:
  explicit TpcSamplingAlg( const std::string&, ISvcLocator* );
  virtual ~TpcSamplingAlg();
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
  /// Handle to EDM collection
  DataHandle<edm4hep::TrackerHitCollection>    m_inHits{ "TpcSegHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::SimTrackerHitCollection> m_simHits{ "TpcDriftHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::TPCHitCollection>        m_outHits{ "TpcSampleHits", Gaudi::DataHandle::Writer, this };

  /// Properties
  Gaudi::Property<double> m_sampleInterval{ this, "sample_interval", 5, "[ns] Sample rate" };
  Gaudi::Property<double> m_shapeTime{ this, "shape_time", 5, "[us] Shaping time of electronics" };
  Gaudi::Property<int>    m_nrPts{ this, "nr_points", 512, "Number of sample points" };
  Gaudi::Property<double> m_gain{ this, "gain", 100, "[1/keV] Gain factor" };
  Gaudi::Property<int>    m_ampOffset{ this, "amplitude_offset", 0, "Amplitude offset" };
};
